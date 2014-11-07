#include "rectangles.h"

Rectangle table_1, table_2;
Relation relation;

None
initialize (None)
{
	Value width  = 100.0;
	Value height =  50.0;

	table_1.position.x =  35;
	table_1.position.y =  35;
	table_2.position.x = 235;
	table_2.position.y = 235;

	table_1.size.width  = table_2.size.width  = width;
	table_1.size.height = table_2.size.height = height;

	relation.table_a = table_1;
	relation.table_b = table_2;
}

None
background_draw (cairo_t *context)
{
	cairo_set_source_rgba (context, 1.0, 1.0, 1.0, 1.0);
	cairo_paint (context);
}

None
rectangle_draw (cairo_t *context, Rectangle rectangle)
{
	Position position = rectangle.position;
	Size size = rectangle.size;

	cairo_rectangle (context, position.x, position.y, size.width, size.height);
	cairo_set_source_rgba (context, 0.2, 0.2, 0.2, 0.6);
	cairo_set_line_width (context, 4.0);
	cairo_set_dash (context, NULL, 0, 0.0);
	cairo_stroke (context);
	
	if (rectangle.is_crossing)
	{
		Value border = 3.5;
		Value dashes[] = { 2.5, 2.5, 2.5, 2.5 };
		gint dash = sizeof (dashes) / sizeof (dashes[0]);
		Value offset = -2.5;
	
		cairo_rectangle (context, position.x - border, position.y - border,
		                          size.width + 2 * border, size.height + 2 * border);
	
		if (rectangle.is_moving)
		{
			cairo_set_source_rgba (context, 0.8, 0.8, 0.8, 0.6);
			cairo_fill (context);
		}
		
		cairo_set_source_rgba (context, 0.2, 0.2, 0.2, 0.6);
		cairo_set_line_width (context, 1.0);
		cairo_set_dash (context, dashes, dash, offset);
		cairo_stroke (context);
	}
}

None
curve_draw (cairo_t *context, Curve curve)
{
	Line from = curve.from;
	Line to = curve.to;

	cairo_move_to (context, from.start.x, from.start.y);
	cairo_curve_to (context, from.end.x, from.end.y, to.start.x, to.start.y, to.end.x, to.end.y);

	cairo_set_source_rgba (context, 1.0, 0.0, 0.0, 1.0);
	cairo_set_line_width (context, 3.0);
	cairo_set_dash (context, NULL, 0, 0.0);
	cairo_stroke (context);
}

None
line_draw (cairo_t *context, Line line)
{
	cairo_move_to (context, line.start.x, line.start.y);
	cairo_line_to (context, line.end.x, line.end.y);

	cairo_set_source_rgba (context, 1.2, 0.2, 0.2, 0.6);
	cairo_set_line_width (context, 6.0);
	cairo_set_dash (context, NULL, 0, 0.0);
	cairo_stroke (context);
}

Point
get_connector_from_side (Rectangle rectangle, Side side)
{
	Point point;

	point.x = rectangle.position.x + (side == RIGHT ? rectangle.size.width : 0);
	point.y = rectangle.position.y + rectangle.size.height / 2.0;

	return point;
}

Value
get_middle_point (Rectangle rectangle_a, Rectangle rectangle_b)
{
	Position start = rectangle_a.position;
	Position end = rectangle_b.position;

	return (end.x - start.x) / 2.0;
}

Flag
is_overlap (Rectangle a, Rectangle b)
{
	return (a.position.x >= b.position.x &&
	        a.position.x <= b.position.x  + b.size.width) ||
	       (a.position.x  + a.size.width >= b.position.x &&
	        a.position.x  + a.size.width <= b.position.x +
	                                        b.size.width);
}

