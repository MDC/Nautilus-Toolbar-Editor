/*
 *  Copyright (C) 2002-2004 Marco Pesenti Gritti
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
 *  $Id: nautilus-toolbars-model.c,v 1.36 2005/02/09 21:08:03 chpe Exp $
 */

#include "config.h"

#include "nautilus-toolbars-model.h"
#include <eel/eel-gconf-extensions.h>
#include <nautilus-file-utilities.h>
#include <cut-n-paste-code/toolbar-editor/eggtypebuiltins.h>
#include <cut-n-paste-code/toolbar-editor/egg-toolbars-model.h>
#include <string.h>

#define NAUTILUS_TOOLBARS_XML_FILE	"toolbars.xml"

#define NAUTILUS_TOOLBARS_MODEL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), NAUTILUS_TYPE_TOOLBARS_MODEL, NautilusToolbarsModelDetails))

struct _NautilusToolbarsModelDetails
{
	char *xml_file;
	EggTbModelFlags style;
	guint timeout;
	guint style_notifier_id;
};

static void nautilus_toolbars_model_class_init (NautilusToolbarsModelClass *klass);
static void nautilus_toolbars_model_init       (NautilusToolbarsModel *model);

static GObjectClass *parent_class = NULL;

GType
nautilus_toolbars_model_get_type (void)
{
	static GType type = 0;

	if (G_UNLIKELY (type == 0))
	{
		static const GTypeInfo our_info = {
		sizeof (NautilusToolbarsModelClass),
		NULL,			/* base_init */
		NULL,			/* base_finalize */
		(GClassInitFunc) nautilus_toolbars_model_class_init,
		NULL,
		NULL,			/* class_data */
		sizeof (NautilusToolbarsModel),
		0,			/* n_preallocs */
		(GInstanceInitFunc) nautilus_toolbars_model_init
	};

	type = g_type_register_static (EGG_TYPE_TOOLBARS_MODEL,
				       "NautilusToolbarsModel",
				       &our_info, 0);
	}

	return type;
}

static gboolean
save_changes_idle (NautilusToolbarsModel *model)
{
	egg_toolbars_model_save_toolbars
		(EGG_TOOLBARS_MODEL (model),
		 model->details->xml_file,
		 "1.0");

	model->details->timeout = 0;

	/* XXXMARCUS: FIXME: Loop thru application windows and upate model ? */

	/* don't run again */
	return FALSE;
}

static void
save_changes (NautilusToolbarsModel *model)
{
	if (model->details->timeout == 0) {
		model->details->timeout =
			g_idle_add ((GSourceFunc) save_changes_idle, model);
	}
}

static void
update_flags (NautilusToolbarsModel *model)
{
	EggToolbarsModel *eggmodel = EGG_TOOLBARS_MODEL (model);
	int i, n_toolbars;
	int flag = EGG_TB_MODEL_ACCEPT_ITEMS_ONLY | EGG_TB_MODEL_NOT_EDITABLE;

	n_toolbars = egg_toolbars_model_n_toolbars (eggmodel);

	if (n_toolbars <= 1)
		flag |= EGG_TB_MODEL_NOT_REMOVABLE;

	for (i = 0; i < n_toolbars; i++) {
		const char *t_name;
		EggTbModelFlags flags;

		t_name = egg_toolbars_model_toolbar_nth (eggmodel, i);
		g_return_if_fail (t_name != NULL);

		flags = egg_toolbars_model_get_flags (eggmodel, i);
		flags &= ~(EGG_TB_MODEL_NOT_REMOVABLE | EGG_TB_MODEL_STYLES_MASK);
		flags |= flag;
		flags |= model->details->style;
		egg_toolbars_model_set_flags (eggmodel, i, flags);
	}
}

static void
update_flags_and_save_changes (NautilusToolbarsModel *model)
{
	update_flags (model);
	save_changes (model);
}

void
nautilus_toolbars_model_set_style (NautilusToolbarsModel *model,
				   const char *style)
{
	eel_gconf_set_string (CONF_INTERFACE_TOOLBAR_STYLE, style);
}

void
nautilus_toolbars_model_load_default (NautilusToolbarsModel *model)
{
	EggToolbarsModel *eggmodel = EGG_TOOLBARS_MODEL (model);
	int n_toolbars, i;
	char *path;
	gboolean success;

	n_toolbars = egg_toolbars_model_n_toolbars (eggmodel);
	
	/* File with default toolbar and available items */
	path = g_build_filename (NAUTILUS_DATADIR, NAUTILUS_TOOLBARS_XML_FILE, NULL);

	/* Clear toolbar */
	for (i = 0; i < n_toolbars; i++) {
		egg_toolbars_model_set_flags (eggmodel, 0, ~EGG_TB_MODEL_NOT_REMOVABLE);
		egg_toolbars_model_remove_toolbar (eggmodel, 0);
	}
	success = egg_toolbars_model_load_toolbars (eggmodel, path);
	
	g_free (path);

	if (!success) {
		g_warning ("could not load toolbar model from \"%s/%s\"", NAUTILUS_DATADIR, NAUTILUS_TOOLBARS_XML_FILE);
	}
	/* Ensure we have at least 1 toolbar */
	if (egg_toolbars_model_n_toolbars (eggmodel) < 1)
		egg_toolbars_model_add_toolbar (eggmodel, 0, "DefaultToolbar");
}

