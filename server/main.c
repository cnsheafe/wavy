#include <stdlib.h>
#include <stdio.h>
#include <wayland-server.h>

struct wl_server
{
  struct wl_display *wl_display;
  struct wl_registry *registry;
  struct wl_compositor *compositor;
  struct wl_output *wl_output;
  struct wl_shm *wl_shm;
};

void create_registry_resource(
    struct wl_client *client,
    void *data,
    uint32_t version,
    uint32_t id)
{
  wl_resource_create(client, &wl_registry_interface, version, id);
}

void create_compositor_resource(
    struct wl_client *client,
    void *data,
    uint32_t version,
    uint32_t id)
{
  wl_resource_create(client, &wl_compositor_interface, version, id);
}

void create_shm_resource(
  struct wl_client *client,
  void *data,
  uint32_t version,
  uint32_t id
) {
  wl_resource_create(client, &wl_shm_interface, version, id);
}

void destroy_output_resource(struct wl_resource *resource)
{
  // get user data
  int *dummy_data = wl_resource_get_user_data(resource);
  printf("Dummy data is %d\n", *dummy_data);

  *dummy_data = 0;
  printf("Dummy data being changed to %d\n", *dummy_data);
}

void release_output(struct wl_client *client, struct wl_resource *resource)
{
  wl_resource_destroy(resource);
}

const struct wl_output_interface wl_output_implementation = {
    .release = release_output,
};

void create_output_resource(
    struct wl_client *client,
    void *data,
    uint32_t version,
    uint32_t id)
{
  // Is seen by client via destroy_listener
  int dummy_data = 1;

  struct wl_resource *resource = wl_resource_create(
      client, &wl_output_interface, version, id);

  wl_resource_set_implementation(
      resource, &wl_output_interface, &dummy_data, destroy_output_resource);
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

  // Adds the global compositor to the display
  wl_global_create(
      server->wl_display,
      &wl_compositor_interface,
      wl_compositor_interface.version,
      &server->compositor,
      create_compositor_resource);

  //  Adds the global output to the display
  wl_global_create(
      server->wl_display,
      &wl_output_interface,
      wl_output_interface.version,
      &server->wl_output,
      create_output_resource);


  // Adds the global shared-memory to the display
  wl_global_create(
    server->wl_display,
    &wl_shm_interface,
    wl_shm_interface.version,
    &server->wl_shm,
    create_shm_resource);

  printf("Running Wayland display on %s\n", socket);

  wl_display_run(server->wl_display);
  wl_display_destroy(server->wl_display);

  return 0;
}