Curve
get_curve_from_relation (Relation relation)
{
	Side side_a = RIGHT;
	Side side_b = RIGHT;

	Table table_a = relation.table_a;
	Table table_b = relation.table_b;

	Flag overlap = is_overlap (table_a, table_b);
	Value middle = get_middle_point (table_a, table_b);

	if (overlap)
	{
		middle = (table_a.size.width + table_b.size.width) / 2.0;
	}
	else if (table_a.position.x + table_a.size.width < table_b.position.x)
	{
		side_b = LEFT;
	}
	else
	{
		side_a = LEFT;
	}

	Position start = get_connector_from_side (table_a, side_a);
	Position end   = get_connector_from_side (table_b, side_b);

	Curve curve;

	curve.from.start  = curve.from.end = start;
	curve.to.start    = curve.to.end   = end;

	curve.from.end.x += middle;
	curve.to.start.x += middle * (overlap ? 1 : -1);

	return curve;
}

None
relation_draw (cairo_t *context, Table table_a, Table table_b)
{
	Relation relation;

	relation.table_a = table_a;
	relation.table_b = table_b;

	Curve curve = get_curve_from_relation (relation);

	curve_draw (context, curve);

	//line_draw (context, curve.from);
	//line_draw (context, curve.to);
}

Flag
expose (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *context;

	context = gdk_cairo_create (widget->window);

	background_draw (context);
	rectangle_draw (context, table_1);
	rectangle_draw (context, table_2);
	relation_draw (context, table_1, table_2);

	cairo_destroy (context);

	return FALSE;
}

Flag
is_over (Point point, Rectangle rectangle)
{
	return point.x > rectangle.position.x &&
	       point.y > rectangle.position.y &&
	       point.x < rectangle.position.x + rectangle.size.width &&
	       point.y < rectangle.position.y + rectangle.size.height;
}

Flag
motion (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
	Position position;

	position.x = event->x;
	position.y = event->y;

	table_1.is_crossing = is_over (position, table_1);
	table_2.is_crossing = is_over (position, table_2);

	if (table_1.is_moving)
	{
		table_1.position.x = position.x - table_1.offset.x;
		table_1.position.y = position.y - table_1.offset.y;
	}

	if (table_2.is_moving)
	{
		table_2.position.x = position.x - table_2.offset.x;
		table_2.position.y = position.y - table_2.offset.y;
	}

	gtk_widget_queue_draw (widget);

	return FALSE;
}

Flag
press (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if ((table_1.is_moving = table_1.is_crossing))
	{
		table_1.offset.x = event->x - table_1.position.x; 
		table_1.offset.y = event->y - table_1.position.y; 
	}

	if ((table_2.is_moving = table_2.is_crossing))
	{
		table_2.offset.x = event->x - table_2.position.x; 
		table_2.offset.y = event->y - table_2.position.y; 
	}

	gtk_widget_queue_draw (widget);

	return FALSE;
}

Flag
release (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	table_1.is_moving = FALSE;
	table_2.is_moving = FALSE;

	gtk_widget_queue_draw (widget);

	return FALSE;
}

int
main (int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *canvas;

	initialize ();
	
	gtk_init (&argc, &argv);
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	canvas = gtk_drawing_area_new ();

	gtk_widget_set_size_request (window, 350, 300);

	gtk_widget_set_events (canvas, 0);

	gtk_widget_add_events (canvas, GDK_EXPOSURE_MASK);
	gtk_widget_add_events (canvas, GDK_POINTER_MOTION_MASK);
	gtk_widget_add_events (canvas, GDK_BUTTON_PRESS_MASK);
	gtk_widget_add_events (canvas, GDK_BUTTON_RELEASE_MASK);

	g_signal_connect (G_OBJECT (canvas), "expose-event", G_CALLBACK (expose), NULL);
	g_signal_connect (G_OBJECT (canvas), "button-press-event", G_CALLBACK (press), NULL);
	g_signal_connect (G_OBJECT (canvas), "button-release-event", G_CALLBACK (release), NULL);
	g_signal_connect (G_OBJECT (canvas), "motion-notify-event", G_CALLBACK (motion), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (gtk_main_quit), NULL);

	gtk_container_add (GTK_CONTAINER (window), canvas);

	gtk_widget_show_all (window);

	gtk_main ();

	return 0;
}

#ifdef WIN32
int APIENTRY
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main ((int) nCmdShow, (char **) lpCmdLine);
}
#endif
