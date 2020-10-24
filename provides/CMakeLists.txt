if(NOT EMSCRIPTEN)
  message(WARNING "`gl_v1` library currently requires Emscripten.")
  return()
endif()

add_conventional_library(gl_v1)
target_link_libraries(gl_v1 PUBLIC data_v1 math3d_v1 emscripten_system_include)
