#include <gtk/gtk.h>

#include "constants.h"
#include "graphics.h"
#include "testenvironment.h"
#include "types.h"

struct Board board;

uint16_t frame[FRAMESIZE];

struct Pair selectorPosition;

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

    gtk_widget_queue_draw(widget);

    return TRUE;
}

gint main(int argc,char *argv[])
{
    GtkWidget *window, *drawing_area;

    init_frame();

    selectorPosition.x = 0;
    selectorPosition.y = 0;

    static gboolean key_pressed = FALSE, *p_key_pressed = &key_pressed;

    gtk_init (&argc, &argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    drawing_area = gtk_drawing_area_new();
    gtk_container_add (GTK_CONTAINER (window), drawing_area);
    gtk_widget_set_size_request (drawing_area, WINWIDTH, WINHEIGHT);
    g_signal_connect (G_OBJECT (drawing_area), "draw", G_CALLBACK (draw_callback), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(key_press_event), p_key_pressed);
    gtk_widget_show_all (window);
    gtk_main ();
    return 0;
}