void
nautilus_toolbars_model_load (NautilusToolbarsModel *model)
{
	EggToolbarsModel *eggmodel = EGG_TOOLBARS_MODEL (model);
	gboolean success;
	char *path;
	/* File with default toolbar and available items */
	path = g_build_filename (NAUTILUS_DATADIR, NAUTILUS_TOOLBARS_XML_FILE, NULL);

	/* Load available items */
	egg_toolbars_model_load_names (eggmodel, path);

	/* Load personal toolbar */
	success = egg_toolbars_model_load_toolbars (eggmodel, model->details->xml_file);
	if (!success) {
		/* Load default toolbar */
		success = egg_toolbars_model_load_toolbars (eggmodel, path);
	}

	g_free (path);

	if (!success) {
		g_warning ("could not load toolbar model from \"%s/%s\"", NAUTILUS_DATADIR, NAUTILUS_TOOLBARS_XML_FILE);
	}

	/* Ensure we have at least 1 toolbar */
	if (egg_toolbars_model_n_toolbars (eggmodel) < 1) {
		egg_toolbars_model_add_toolbar (eggmodel, 0, "DefaultToolbar");
	}
}

static EggTbModelFlags
get_toolbar_style (void)
{
	GFlagsClass *flags_class;
	const GFlagsValue *value;
	EggTbModelFlags flags = 0;
	char *pref;

	pref = eel_gconf_get_string (CONF_INTERFACE_TOOLBAR_STYLE);
	if (pref != NULL) {
		flags_class = g_type_class_ref (EGG_TYPE_TB_MODEL_FLAGS);
		value = g_flags_get_value_by_nick (flags_class, pref);
		if (value != NULL) {
			flags = value->value;
		}
		g_type_class_unref (flags_class);
	}
	flags &= EGG_TB_MODEL_STYLES_MASK;

	g_free (pref);

	return flags;
}

static void
toolbar_style_notifier (GConfClient *client,
			guint cnxn_id,
			GConfEntry *entry,
			NautilusToolbarsModel *model)
{
	model->details->style = get_toolbar_style ();

	update_flags (model);
}

static void
nautilus_toolbars_model_init (NautilusToolbarsModel *model)
{
	NautilusToolbarsModelDetails *details;
	char *user_directory;

	details = model->details = NAUTILUS_TOOLBARS_MODEL_GET_PRIVATE (model);

	user_directory = nautilus_get_user_directory ();
	details->xml_file = g_build_filename (user_directory,
					      NAUTILUS_TOOLBARS_XML_FILE,
					      NULL);
	g_free (user_directory);

	details->style = get_toolbar_style ();
	details->style_notifier_id = eel_gconf_notification_add
		(CONF_INTERFACE_TOOLBAR_STYLE,
		 (GConfClientNotifyFunc) toolbar_style_notifier, model);
	
	g_signal_connect_after (model, "item_added",
				G_CALLBACK (save_changes), NULL);
	g_signal_connect_after (model, "item_removed",
				G_CALLBACK (save_changes), NULL);
	g_signal_connect_after (model, "toolbar_added",
				G_CALLBACK (update_flags_and_save_changes), NULL);
	g_signal_connect_after (model, "toolbar_removed",
				G_CALLBACK (update_flags_and_save_changes), NULL);
}

static void
nautilus_toolbars_model_dispose (GObject *object)
{
	NautilusToolbarsModel *model = NAUTILUS_TOOLBARS_MODEL (object);

	save_changes_idle (model);

	G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
nautilus_toolbars_model_finalize (GObject *object)
{
	NautilusToolbarsModel *model = NAUTILUS_TOOLBARS_MODEL (object);
	NautilusToolbarsModelDetails *details = model->details;

	if (details->style_notifier_id != 0)
		eel_gconf_notification_remove (details->style_notifier_id);

	if (details->timeout != 0)
		g_source_remove (details->timeout);

	g_free (details->xml_file);

	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
nautilus_toolbars_model_class_init (NautilusToolbarsModelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	parent_class = g_type_class_peek_parent (klass);

	object_class->dispose = nautilus_toolbars_model_dispose;
	object_class->finalize = nautilus_toolbars_model_finalize;

	g_type_class_add_private (object_class, sizeof (NautilusToolbarsModelDetails));
}

EggToolbarsModel *
nautilus_toolbars_model_new (void)
{
	return EGG_TOOLBARS_MODEL (g_object_new (NAUTILUS_TYPE_TOOLBARS_MODEL, NULL));
}
