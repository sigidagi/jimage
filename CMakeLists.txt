# (C) 2010 SIGIDAGI 
#
cmake_minimum_required(VERSION 2.6)

set(JIMAGE_MAJOR 0)
set(JIMAGE_MINOR 1)
set(JIMAGE_PATCH 10)

message(STATUS "Configuring jimage ${JIMAGE_MAJOR}.${JIMAGE_MINOR}.${JIMAGE_PATCH}")
message(STATUS "Configuring jimage cmake_source_dir  = ${CMAKE_SOURCE_DIR}")

project(JIMAGE CXX)

set(CMAKE_CXX_FLAGS "-g -Wall")

#set(Boost_USE_STATIC_LIBS ON)
# when components are found these variables ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSYTEM_LIBRARY}
# and ${Boost_LIBRARIES} are filed with values. If components ate not defined - variables are EMPTY!
find_package(Boost 1.34 COMPONENTS system filesystem thread REQUIRED)
message(STATUS "boost_libraries " ${Boost_LIBRARIES})

#message(STATUS "boost_filesysytem_library " ${Boost_FILESYSTEM_LIBRARY})
#message(STATUS "boost_sysytem_library " ${Boost_SYSTEM_LIBRARY})

find_program(EXIFTOOL_PROGRAM
  NAMES exiftool 
)

if(EXIFTOOL_PROGRAM)
    message(STATUS "Found " ${EXIFTOOL_PROGRAM})
else(EXIFTOOL_PROGRAM)
    message(FATAL_ERROR "Can NOT find Exiftool. program needs for parsing header of images. Install
    'libimage-exiftool-perl".)
endif()

add_subdirectory(json)
add_subdirectory(src/server)
add_subdirectory(src/client)
