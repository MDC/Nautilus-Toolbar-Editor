/*
 *  Copyright (C) 2003, 2004 Marco Pesenti Gritti
 *  Copyright (C) 2003, 2004 Christian Persch
 *  Copyright (C) 2005 Christian Neumair
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  $Id: nautilus-toolbars-model.h,v 1.15 2005/01/08 16:40:45 chpe Exp $
 */

#ifndef NAUTILUS_TOOLBARS_MODEL_H
#define NAUTILUS_TOOLBARS_MODEL_H

#include <cut-n-paste-code/toolbar-editor/egg-toolbars-model.h>

G_BEGIN_DECLS

#define NAUTILUS_TYPE_TOOLBARS_MODEL	     (nautilus_toolbars_model_get_type ())
#define NAUTILUS_TOOLBARS_MODEL(o)	     (G_TYPE_CHECK_INSTANCE_CAST ((o), NAUTILUS_TYPE_TOOLBARS_MODEL, NautilusToolbarsModel))
#define NAUTILUS_TOOLBARS_MODEL_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), NAUTILUS_TYPE_TOOLBARS_MODEL, NautilusToolbarsModelClass))
#define NAUTILUS_IS_TOOLBARS_MODEL(o)	     (G_TYPE_CHECK_INSTANCE_TYPE ((o), NAUTILUS_TYPE_TOOLBARS_MODEL))
#define NAUTILUS_IS_TOOLBARS_MODEL_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), NAUTILUS_TYPE_TOOLBARS_MODEL))
#define NAUTILUS_TOOLBARS_MODEL_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), NAUTILUS_TYPE_TOOLBARS_MODEL, NautilusToolbarsModelClass))

typedef struct _NautilusToolbarsModelClass	NautilusToolbarsModelClass;
typedef struct _NautilusToolbarsModel		NautilusToolbarsModel;
typedef struct _NautilusToolbarsModelDetails	NautilusToolbarsModelDetails;

struct _NautilusToolbarsModel
{
	EggToolbarsModel parent_object;

	NautilusToolbarsModelDetails *details;
};

struct _NautilusToolbarsModelClass
{
	EggToolbarsModelClass parent_class;
};

GType		  nautilus_toolbars_model_get_type	(void);

EggToolbarsModel *nautilus_toolbars_model_new	(void);

void		  nautilus_toolbars_model_load	(NautilusToolbarsModel *model);

G_END_DECLS

#endif
