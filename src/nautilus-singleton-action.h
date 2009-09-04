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
 *
 *  Based on nautilus-navigation-action.h
 *
 *  Authors: Alexander Larsson <alexl@redhat.com>
 *           Marco Pesenti Gritti
 *           Marcus Carlson <marcus@mejlamej.nu>
 *
 */

#ifndef NAUTILUS_SINGLETON_ACTION_H
#define NAUTILUS_SINGLETON_ACTION_H

#include <gtk/gtk.h>

#define NAUTILUS_TYPE_SINGLETON_ACTION            (nautilus_singleton_action_get_type ())
#define NAUTILUS_SINGLETON_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NAUTILUS_TYPE_SINGLETON_ACTION, NautilusSingletonAction))
#define NAUTILUS_SINGLETON_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NAUTILUS_TYPE_SINGLETON_ACTION, NautilusSingletonActionClass))
#define NAUTILUS_IS_SINGLETON_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NAUTILUS_TYPE_SINGLETON_ACTION))
#define NAUTILUS_IS_SINGLETON_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), NAUTILUS_TYPE_SINGLETON_ACTION))
#define NAUTILUS_SINGLETON_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), NAUTILUS_TYPE_SINGLETON_ACTION, NautilusSingletonActionClass))

typedef struct _NautilusSingletonAction       NautilusSingletonAction;
typedef struct _NautilusSingletonActionClass  NautilusSingletonActionClass;
typedef struct NautilusSingletonActionPrivate NautilusSingletonActionPrivate;

struct _NautilusSingletonAction
{
	GtkAction parent;
	
	/*< private >*/
	NautilusSingletonActionPrivate *priv;
};

struct _NautilusSingletonActionClass
{
	GtkActionClass parent_class;
};

GType    nautilus_singleton_action_get_type   (void);

GtkAction *
nautilus_singleton_action_new (const char *name,
                               const char *label,
                               const char *stock_id,
                               gboolean is_important,
                               gboolean visible_overflown,
                               gboolean expand,
                               GtkWidget *widget);

#endif
