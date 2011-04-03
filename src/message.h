
#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <boost/asio.hpp>

#include <iostream>
#include <cstring>
#include	<string>

using std::string;

struct msdata_ack {
	int a1;

	friend std::ostream& operator<<(std::ostream& os, msdata_ack const & d ) {
		return os << "a1=" << d.a1;
	}
};

struct msdata_data {
    char fname [64];
    char isize [64];
    char fsize [64];
    
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & fname;
        ar & isize;
        ar & fsize;
    }

	friend std::ostream& operator<<(std::ostream& os, msdata_data const & d ) {
		return os << "name=" << d.fname << ", image=" << d.isize << ", size=" << d.fsize;
	}
};

struct msdata_end {
	int e1;
	int e2;

	friend std::ostream& operator<<(std::ostream& os, msdata_end const & d ) {
		return os << "e1=" << d.e1 << ", e2=" << d.e2;
	}
};

struct msheader {
	int id;
	int length;
	msheader() : id(0), length(0){ }

	friend std::ostream& operator<<(std::ostream& os, msheader const & h ) {
		os << "header{ id= " << h.id << ", length= " << h.length << " }";
		return os;
	}
};

union msbody {
	msdata_ack ms_ack;
	msdata_data ms_data;
	msdata_end ms_end;
	msbody() {
		memset(this, 0, sizeof(*this) );
	}
};

struct message {
	enum mstype { MS_ACK = 11,
				  MS_DATA = 22,
				  MS_END = 99 };

	msheader header;
	msbody body;

	message() {	}

	message( mstype mt ) {
		switch( mt ) {
			case MS_ACK : header.length = sizeof( msdata_ack ); break;
			case MS_DATA : header.length = sizeof( msdata_data );break;
			case MS_END : header.length = sizeof( msdata_end ); break;
			default: header.length = sizeof( msbody );
		}

		header.id = static_cast< int >( mt );
		header.length += sizeof(msheader);
	}

	void setData(msdata_data& message ) 
    {
        // 
		memcpy(body.ms_data.fname, message.fname, sizeof(message.fname));
		memcpy(body.ms_data.isize, message.isize, sizeof(message.isize));
		memcpy(body.ms_data.fsize, message.fsize, sizeof(message.fsize));
	}

	void send( boost::asio::ip::tcp::socket& s ) const {
		try {
			boost::asio::write( s, boost::asio::buffer( this, header.length));
		}
		catch( std::exception e ) {
			throw std::runtime_error("message send error | " + std::string( e.what() ) );
		}
	}

	void read( boost::asio::ip::tcp::socket& s ) {
		try {
			// Read header
			size_t lh = boost::asio::read( s, boost::asio::buffer( &header, sizeof(header)));

			// Read body
			boost::asio::read( s, boost::asio::buffer( &body, header.length - lh ));
		}
		catch( std::exception e ) {
			throw std::runtime_error( "message read error | " + std::string( e.what() ) );
		}
	}


	friend std::ostream& operator<<(std::ostream& os, message const & ms ) {
		os << ms.header << "   body{ ";
		switch( ms.header.id ) {
			case MS_ACK : os << ms.body.ms_ack; break;
			case MS_DATA : os << ms.body.ms_data; break;
			case MS_END : os << ms.body.ms_end; break;
		}
		os << " } " << std::endl;
		return os;
	}
};

#endif
