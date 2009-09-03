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

#ifndef NAUTILUS_VIEW_CHOOSER_ACTION_H
#define NAUTILUS_VIEW_CHOOSER_ACTION_H

#include <gtk/gtk.h>

#define NAUTILUS_TYPE_VIEW_CHOOSER_ACTION            (nautilus_view_chooser_action_get_type ())
#define NAUTILUS_VIEW_CHOOSER_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NAUTILUS_TYPE_VIEW_CHOOSER_ACTION, NautilusViewChooserAction))
#define NAUTILUS_VIEW_CHOOSER_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NAUTILUS_TYPE_VIEW_CHOOSER_ACTION, NautilusViewChooserActionClass))
#define NAUTILUS_IS_VIEW_CHOOSER_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NAUTILUS_TYPE_VIEW_CHOOSER_ACTION))
#define NAUTILUS_IS_VIEW_CHOOSER_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), NAUTILUS_TYPE_VIEW_CHOOSER_ACTION))
#define NAUTILUS_VIEW_CHOOSER_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), NAUTILUS_TYPE_VIEW_CHOOSER_ACTION, NautilusViewChooserActionClass))

typedef struct _NautilusViewChooserAction       NautilusViewChooserAction;
typedef struct _NautilusViewChooserActionClass  NautilusViewChooserActionClass;
typedef struct NautilusViewChooserActionPrivate NautilusViewChooserActionPrivate;

struct _NautilusViewChooserAction
{
	GtkAction parent;
	
	/*< private >*/
	NautilusViewChooserActionPrivate *priv;
};

struct _NautilusViewChooserActionClass
{
	GtkActionClass parent_class;

	/* Signals */
};

GType    nautilus_view_chooser_action_get_type   (void);

#endif
