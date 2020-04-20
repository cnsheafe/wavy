#include "../lib/xdg-shell-client-protocol.h"
#include "../lib/shm.h"
#include "xdg.h"
#include <stdio.h>
#include <string.h>

/**** XDG_WM_BASE ****/
void pong(void *data, struct xdg_wm_base *wm_base, uint32_t serial)
{
  xdg_wm_base_pong(wm_base, serial);
}

const struct xdg_wm_base_listener wm_base_listener = {
    .ping = pong,
};

/**** XDG_SURFACE ****/
void configure_xdg_surface(
    void *data, struct xdg_surface *surface, uint32_t serial)
{
  struct client_state *state = data;

  xdg_surface_ack_configure(surface, serial);
  draw_frame(state);

  wl_surface_attach(state->wl_surface, state->wl_buffer, 0, 0);
  wl_surface_commit(state->wl_surface);
};

const struct xdg_surface_listener surface_listener = {
    .configure = configure_xdg_surface,
};

int draw_frame(struct client_state *state)
{
  const int32_t width = 640;
  const int32_t height = 320;
  const int32_t stride = width * 4;

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
  // wl_shm_pool_destroy(pool);
  // close(fd);

  uint32_t *pixels = (uint32_t *)&pool_data[offset];
  memset(pixels, 0xFFFFFFFF, stride * height);

  state->wl_buffer = buffer;
  printf("Buffer formatted\n");
  wl_surface_attach(state->wl_surface, buffer, 0, 0);
  wl_surface_damage(state->wl_surface, 0, 0, UINT32_MAX, UINT32_MAX);
  wl_surface_commit(state->wl_surface);

  return 1;
}

// void format_buffer(
// 		void *data, struct wl_shm *wl_shm, uint32_t format)
// {
// 	printf("Format event recieved\n");
// }
// static const struct wl_shm_listener
// 		shm_listener = {
// 				.format = format_buffer,
// };
