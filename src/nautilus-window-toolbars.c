/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Nautilus
 *
 * Copyright (C) 2000, 2001 Eazel, Inc.
 *
 * Nautilus is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Nautilus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Author: John Sullivan <sullivan@eazel.com> 
 */

/* nautilus-window-toolbars.c - implementation of nautilus window toolbar operations,
 * split into separate file just for convenience.
 */

#include <config.h>

#include <unistd.h>
#include "nautilus-application.h"
#include "nautilus-window-manage-views.h"
#include "nautilus-window-private.h"
#include "nautilus-window.h"
#include "nautilus-throbber.h"
#include "nautilus-pathbar.h"
#include "nautilus-zoom-action.h"
#include "nautilus-view-chooser-action.h"
#include "nautilus-location-bar-action.h"
#include <cut-n-paste-code/toolbar-editor/egg-editable-toolbar.h>
#include <nautilus-toolbars-model.h>
#include <eel/eel-gnome-extensions.h>
#include <eel/eel-gtk-extensions.h>
#include <eel/eel-string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gi18n.h>
#include <libnautilus-extension/nautilus-menu-provider.h>
#include <libnautilus-private/nautilus-bookmark.h>
#include <libnautilus-private/nautilus-file-utilities.h>
#include <libnautilus-private/nautilus-ui-utilities.h>
#include <libnautilus-private/nautilus-global-preferences.h>
#include <libnautilus-private/nautilus-module.h>

/* FIXME bugzilla.gnome.org 41243: 
 * We should use inheritance instead of these special cases
 * for the desktop window.
 */
#include "nautilus-desktop-window.h"

#define TOOLBAR_PATH_EXTENSION_ACTIONS "/Toolbar/Extra Buttons Placeholder/Extension Actions"

void
nautilus_navigation_window_set_throbber_active (NautilusNavigationWindow *window, 
						gboolean allow)
{
	if (( window->details->throbber_active &&  allow) ||
	    (!window->details->throbber_active && !allow)) {
		return;
	}

	window->details->throbber_active = allow;
	if (allow) {
		nautilus_throbber_start (NAUTILUS_THROBBER (window->details->throbber));
	} else {
		nautilus_throbber_stop (NAUTILUS_THROBBER (window->details->throbber));
	}
}

static void
toolbar_reconfigured_cb (GtkToolItem *item,
			 NautilusThrobber *throbber)
{
	GtkToolbarStyle style;
	GtkIconSize size;

	style = gtk_tool_item_get_toolbar_style (item);

	if (style == GTK_TOOLBAR_BOTH)
	{
		size = GTK_ICON_SIZE_DIALOG;
	}
	else
	{
		size = GTK_ICON_SIZE_LARGE_TOOLBAR;
	}

	nautilus_throbber_set_size (throbber, size);
}

static void
nautilus_navigation_window_activate_throbber (NautilusNavigationWindow *window)
{
 	GtkToolbar *fixed_toolbar;
	GtkToolItem *item;
	GtkWidget *throbber;

	if (window->details->throbber != NULL) {
		return;
	}

 	fixed_toolbar = GTK_TOOLBAR (gtk_toolbar_new ());

	/* Add expander */
	item = gtk_tool_item_new ();
	gtk_widget_show (GTK_WIDGET (item));
	gtk_tool_item_set_expand (item, TRUE);
 	gtk_toolbar_insert (fixed_toolbar, item, -1);

	/* Create throbber */
	throbber = nautilus_throbber_new ();
	gtk_widget_show (GTK_WIDGET (throbber));

	item = gtk_tool_item_new ();
	gtk_container_add (GTK_CONTAINER (item), throbber);
	gtk_widget_show (GTK_WIDGET (item));
	
	g_signal_connect (item, "toolbar-reconfigured",
			  G_CALLBACK (toolbar_reconfigured_cb), throbber);

 	gtk_toolbar_insert (fixed_toolbar, item, -1);

 	egg_editable_toolbar_set_fixed (EGG_EDITABLE_TOOLBAR (window->details->toolbar),
 					fixed_toolbar);

	window->details->throbber = throbber;
}

