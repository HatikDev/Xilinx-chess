#include "constants.h"
#include "testenvironment.h"
#include "types.h"

#ifdef TESTENVIRONMENT
extern uint16_t frame[FRAMESIZE];

extern struct Pair selectorPosition;

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    guint width, height;
    GtkStyleContext *context;

    context = gtk_widget_get_style_context (widget);
    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);
    gtk_render_background(context, cr, 0, 0, width, height);

    // test code for displaying frame

    int x;
    int y;
    GdkRGBA color;
    for (int i = 0; i < FRAMESIZE; ++i)
    {
        x = i % WIDTH;
        y = i / WIDTH;

        color.red = ((frame[i] & 0xf00) >> 8) / 16.;
        color.green = ((frame[i] & 0xf0) >> 4) / 16.;
        color.blue = (frame[i] & 0xf) / 16.;
        color.alpha = 1;
        gdk_cairo_set_source_rgba(cr, &color);
        cairo_rectangle(cr, x * CELLSIZE, y * CELLSIZE, CELLSIZE, CELLSIZE);

        cairo_fill (cr);
    }
    return FALSE;
}
#endif
