if(NOT EMSCRIPTEN)
  message(WARNING "`gl_v1` library currently requires Emscripten.")
  return()
endif()

add_conventional_executable_tests(PRIVATE gl_v1)
