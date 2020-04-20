struct client_state
{
	int foo;
	struct wl_compositor *compositor;
	struct wl_shm *shm;
	struct wl_surface *wl_surface;
	struct wl_buffer *wl_buffer;
	struct xdg_wm_base *xdg_wm_base;
};
