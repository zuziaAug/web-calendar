# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/client_gui_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/client_gui_autogen.dir/ParseCache.txt"
  "CMakeFiles/server_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/server_autogen.dir/ParseCache.txt"
  "client_gui_autogen"
  "server_autogen"
  )
endif()
