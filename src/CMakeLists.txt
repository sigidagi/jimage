#
#
file(GLOB FOTODK_SOURCE_CXX test.cpp ImageInfo.cpp) 

include_directories(${Boost_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
link_directories(${FOTODK_BINARY_DIR}/json ) 

add_executable(jimage ${FOTODK_SOURCE_CXX})
# On linux CMake figures itself that boost_filesystem is linked against boost_system. Obviously you have to tell it
# explicitly on Mac
target_link_libraries(jimage ${Boost_LIBRARIES} json_spirit)
