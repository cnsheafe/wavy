#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include "../lib/shm.h"
#include "../lib/xdg-shell-client-protocol.h"
// #include "state.h"
#include "registry.h"
#include "xdg.h"

int main(int argc, char *argv[])
{
	struct client_state state;
	state.foo = 10;

	struct wl_display *display = wl_display_connect(NULL);
	struct wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_listener, &state);

	// Will prompt server to send back event for registry.global
	wl_display_roundtrip(display);

	struct wl_surface *wl_surface = wl_compositor_create_surface(state.compositor);
	state.wl_surface = wl_surface;

	struct xdg_surface *xdg_surface = xdg_wm_base_get_xdg_surface(
			state.xdg_wm_base, wl_surface);

	xdg_surface_add_listener(xdg_surface, &surface_listener, &state);

	struct xdg_toplevel *xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
	xdg_toplevel_set_title(xdg_toplevel, "Hello Wayland!");

	// triggers the rendering to start
	wl_surface_commit(state.wl_surface);

	while (wl_display_dispatch(display))
	{
	}
	while (1)
	{
	}
	wl_display_disconnect(display);

	return 0;
}