/* XXXMARCUS FIXME: Actually this function only adds the custom action to the action group so it might 
be good to change the name */
static void
nautilus_navigation_window_initialize_navigation_bar (NautilusNavigationWindow *window)
{
	GtkToolItem *item;
	GtkWidget *hbox, *view_as_menu_vbox;
	GtkAction *action;

	/* Location action */
	action = g_object_new (NAUTILUS_TYPE_LOCATION_BAR_ACTION,
			       "name", "Location",
			       "label", _("_Location"),
			       "stock_id", GTK_STOCK_EDIT,
			       "tooltip", _("Location"), //XXXMARCUS FIXME
			       "window", window,
			       "visible-overflown", FALSE,
			       "is_important", TRUE, //XXXMARCUS ??
			       NULL);

	gtk_action_group_add_action (window->details->navigation_action_group, 
				     action);

	g_object_unref (action); ///XXMARCUS Should this be here?

	/* Zoom action */
	action = g_object_new (NAUTILUS_TYPE_ZOOM_ACTION,
			       "name", "Zoom",
			       "label", _("_Zoom"),
			       "stock_id", GTK_STOCK_ZOOM_IN,
			       "tooltip", _("Zoooom"), //XXXMARCUS FIXME
			       "window", window,
			       "is_important", FALSE, //XXXMARCUS ??
			       NULL);

	gtk_action_group_add_action (
		window->details->navigation_action_group, action);

	g_object_unref (action);

	/* View Chooser action */
	action = g_object_new (NAUTILUS_TYPE_VIEW_CHOOSER_ACTION,
			       "name", "ViewChooser",
			       "label", _("_View Chooser"),
			       "stock_id", GTK_STOCK_FIND,
			       "tooltip", _("Viewchooser"), //XXXMARCUS FIXME
			       "window", window,
			       "is_important", FALSE, //XXXMARCUS ??
			       NULL);

	gtk_action_group_add_action (
		window->details->navigation_action_group, action);
}

void
nautilus_navigation_window_initialize_toolbars (NautilusNavigationWindow *window)
{
	const char *ui;

 	window->details->toolbars_model = nautilus_toolbars_model_new ();
 
	/* Creates the toolbar and specifiying the popup menu /editor from file editor.xml */ 
	window->details->toolbar = egg_editable_toolbar_new_with_model (
 		nautilus_window_get_ui_manager (NAUTILUS_WINDOW (window)),
 		window->details->toolbars_model, "/editor");

	ui = nautilus_ui_string_get ("editor.xml");
	gtk_ui_manager_add_ui_from_string (nautilus_window_get_ui_manager (NAUTILUS_WINDOW (window)), ui, -1, NULL);

	nautilus_navigation_window_activate_throbber (window);
	nautilus_navigation_window_initialize_navigation_bar (window);

 	nautilus_toolbars_model_load (NAUTILUS_TOOLBARS_MODEL (window->details->toolbars_model));
}

static GList *
get_extension_toolbar_items (NautilusNavigationWindow *window)
{
	NautilusWindowSlot *slot;
	GList *items;
	GList *providers;
	GList *l;
	
	providers = nautilus_module_get_extensions_for_type (NAUTILUS_TYPE_MENU_PROVIDER);
	items = NULL;

	slot = NAUTILUS_WINDOW (window)->details->active_slot;

	for (l = providers; l != NULL; l = l->next) {
		NautilusMenuProvider *provider;
		GList *file_items;
		
		provider = NAUTILUS_MENU_PROVIDER (l->data);
		file_items = nautilus_menu_provider_get_toolbar_items 
			(provider, 
			 GTK_WIDGET (window),
			 slot->viewed_file);
		items = g_list_concat (items, file_items);		
	}

	nautilus_module_extension_list_free (providers);

	return items;
}

void
nautilus_navigation_window_load_extension_toolbar_items (NautilusNavigationWindow *window)
{
	GtkActionGroup *action_group;
	GtkAction *action;
	GtkUIManager *ui_manager;
	GList *items;
	GList *l;
	NautilusMenuItem *item;
	guint merge_id;

	ui_manager = nautilus_window_get_ui_manager (NAUTILUS_WINDOW (window));
	if (window->details->extensions_toolbar_merge_id != 0) {
		gtk_ui_manager_remove_ui (ui_manager,
					  window->details->extensions_toolbar_merge_id);
		window->details->extensions_toolbar_merge_id = 0;
	}

	if (window->details->extensions_toolbar_action_group != NULL) {
		gtk_ui_manager_remove_action_group (ui_manager,
						    window->details->extensions_toolbar_action_group);
		window->details->extensions_toolbar_action_group = NULL;
	}
	
	merge_id = gtk_ui_manager_new_merge_id (ui_manager);
	window->details->extensions_toolbar_merge_id = merge_id;
	action_group = gtk_action_group_new ("ExtensionsToolbarGroup");
	window->details->extensions_toolbar_action_group = action_group;
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_ui_manager_insert_action_group (ui_manager, action_group, -1);
	g_object_unref (action_group); /* owned by ui manager */

	items = get_extension_toolbar_items (window);

	for (l = items; l != NULL; l = l->next) {
		item = NAUTILUS_MENU_ITEM (l->data);

		action = nautilus_toolbar_action_from_menu_item (item);

		gtk_action_group_add_action (action_group,
					     GTK_ACTION (action));
		g_object_unref (action);
		
		gtk_ui_manager_add_ui (ui_manager,
				       merge_id,
				       TOOLBAR_PATH_EXTENSION_ACTIONS,
				       gtk_action_get_name (action),
				       gtk_action_get_name (action),
				       GTK_UI_MANAGER_TOOLITEM,
				       FALSE);

		g_object_unref (item);
	}

	g_list_free (items);
}
