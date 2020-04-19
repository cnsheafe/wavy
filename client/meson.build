project('wavy', 'c')

cc = meson.get_compiler('c')

# Clang complains about some zeroed initializer lists (= {0}), even though they
# are valid
if cc.get_id() == 'clang'
  add_project_arguments('-Wno-missing-field-initializers', language: 'c')
  add_project_arguments('-Wno-missing-braces', language: 'c')
endif

# Avoid wl_buffer deprecation warnings
add_project_arguments('-DWL_HIDE_DEPRECATED', language: 'c')

pixman          = dependency('pixman-1')
wayland_client  = dependency('wayland-client')

executable('wl', ['main.c', 'lib/shm.c'],
dependencies: [
  pixman,
  wayland_client,
],
link_args: '-lrt')
