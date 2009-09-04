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

//#include "nautilus-navigation-window.h"
//#include "nautilus-window-private.h"
//#include "nautilus-navigation-window-slot.h"
#include "nautilus-singleton-action.h"
#include <gtk/gtk.h>
#include <eel/eel-gtk-extensions.h>

static void nautilus_singleton_action_init       (NautilusSingletonAction *action);
static void nautilus_singleton_action_class_init (NautilusSingletonActionClass *class);

static GObjectClass *parent_class = NULL;

#define NAUTILUS_SINGLETON_ACTION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), NAUTILUS_TYPE_SINGLETON_ACTION, NautilusSingletonActionPrivate))

struct NautilusSingletonActionPrivate
{
        GtkWidget *widget;
        gboolean expand;
};

enum
{
	PROP_0,
	PROP_WIDGET,
        PROP_EXPAND
};

GType
nautilus_singleton_action_get_type (void)
{
	static GType type = 0;

	if (type == 0) {
		const GTypeInfo type_info = {
			sizeof (NautilusSingletonActionClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) nautilus_singleton_action_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,
			sizeof (NautilusSingletonAction),
			0, /* n_preallocs */
			(GInstanceInitFunc) nautilus_singleton_action_init,
		};
		
		type = g_type_register_static (GTK_TYPE_ACTION,
					       "NautilusSingletonAction",
					       &type_info, 0);
	}

	return type;
}


static GtkWidget *
create_tool_item (GtkAction *action)
{
        GtkToolItem *proxy;

        proxy = gtk_tool_item_new ();
        gtk_tool_item_set_expand (proxy, NAUTILUS_SINGLETON_ACTION (action)->priv->expand);
	gtk_widget_show (GTK_WIDGET (proxy));

        return GTK_WIDGET (proxy);
}

static void
connect_proxy (GtkAction *action, GtkWidget *proxy)
{
	if (GTK_IS_TOOL_ITEM (proxy)) {
		NautilusSingletonAction *naction = NAUTILUS_SINGLETON_ACTION (action);
                if (naction->priv->widget->parent)
                        gtk_container_remove (GTK_CONTAINER (naction->priv->widget->parent), naction->priv->widget);

                gtk_container_add (GTK_CONTAINER (proxy), naction->priv->widget);
	}

	(* GTK_ACTION_CLASS (parent_class)->connect_proxy) (action, proxy);
}

static void
disconnect_proxy (GtkAction *action, GtkWidget *proxy)
{
	if (GTK_IS_TOOL_ITEM (proxy)) {
		NautilusSingletonAction *naction = NAUTILUS_SINGLETON_ACTION (action);

                if (naction->priv->widget && 
                    naction->priv->widget->parent && 
                    naction->priv->widget->parent == proxy)
                        gtk_container_remove (GTK_CONTAINER (proxy), naction->priv->widget);
	}

	(* GTK_ACTION_CLASS (parent_class)->disconnect_proxy) (action, proxy);
}

static void
nautilus_singleton_action_finalize (GObject *object)
{
	(* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static void
nautilus_singleton_action_set_property (GObject *object,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *pspec)
{
	NautilusSingletonAction *nav;

	nav = NAUTILUS_SINGLETON_ACTION (object);

	switch (prop_id)
	{
		case PROP_WIDGET:
			nav->priv->widget = GTK_WIDGET (g_value_get_object (value));
			break;
		case PROP_EXPAND:
                        nav->priv->expand = g_value_get_boolean (value);
			break;
	}
}

static void
nautilus_singleton_action_get_property (GObject *object,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *pspec)
{
	NautilusSingletonAction *nav;

	nav = NAUTILUS_SINGLETON_ACTION (object);

	switch (prop_id)
	{
		case PROP_WIDGET:
			g_value_set_object (value, nav->priv->widget);
			break;
                case PROP_EXPAND:
                        g_value_set_boolean (value, nav->priv->expand);
                        break;
	}
}

GtkAction *
nautilus_singleton_action_new (const char *name,
                               const char *label,
                               const char *stock_id,
                               gboolean is_important,
                               gboolean visible_overflown,
                               gboolean expand,
                               GtkWidget *widget)
{
        GtkAction *action;

	action = g_object_new (NAUTILUS_TYPE_SINGLETON_ACTION,
			       "name", name,
			       "label", label,
			       "stock_id", stock_id,
			       "widget", widget,
			       "visible-overflown", visible_overflown,
			       "is_important", is_important,
			       "expand", expand,
			       NULL);

        return GTK_ACTION (action);
}


static void
nautilus_singleton_action_class_init (NautilusSingletonActionClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GtkActionClass *action_class = GTK_ACTION_CLASS (class);

	object_class->finalize = nautilus_singleton_action_finalize;
	object_class->set_property = nautilus_singleton_action_set_property;
	object_class->get_property = nautilus_singleton_action_get_property;

	parent_class = g_type_class_peek_parent (class);

	action_class->toolbar_item_type = GTK_TYPE_MENU_TOOL_BUTTON;
	action_class->connect_proxy = connect_proxy;
	action_class->disconnect_proxy = disconnect_proxy;
	action_class->create_tool_item = create_tool_item;

	g_object_class_install_property (object_class,
                                         PROP_WIDGET,
                                         g_param_spec_object ("widget",
                                                              "Widget",
                                                              "The widget to get wrapped",
                                                              G_TYPE_OBJECT,
                                                              G_PARAM_READWRITE));

        g_object_class_install_property (object_class,
                                         PROP_EXPAND,
                                         g_param_spec_boolean ("expand",
                                                               NULL,
                                                               NULL,
                                                               FALSE,
                                                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_type_class_add_private (object_class, sizeof(NautilusSingletonActionPrivate));
}

static void
nautilus_singleton_action_init (NautilusSingletonAction *action)
{
        action->priv = NAUTILUS_SINGLETON_ACTION_GET_PRIVATE (action);
}
