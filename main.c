#ifdef TESTENVIRONMENT
#include <gtk/gtk.h>
#endif

#include "constants.h"
#include "graphics.h"
#include "testenvironment.h"
#include "types.h"

#ifdef TESTENVIRONMENT
uint16_t frame[FRAMESIZE];
#else
#include "vga.h"
#endif

struct Board board;

struct Pair selectedPosition;
struct Pair selectorPosition;

int state = NOT_SELECTED;
int turn = WHITE;

void drawAll()
{
    draw_board();

    draw_figures();

    draw_selector(selectorPosition.x, selectorPosition.y);
}

void move(int x1, int y1, int x2, int y2)
{
    int pos1 = y1 * 8 + x1;
    int pos2 = y2 * 8 + x2;

    board.cells[pos2] = board.cells[pos1];
    board.cells[pos1] = EMPTY;

    drawAll();
}

#ifdef TESTENVIRONMENT
static gboolean key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    gboolean *p_key_pressed = data;

    if (event->keyval == 'w' && selectorPosition.y > 0)
        --selectorPosition.y;
    if (event->keyval == 'a' && selectorPosition.x > 0)
        --selectorPosition.x;
    if (event->keyval == 's' && selectorPosition.y < 7)
        ++selectorPosition.y;
    if (event->keyval == 'd' && selectorPosition.x < 7)
        ++selectorPosition.x;

    int cellPos = selectorPosition.y * 8 + selectorPosition.x;
    if (event->keyval == ' ')
    {
        if (state == NOT_SELECTED && board.cells[cellPos] != EMPTY)
        {
            selectedPosition = selectorPosition;
            state = SELECTED;
        }
        else if (state == SELECTED)
        {
            move(selectedPosition.x, selectedPosition.y, selectorPosition.x, selectorPosition.y);
            state = NOT_SELECTED;
        }
    }

    drawAll();
    gtk_widget_queue_draw(widget);

    return TRUE;
}
#else
void keyHandler()
{
    key = inbyte();
    if (key == 'w' && selectorPosition.y > 0)
        --selectorPosition.y;
    if (key == 'a' && selectorPosition.x > 0)
        --selectorPosition.x;
    if (key == 's' && selectorPosition.y < 7)
        ++selectorPosition.y;
    if (key == 'd' && selectorPosition.x < 7)
        ++selectorPosition.x;
    if (key == ' ')
    {
        if (state == NOT_SELECTED && board.cells[cellPos] != EMPTY)
        {
            selectedPosition = selectorPosition;
            state = SELECTED;
        }
        else if (state == SELECTED)
        {
            makeMove(selectedPosition.x, selectedPosition.y, selectorPosition.x, selectorPosition.y);
            state = NOT_SELECTED;
        }
    }

    drawAll();

    xil_printf("%c",key);
}
#endif

#ifdef TESTENVIRONMENT
gint main(int argc,char *argv[])
{
    init_frame();
    drawAll();

    GtkWidget *window, *drawing_area;

    selectorPosition.x = 0;
    selectorPosition.y = 0;

    static gboolean key_pressed = FALSE, *p_key_pressed = &key_pressed;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER (window), drawing_area);
    gtk_widget_set_size_request(drawing_area, WINWIDTH, WINHEIGHT);
    g_signal_connect(G_OBJECT (drawing_area), "draw", G_CALLBACK (draw_callback), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(key_press_event), p_key_pressed);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
