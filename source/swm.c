#include <stdlib.h>

#include <xcb/xcb.h>

static struct xcb_screen_t* xcb_screen       = NULL;
static xcb_window_t         xcb_focus_window = 0;

enum focus_mode
{
	FOCUS_MODE_ACTIVE,
	FOCUS_MODE_INACTIVE
};

static struct xcb_connection_t* swm_init(void)
{
	struct xcb_connection_t* xcb_connection = xcb_connect(NULL, NULL);
	if (xcb_connection_has_error(xcb_connection))
	{
		return NULL;
	}

	xcb_screen = xcb_setup_roots_iterator(xcb_get_setup(xcb_connection)).data;
	xcb_focus_window = xcb_screen->root;

	int      mask      = XCB_CW_EVENT_MASK;
	uint32_t values[1] = { XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY };
	xcb_change_window_attributes_checked(xcb_connection,
	                                     xcb_screen->root,
	                                     mask,
	                                     values);
	xcb_flush(xcb_connection);

	return xcb_connection;
}

static void swm_shutdown(struct xcb_connection_t* connection)
{
	xcb_disconnect(connection);
}

static void swm_focus_window(struct xcb_connection_t* xcb_connection,
                             xcb_window_t             window,
                             enum focus_mode          mode)
{
	uint32_t values[1] = { (mode == FOCUS_MODE_ACTIVE) ? 0xffffff : 0xff0000 };
	xcb_change_window_attributes(xcb_connection,
	                             window,
	                             XCB_CW_BORDER_PIXMAP,
	                             values);

	if (mode == FOCUS_MODE_ACTIVE)
	{
		xcb_set_input_focus(xcb_connection,
		                    XCB_INPUT_FOCUS_POINTER_ROOT,
		                    window,
		                    XCB_CURRENT_TIME);

		if (window != xcb_focus_window)
		{
			xcb_focus_window = window;
		}
	}
}

static void swm_subscribe_window(struct xcb_connection_t* connection,
                                 xcb_window_t             window)
{
	uint32_t event_mask_values[2]
	    = { XCB_EVENT_MASK_ENTER_WINDOW, XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY };
	xcb_change_window_attributes(connection,
	                             window,
	                             XCB_CW_EVENT_MASK,
	                             event_mask_values);

	/* border width */
	uint32_t border_values[1] = { 44 };
	xcb_configure_window(connection,
	                     window,
	                     XCB_CONFIG_WINDOW_BORDER_WIDTH,
	                     border_values);
}

static void swm_handle_create_notify(struct xcb_connection_t* xcb_connection,
                                     struct xcb_create_notify_event_t* event)
{
	if (!event->override_redirect)
	{
		swm_subscribe_window(xcb_connection, event->window);
		swm_focus_window(xcb_connection, event->window, FOCUS_MODE_ACTIVE);
	}
}

static void swm_handle_destroy_notify(struct xcb_connection_t* xcb_connection,
                                      struct xcb_destroy_notify_event_t* event)
{
	xcb_kill_client(xcb_connection, event->window);
}

static void swm_handle_map_notify(struct xcb_connection_t*       xcb_connection,
                                  struct xcb_map_notify_event_t* event)
{
	if (!event->override_redirect)
	{
		xcb_map_window(xcb_connection, event->window);
		swm_focus_window(xcb_connection, event->window, FOCUS_MODE_ACTIVE);
	}
}

static void
swm_handle_configure_notify(struct xcb_connection_t*             xcb_connection,
                            struct xcb_configure_notify_event_t* event)
{
	enum focus_mode mode = event->window != xcb_focus_window
	                           ? FOCUS_MODE_INACTIVE
	                           : FOCUS_MODE_ACTIVE;
	swm_focus_window(xcb_connection, event->window, mode);
}

static void swm_handle_events(struct xcb_connection_t* xcb_connection)
{
	xcb_generic_event_t* event = NULL;

	while (1)
	{
		event = xcb_wait_for_event(xcb_connection);

		switch (event->response_type & ~0x80)
		{
		case XCB_CREATE_NOTIFY:
			swm_handle_create_notify(xcb_connection,
			                         (struct xcb_create_notify_event_t*)event);
			break;
		case XCB_DESTROY_NOTIFY:
			swm_handle_destroy_notify(xcb_connection,
			                          (struct xcb_destroy_notify_event_t*)
			                              event);
			break;
		case XCB_MAP_NOTIFY:
			swm_handle_map_notify(xcb_connection,
			                      (struct xcb_map_notify_event_t*)event);
			break;
		case XCB_CONFIGURE_NOTIFY:
			swm_handle_configure_notify(xcb_connection,
			                            (struct xcb_configure_notify_event_t*)
			                                event);
			break;
		}

		xcb_flush(xcb_connection);
		free(event);
	}
}

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	struct xcb_connection_t* xcb_connection = swm_init();
	swm_handle_events(xcb_connection);
	swm_shutdown(xcb_connection);

	return 0;
}
