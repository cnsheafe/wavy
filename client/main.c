#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include "lib/shm.h"

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

	/* An example of how client state can be updated */
	// printf("The client_data.foo: %d\n", state->foo);
	// state->foo += 1;
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

int create_shm_pool(struct client_state *state)
{
	const long int width = 1920;
	const long int height = 1080;
	const long int stride = width * 4;

	const long int shm_pool_size = height * width * stride;
	const int fd = allocate_shm_file(shm_pool_size);
	if (fd < 0)
	{
		printf("Invalid file descriptor\n");
		return -1;
	}

	uint8_t *pool_data = mmap(
			NULL, shm_pool_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	struct wl_shm *wl_shm = state->shm;
	struct wl_shm_pool *pool = wl_shm_create_pool(wl_shm, fd, shm_pool_size);

	int index = 0;
	int offset = height * stride * index;
	struct wl_buffer *buffer = wl_shm_pool_create_buffer(
			pool, offset, width, height, stride, WL_SHM_FORMAT_ARGB8888);

	uint32_t *pixels = (uint32_t *)&pool_data[offset];
	memset(pixels, 0, stride * height);

	state->wl_buffer = buffer;
	printf("Buffer formatted\n");
	return 1;
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
	struct client_state state;
	state.foo = 10;

	struct wl_display *display = wl_display_connect(NULL);
	struct wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_listener, &state);

	// Will prompt server to send back event for registry.global
	wl_display_roundtrip(display);

	// Setup shm
	wl_shm_add_listener(state.shm, &shm_listener, &state);
	assert(state.shm);
	int status = create_shm_pool(&state);
	if (status < 0)
	{
		printf("Failed to create shm_pool\n");
		return 1;
	}

	struct wl_surface *surface = wl_compositor_create_surface(state.compositor);
	wl_surface_attach(surface, state.wl_buffer, 0, 0);
	wl_surface_damage(surface, 0 , 0, UINT32_MAX, UINT32_MAX);
	wl_surface_commit(surface);


	while (1)
	{
	}
	wl_display_disconnect(display);

	return 0;
}