#else
int main()
{
    int Status;
    int Index;
    int x, y;
    u16* frame;
    u16 *frame0 = (u16*)FRAME_0_BASE,
    *frame1 = (u16*)FRAME_1_BASE;
    char key;
    Krug k = {640/2, 480/2, 1600, 0xFF0};
    init_platform();
    xil_printf("Start\n\r");
    XAxiDma_Config *CfgPtr;
    CfgPtr = XAxiDma_LookupConfig(DMA_DEV_ID);
    if (!CfgPtr) {
        xil_printf("No config found for %d\r\n", DMA_DEV_ID);
        return XST_FAILURE;
    }
    Status = XAxiDma_CfgInitialize(&AxiDma, CfgPtr);
    if (Status != XST_SUCCESS) {
        xil_printf("Initialization failed %d\r\n", Status);
        return XST_FAILURE;
    }
    if(XAxiDma_HasSg(&AxiDma)){
        xil_printf("Device configured as SG mode \r\n");
        return XST_FAILURE;
    }
    XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);
    /*Начальное заполнение кадра*/
    for(Index = 0; Index < FRAME_SIZE/2; Index++)
        frame0[Index] = frame1[Index] = 0x000;

    frame0[0] = frame1[0] = 0x2000;
    Xil_DCacheFlushRange((UINTPTR)frame0, FRAME_SIZE);
    Xil_DCacheFlushRange((UINTPTR)frame1, FRAME_SIZE);
    Status = SetupIntrSystem(&Intc, &AxiDma, TX_INTR_ID, VGA_INTR_ID);

    if (Status != XST_SUCCESS)
    {
        xil_printf("Failed intr setup\r\n");
        return XST_FAILURE;
    }
    /* Disable all interrupts before setup */
    XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
    XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    /* Enable all interrupts */
    XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
    XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    /* Основной цикл */
    while(1)
    {
        while(new_frame_ready);
        frame = !corent_frame ? frame1 : frame0;
        
        drawAll();
        /*for(Index = 0; Index < FRAME_SIZE/2; Index++)
        {
            x = Index%640 - k.x;
            y = Index/640 - k.y;
            if(x*x+y*y < k.r2)
                Frame_Base_Addr[Index] = k.col;
            else
                Frame_Base_Addr[Index] = 0x0000;
        }*/
        frame[0] = frame[0]|0x2000;
        Xil_DCacheFlushRange((UINTPTR)frame, FRAME_SIZE);
        new_frame_ready = 1;

        keyHandler();
    }
    cleanup_platform();
    return 0;
}
int SetupIntrSystem(INTC * IntcInstancePtr, XAxiDma * AxiDmaPtr, u16 TxIntrId, u16 Data_reqwestIntrId)
{
    int Status;
    XScuGic_Config *IntcConfig;
    IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    if (NULL == IntcConfig)
        return XST_FAILURE;

    Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
    IntcConfig->CpuBaseAddress);
    if (Status != XST_SUCCESS)
        return XST_FAILURE;

    XScuGic_SetPriorityTriggerType(IntcInstancePtr, TxIntrId, 0xA0, 0x3);
    XScuGic_SetPriorityTriggerType(IntcInstancePtr, Data_reqwestIntrId, 0xA0, 0x0);
    Status = XScuGic_Connect(IntcInstancePtr, TxIntrId, (Xil_InterruptHandler)TxIntrHandler, AxiDmaPtr);
    if (Status != XST_SUCCESS)
        return Status;

    Status = XScuGic_Connect(IntcInstancePtr, Data_reqwestIntrId, (Xil_InterruptHandler)Data_reqwestIntrHandler, AxiDmaPtr);
    if (Status != XST_SUCCESS)
        return Status;

    XScuGic_Enable(IntcInstancePtr, TxIntrId);
    XScuGic_Enable(IntcInstancePtr, Data_reqwestIntrId);
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
    (Xil_ExceptionHandler)INTC_HANDLER,
    (void *)IntcInstancePtr);
    Xil_ExceptionEnable();
    return 0;
}
void TxIntrHandler(void *Callback)
{
    XScuGic_Enable(&Intc, VGA_INTR_ID);
    XAxiDma *AxiDmaInst = (XAxiDma *)Callback;
    u32 IrqStatus = XAxiDma_IntrGetIrq(AxiDmaInst, XAXIDMA_DMA_TO_DEVICE);
    XAxiDma_IntrAckIrq(AxiDmaInst, IrqStatus, XAXIDMA_DMA_TO_DEVICE);
    if (frame_part == 0 && new_frame_ready == 1)
    {
        corent_frame = !corent_frame;
        new_frame_ready = 0;
    }
}
void Data_reqwestIntrHandler(void *Callback)
{
    u8* frame = corent_frame ? (u8 *)FRAME_1_BASE : (u8 *)FRAME_0_BASE;
    XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR) (frame + frame_part * FRAME_PART_SIZE), FRAME_PART_SIZE, XAXIDMA_DMA_TO_DEVICE);
    XScuGic_Disable(&Intc, VGA_INTR_ID);
    frame_part = (frame_part + 1)%FRAME_PARTS;
}
#endif
