#include <stdint.h>
#include <wayland-client.h>
#include "../lib/xdg-shell-client-protocol.h"
#include "state.h"
#include "xdg.h"
#include "stdio.h"
#include "string.h"

void registry_handle_global(
    void *data, struct wl_registry *registry,
    uint32_t name, const char *interface, uint32_t version)
{
  struct client_state *state = data;

  // Bind compositor
  if (strcmp(interface, wl_compositor_interface.name) == 0)
  {
    state->compositor = wl_registry_bind(
        registry, name, &wl_compositor_interface, version);
  }
  // Bind shared-memory
  else if (strcmp(interface, wl_shm_interface.name) == 0)
  {
    state->shm = wl_registry_bind(
        registry, name, &wl_shm_interface, version);
  }
  else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
  {

    state->xdg_wm_base = wl_registry_bind(
        registry, name, &xdg_wm_base_interface, version);

    xdg_wm_base_add_listener(state->xdg_wm_base, &wm_base_listener, &state);
    // xdg_wm_base_add_listener(state->xdg_wm_base, &wm_base_listener, &state);
  }

  /* An example of how client state can be updated */
  // printf("The client_data.foo: %d\n", state->foo);
  // state->foo += 1;
  // printf("interface: '%s', version: %d, name: %d\n",
  //        interface, version, name);
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry,
															uint32_t name)
{
	// This space deliberately left blank
}

const struct wl_registry_listener
		registry_listener = {
				.global = registry_handle_global,
				.global_remove = registry_handle_global_remove,
};
