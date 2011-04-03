#ifndef SERVER_H_
#define SERVER_H_

#include "../message.h"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include <iostream>

namespace {
	boost::condition host_condition__;
	boost::mutex io_mutex__;

	inline void dump( std::string const & s ) {
		boost::mutex::scoped_lock l( io_mutex__ );
		std::cout << " server# " << s;
	}
}

struct connectionThread {
	void operator()( boost::shared_ptr< boost::asio::ip::tcp::socket >& s ) {
		try{
			dump( "client has connected to the Host. Instanciated connection thread.\n" );
			dump( "sending ACK to client.\n" );

			message ms_s( message::MS_ACK);
			//boost::asio::write( *s, boost::asio::buffer( &ms_s, ms_s.header.length));
			ms_s.send( *s );

			dump( "waiting for MS_END from client to exit\n" );

			message ms_r;
			do {
				ms_r.read( *s );

				std::stringstream ss;
				ss << ms_r;
				dump( "Received MESSAGE from client: " + ss.str() );

			} while( ms_r.header.id != message::MS_END );

			host_condition__.notify_all();
		}
		catch( std::exception & e ) {
			dump( "connection exception: " + std::string( e.what()) + "\n" );
		}
	}
};


struct server {
	server( boost::asio::io_service& io_s )	: io_service_(io_s) { }

	void start( int argc, char** argv ) {
		int port = 45000;
		if( argc>1 )
			port = std::atoi( argv[1] );

		thread_ = boost::thread( &server::operator(), this, port );
		thread_.join();
	}

	void operator()(int port) {
		try{
			connect_( port);

			dump( "Host waiting for connections on port: " +
					boost::lexical_cast< std::string>( port ) + "\n" );

			boost::shared_ptr< boost::asio::ip::tcp::socket > s;
			s.reset( new boost::asio::ip::tcp::socket( io_service_ ) );

			acceptor_->accept( *s );

			dump( "Host has accepted a client connection\n" );

			connectionThread c;
			boost::thread thr( &connectionThread::operator(), &c, s );

			dump( "Host waiting for MS_END to exit.\n" );

			boost::mutex::scoped_lock lock( mutex_ );
			host_condition__.wait( lock );

			dump( "Host finished\n" );
		}
		catch( std::runtime_error e ) {
			std::cerr << "  Oops SERVER network error | runtime_error: " << e.what();
		}
		catch( std::exception e ) {
			std::cerr << "  Oops SERVER network error | " << e.what();
		}
	}

private:
	boost::asio::io_service& io_service_;
	boost::shared_ptr< boost::asio::ip::tcp::acceptor> acceptor_;
	boost::thread thread_;
	boost::mutex mutex_;

	void connect_( short port ) {
		try{
			acceptor_.reset( new boost::asio::ip::tcp::acceptor( io_service_,
					 boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port ) ) );
		}
		catch( std::exception e ) {
			throw std::runtime_error("connection error | " + std::string( e.what() ));
		}
	}
};

#endif

