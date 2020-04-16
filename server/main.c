#include <stdlib.h>
#include <stdio.h>
#include <wayland-server.h>

struct wl_server
{
  struct wl_display *wl_display;
  struct wl_registry *registry;
};

void create_registry_resource(
  struct wl_client *client,
  void *data,
  uint32_t version,
  uint32_t id
  )
{
  wl_resource_create(client, &wl_registry_interface, version, id);
}

int main(int argc, char const *argv[])
{
  struct wl_server *server = calloc(1, sizeof *server);

  server->wl_display = wl_display_create();

  if (!server->wl_display)
  {
    fprintf(stderr, "Could not create a Wayland display\n");
    return 1;
  }

  const char *socket = wl_display_add_socket_auto(server->wl_display);
  if (!socket)
  {
    fprintf(stderr, "Could not create a Wayland display\n");
    return 1;
  }

  // Adds the global registry to the display
  wl_global_create(
      server->wl_display,
      &wl_registry_interface,
      wl_registry_interface.version,
      &server->registry,
      create_registry_resource);

  printf("Running Wayland display on %s\n", socket);

  wl_display_run(server->wl_display);
  wl_display_destroy(server->wl_display);

  return 0;
}
