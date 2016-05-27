/*
	GtkWidget2 - User-customizable widget class for GTK+ 2.x.
	
	Copyright (C) 2015-2016 Minho Jo <whitestone8214@openmailbox.org>

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 3 of the License, or (at your option) any later version.
	
	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the
	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.
*/

// gcc -O3 -std=gnu99 $(pkg-config --cflags --libs gtk+-2.0) gtkwidget2.c -shared -o libgtkwidget2.so


#include "gtkwidget2.h"


static void gtk_widget_2_callbacks (GtkWidget2Class *class);
static void gtk_widget_2_contents (GtkWidget2 *widget);
static void gtk_widget_2_size_request (GtkWidget *widget, GtkRequisition *requisition);
static void gtk_widget_2_size_allocate (GtkWidget *widget, GtkAllocation *allocation);
static void gtk_widget_2_show (GtkWidget *widget);
static gboolean gtk_widget_2_refresh (GtkWidget *widget, GdkEventExpose *event);
static void gtk_widget_2_destroy (GtkObject *object);


unsigned long gtk_widget_2_get_id () {
	static unsigned long id = 0; if (id == 0) {
		static const GtkTypeInfo information = {
			"GtkWidget2",
			sizeof (GtkWidget2),
			sizeof (GtkWidget2Class),
			(GtkClassInitFunc) gtk_widget_2_callbacks,
			(GtkObjectInitFunc) gtk_widget_2_contents,
			NULL,
			NULL,
			(GtkClassInitFunc) NULL
		};
		id = gtk_type_unique (gtk_widget_get_type (), &information);
	}
	
	return id;
}

static void gtk_widget_2_callbacks (GtkWidget2Class *class) {
	GtkWidgetClass *class_widget = (GtkWidgetClass *) class;
	class_widget->realize = gtk_widget_2_show;
	class_widget->size_request = gtk_widget_2_size_request;
	class_widget->size_allocate = gtk_widget_2_size_allocate;
	class_widget->expose_event = gtk_widget_2_refresh;
	
	GtkObjectClass *class_object = (GtkObjectClass *) class;
	class_object->destroy = gtk_widget_2_destroy;
}

static void gtk_widget_2_contents (GtkWidget2 *widget) {
	widget->index = 0; widget->label = NULL; widget->data = NULL;
	widget->width = 1; widget->height = 1; widget->fixedSize = 0;
	widget->x = 0; widget->y = 0; widget->fixedPosition = 0;
	widget->canvas = NULL; widget->customCanvas = 0;
	widget->customAttributes = 0;
	
	widget->fixed_size = 0; widget->fixed_position = 0; widget->custom_gdk_window = 0; widget->custom_window_attr = 0;
	
	widget->draw = NULL; widget->forget = NULL;
}

static void gtk_widget_2_size_request (GtkWidget *widget, GtkRequisition *requisition) {
	requisition->width = ((GtkWidget2 *) widget)->width; requisition->height = ((GtkWidget2 *) widget)->height;
}

static void gtk_widget_2_size_allocate (GtkWidget *widget, GtkAllocation *allocation) {
	widget->allocation = *allocation; GtkWidget2 *_widget = (GtkWidget2 *) widget;
	if (gtk_widget_get_realized (widget) == TRUE) {
		int _width, _height, _x, _y;
		if ((_widget->fixed_size == 1) || (_widget->fixedSize == 1)) {_width = _widget->width; _height = _widget->height;}
		else {_width = allocation->width; _height = allocation->height;}
		if ((_widget->fixed_position == 1) || (_widget->fixedPosition == 1)) {_x = _widget->x; _y = _widget->y;}
		else {_x = allocation->x; _y = allocation->y;}
		
		gdk_window_move_resize (widget->window, _x, _y, _width, _height);
		_widget->width = _width; _widget->height = _height; _widget->x = _x; _widget->y = _y;
	}
}

static void gtk_widget_2_show (GtkWidget *widget) {
	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
	
	GtkWidget2 *_widget = (GtkWidget2 *) widget;
	if ((_widget->custom_window_attr != 1) || (_widget->customAttributes != 1)) {
		int _width, _height, _x, _y;
		if ((_widget->fixed_size == 1) || (_widget->fixedSize == 1)) {_width = _widget->width; _height = _widget->height;}
		else {_width = widget->allocation.width; _height = widget->allocation.height;}
		if ((_widget->fixed_position == 1) || (_widget->fixedPosition == 1)) {_x = _widget->x; _y = _widget->y;}
		else {_x = widget->allocation.x; _y = widget->allocation.y;}
		_widget->settings.width = _width; _widget->settings.height = _height;
		_widget->settings.x = _x; _widget->settings.y = _y;
		_widget->settings.window_type = GDK_WINDOW_CHILD; _widget->settings.wclass = GDK_INPUT_OUTPUT;
		_widget->settings.event_mask = GDK_ALL_EVENTS_MASK;
	}
	
	GdkWindow *_canvas;
	if ((_widget->custom_gdk_window == 1) || (_widget->customCanvas == 1)) _canvas = _widget->canvas;
	else _canvas = gdk_window_new (gtk_widget_get_parent_window (widget), &(_widget->settings), GDK_WA_X | GDK_WA_Y);
	widget->window = _canvas;
	gdk_window_set_user_data (widget->window, widget);
	widget->style = gtk_style_attach (widget->style, widget->window);
	gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
}

static gboolean gtk_widget_2_refresh (GtkWidget *widget, GdkEventExpose *event) {
	if (((GtkWidget2 *) widget)->draw != NULL) return ((GtkWidget2 *) widget)->draw ((GtkWidget2 *) widget, event);
	else return FALSE;
}

static void gtk_widget_2_destroy (GtkObject *object) {
	GtkWidget2Class *class = (GtkWidget2Class *) gtk_type_class (gtk_widget_get_type ());
	if (((GtkWidget2 *) object)->forget != NULL) ((GtkWidget2 *) object)->forget ((GtkWidget2 *) object);
	if (GTK_OBJECT_CLASS (class)->destroy != NULL) (* GTK_OBJECT_CLASS (class)->destroy) (object);
}
