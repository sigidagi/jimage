#
#
set(CLIENT_SOURCE_CXX 
    main_client.cpp
    ImageInfo.cpp

) 

include_directories(${Boost_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
link_directories(${JIMAGE_BINARY_DIR}/json ) 

add_executable(asio_client ${CLIENT_SOURCE_CXX})
# On linux CMake figures itself that boost_filesystem is linked against boost_system. Obviously you have to tell it
# explicitly on Mac
target_link_libraries(asio_client ${Boost_LIBRARIES} json_spirit)
