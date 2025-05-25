# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\file_tag_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\file_tag_autogen.dir\\ParseCache.txt"
  "file_tag_autogen"
  )
endif()
