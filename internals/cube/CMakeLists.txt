add_conventional_executable(cube)
target_link_libraries(cube PRIVATE gl_v1 emscripten_system_include)
target_link_options(cube PUBLIC
  "SHELL:-s USE_WEBGL2=1"
  "SHELL:-s MIN_WEBGL_VERSION=2"
  "SHELL:-s GL_EMULATE_GLES_VERSION_STRING_FORMAT=0"
  "SHELL:-s WASM=1")
add_custom_command(
  TARGET cube POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:cube>/cube.* ${CMAKE_SOURCE_DIR}/docs/)
