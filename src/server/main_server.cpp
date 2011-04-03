#include "server.h"
#include "../message.h"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <iostream>


int main(int argc, char** argv ) {

	std::cout << "Synchronous TCP asio SERVER" << std::endl;

	boost::asio::io_service io_service;
	server s( io_service );

	s.start( argc, argv );

	return 0;
}
