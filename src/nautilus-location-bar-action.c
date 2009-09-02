/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Nautilus
 *
 *  Copyright (C) 2004 Red Hat, Inc.
 *  Copyright (C) 2003 Marco Pesenti Gritti
 *
 *  Nautilus is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Nautilus is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Based on ephy-navigation-action.h from Epiphany
 *
 *  Authors: Alexander Larsson <alexl@redhat.com>
 *           Marco Pesenti Gritti
 *           Marcus Carlson <marcus@mejlamej.nu>
 *
 */

#include <config.h>

#include "nautilus-location-bar-action.h"
#include "nautilus-location-bar.h"
#include "nautilus-pathbar.h"
#include "nautilus-navigation-window.h"
#include "nautilus-window-private.h"
#include "nautilus-navigation-window-slot.h"
#include <gtk/gtk.h>
#include <eel/eel-gtk-extensions.h>

static void nautilus_location_bar_action_init       (NautilusLocationBarAction *action);
static void nautilus_location_bar_action_class_init (NautilusLocationBarActionClass *class);

static GObjectClass *parent_class = NULL;

#define NAUTILUS_LOCATION_BAR_ACTION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), NAUTILUS_TYPE_LOCATION_BAR_ACTION, NautilusLocationBarActionPrivate))

struct NautilusLocationBarActionPrivate
{
	NautilusNavigationWindow *window;
};

enum
{
	PROP_0,
	PROP_WINDOW
};

GType
nautilus_location_bar_action_get_type (void)
{
	static GType type = 0;

	if (type == 0) {
		const GTypeInfo type_info = {
			sizeof (NautilusLocationBarActionClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) nautilus_location_bar_action_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,
			sizeof (NautilusLocationBarAction),
			0, /* n_preallocs */
			(GInstanceInitFunc) nautilus_location_bar_action_init,
		};
		
		type = g_type_register_static (GTK_TYPE_ACTION,
					       "NautilusLocationBarAction",
					       &type_info, 0);
	}

	return type;
}


static GtkWidget *
create_tool_item (GtkAction *action)
{
        GtkToolItem *proxy;

        proxy = gtk_tool_item_new ();
        gtk_tool_item_set_expand (proxy, TRUE);
	gtk_widget_show (GTK_WIDGET (proxy));

        return GTK_WIDGET (proxy);
}

static void
connect_proxy (GtkAction *action, GtkWidget *proxy)
{
	if (GTK_IS_TOOL_ITEM (proxy)) {
		NautilusLocationBarAction *naction = NAUTILUS_LOCATION_BAR_ACTION (action);
                GtkWidget *bar;
                bar = naction->priv->window->details->location_bar_widget;
                if (bar->parent)
                        gtk_container_remove (GTK_CONTAINER (bar->parent), bar);

                gtk_container_add (GTK_CONTAINER (proxy), bar);
	}

	(* GTK_ACTION_CLASS (parent_class)->connect_proxy) (action, proxy);
}

static void
disconnect_proxy (GtkAction *action, GtkWidget *proxy)
{
	if (GTK_IS_TOOL_ITEM (proxy)) {
		NautilusLocationBarAction *naction = NAUTILUS_LOCATION_BAR_ACTION (action);
                GtkWidget *bar;
                bar = naction->priv->window->details->location_bar_widget;

                if (bar && bar->parent && bar->parent == proxy)
                        gtk_container_remove (GTK_CONTAINER (proxy), bar);
	}

	(* GTK_ACTION_CLASS (parent_class)->disconnect_proxy) (action, proxy);
}

static void
nautilus_location_bar_action_finalize (GObject *object)
{
	NautilusLocationBarAction *action = NAUTILUS_LOCATION_BAR_ACTION (object);

	(* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static void
nautilus_location_bar_action_set_property (GObject *object,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *pspec)
{
	NautilusLocationBarAction *nav;

	nav = NAUTILUS_LOCATION_BAR_ACTION (object);

	switch (prop_id)
	{
		case PROP_WINDOW:
			nav->priv->window = NAUTILUS_NAVIGATION_WINDOW (g_value_get_object (value));
			break;
	}
}

static void
nautilus_location_bar_action_get_property (GObject *object,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *pspec)
{
	NautilusLocationBarAction *nav;

	nav = NAUTILUS_LOCATION_BAR_ACTION (object);

	switch (prop_id)
	{
		case PROP_WINDOW:
			g_value_set_object (value, nav->priv->window);
			break;
	}
}

static void
nautilus_location_bar_action_class_init (NautilusLocationBarActionClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GtkActionClass *action_class = GTK_ACTION_CLASS (class);

	object_class->finalize = nautilus_location_bar_action_finalize;
	object_class->set_property = nautilus_location_bar_action_set_property;
	object_class->get_property = nautilus_location_bar_action_get_property;

	parent_class = g_type_class_peek_parent (class);

	action_class->toolbar_item_type = GTK_TYPE_MENU_TOOL_BUTTON;
	action_class->connect_proxy = connect_proxy;
	action_class->disconnect_proxy = disconnect_proxy;
	action_class->create_tool_item = create_tool_item;

	g_object_class_install_property (object_class,
                                         PROP_WINDOW,
                                         g_param_spec_object ("window",
                                                              "Window",
                                                              "The navigation window",
                                                              G_TYPE_OBJECT,
                                                              G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof(NautilusLocationBarActionPrivate));
}

static void
nautilus_location_bar_action_init (NautilusLocationBarAction *action)
{
        action->priv = NAUTILUS_LOCATION_BAR_ACTION_GET_PRIVATE (action);
}
