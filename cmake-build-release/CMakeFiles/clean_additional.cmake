# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\find_it_ssu_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\find_it_ssu_autogen.dir\\ParseCache.txt"
  "find_it_ssu_autogen"
  )
endif()
