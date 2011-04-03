#
#e
project(ASIO_SERVER)

set(SERVER_SOURCE_CXX 
    main_server.cpp
) 

include_directories(${Boost_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
link_directories(${JIMAGE_BINARY_DIR}/json ) 

add_executable(asio_server ${SERVER_SOURCE_CXX})
# On linux CMake figures itself that boost_filesystem is linked against boost_system. Obviously you have to tell it
# explicitly on Mac
target_link_libraries(asio_server ${Boost_LIBRARIES} json_spirit)
