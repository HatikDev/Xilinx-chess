#ifdef VGA

#define DMA_DEV_ID XPAR_AXIDMA_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID
#define FRAME_SIZE 2*480*641
#define FRAME_PARTS 40
#define FRAME_PART_SIZE FRAME_SIZE/FRAME_PARTS
#define INTC XScuGic
#define INTC_HANDLER XScuGic_InterruptHandler
#define TX_INTR_ID XPAR_FABRIC_AXI_DMA_0_MM2S_INTROUT_INTR
#define VGA_INTR_ID 62
#define FRAME_0_BASE (XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x00100000)
#define FRAME_1_BASE (XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x00110000)

XAxiDma AxiDma; /* Instance of the XAxiDma */
INTC Intc; /* Instance of the Interrupt Controller */

/* глобальные переменные, используемые в обработчике прерываний */
int frame_part = 0;
int new_frame_ready = 0;
u8 corent_frame = 1;
int SetupIntrSystem(INTC * IntcInstancePtr,XAxiDma * AxiDmaPtr, u16 TxIntrId, u16 RxIntrId);
void TxIntrHandler(void *Callback);
void Data_reqwestIntrHandler(void *Callback);

#endif
