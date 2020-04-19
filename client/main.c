#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include "shm.h"

struct client_state
{
	int foo;
	struct wl_compositor *compositor;
	struct wl_shm *shm;
	struct wl_surface *wl_surface;
	struct wl_buffer *wl_buffer;
};

static void
registry_handle_global(
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

	printf("The client_data.foo: %d\n", state->foo);
	state->foo += 1;
	printf("interface: '%s', version: %d, name: %d\n",
				 interface, version, name);
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry,
															uint32_t name)
{
	// This space deliberately left blank
}

static const struct wl_registry_listener
		registry_listener = {
				.global = registry_handle_global,
				.global_remove = registry_handle_global_remove,
};

void create_shm_pool(struct client_state *state, struct wl_shm *wl_shm) {
	const long int width = 1920;
	const long int height = 1080;
	const long int stride = width * 4;

	const long int shm_pool_size = height * width * stride;
	const int fd = allocate_shm_file(shm_pool_size);
	if (fd < 0) {
		printf("Invalid file descriptor\n");
		return NULL;
	}

	uint8_t *pool_data = mmap(
			NULL, shm_pool_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


	struct wl_shm_pool *pool = wl_shm_create_pool(wl_shm, fd, shm_pool_size);
	int index = 0;
	int offset = height * stride * index;
	struct wl_buffer *buffer = wl_shm_pool_create_buffer(
			pool, offset, width, height, stride, WL_SHM_FORMAT_ARGB8888);

	uint32_t *pixels = (uint32_t *)&pool_data[offset];
	memset(pixels, 0, stride * height);

	state->wl_buffer = buffer;
	printf("Buffer formatted\n");
}

void format_buffer(
		void *data, struct wl_shm *wl_shm, uint32_t format)
{
	printf("Format event recieved\n");
}
static const struct wl_shm_listener
		shm_listener = {
				.format = format_buffer,
};

int main(int argc, char *argv[])
{
	struct client_state data;
	data.foo = 10;

	struct wl_display *display = wl_display_connect(NULL);
	struct wl_registry *registry = wl_display_get_registry(display);

	wl_registry_add_listener(registry, &registry_listener, &data);
	wl_display_roundtrip(display);
	wl_shm_add_listener(data.shm, &shm_listener, &data);
	assert(data.shm);
	create_shm_pool(&data, data.shm);
	while(1) {

	}
	wl_display_disconnect(display);

	return 0;
}