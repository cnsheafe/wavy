#! /bin/sh

ninja -C build/
WAYLAND_DISPLAY=wayland-1 ./build/wl