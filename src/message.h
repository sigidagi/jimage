
#ifndef MESSAGE_H_
#define MESSAGE_H_

#include    <boost/asio.hpp>
#include	<boost/variant.hpp>
#include	<boost/archive/text_iarchive.hpp>
#include	<boost/archive/text_oarchive.hpp>
#include	<boost/serialization/variant.hpp>
#include	<boost/serialization/string.hpp>
#include	<sstream>
#include    <iostream>
#include	<string>
#include	<vector>

using std::string;
using boost::variant;
using std::vector;

struct msdata_ack {
	int a1;
    
    //friend boost:;serialization::access;
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & a1;
    }

	friend std::ostream& operator<<(std::ostream& os, msdata_ack const & d ) {
		return os << "a1=" << d.a1;
	}
};

struct msdata_data {
    string fname;
    string isize;
    string fsize;
    
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
    
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int  version)
    {
        ar & e1;
        ar & e2;
    }

	friend std::ostream& operator<<(std::ostream& os, msdata_end const & d ) {
		return os << "e1=" << d.e1 << ", e2=" << d.e2;
	}
};

struct msheader {
	int id;
	size_t length;
	msheader() : id(0), length(0){ }
    
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & id;
        ar & length;
    }

	friend std::ostream& operator<<(std::ostream& os, msheader const & h ) {
		os << "header{ id= " << h.id << ", length= " << h.length << " }";
		return os;
	}
};



struct message {
	enum mstype { MS_ACK = 11,
				  MS_DATA = 22,
				  MS_END = 99 };

	msheader header;
    boost::variant<msdata_ack, msdata_data, msdata_end> messageBody;
    
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & header;
        ar & messageBody; 
    }

	message() {	}
	message( mstype mt ) 
    {
		header.id = static_cast< int >( mt );
        header.length = sizeof(messageBody);
	}

	void setData(msdata_data& message ) 
    {
        messageBody = message;
	}
    
    void setDataSize( size_t datasize, string& outbound_headersize) const
    {
        std::ostringstream headersizeStream;
        headersizeStream << std::setw(8) << std::hex << datasize;

        outbound_headersize = headersizeStream.str();
    }

    template<typename T>
    void setMessageData(T& t, string& outbound_data) const
    {
        std::ostringstream istream;
        boost::archive::text_oarchive archive(istream);
        archive << t; 
        outbound_data = istream.str();
    }

	void send( boost::asio::ip::tcp::socket& s ) const 
    {

        string outbound_header;
        setMessageData(header, outbound_header);

        string outbound_headersize;
        setDataSize(outbound_header.size(), outbound_headersize);

        string outbound_data;
        setMessageData(messageBody, outbound_data);

        string outbound_datasize;
        setDataSize(outbound_data.size(), outbound_datasize);

        std::vector<boost::asio::const_buffer> buffers;
        
        buffers.push_back(boost::asio::buffer(outbound_headersize));
        buffers.push_back(boost::asio::buffer(outbound_header));
        buffers.push_back(boost::asio::buffer(outbound_datasize));
        buffers.push_back(boost::asio::buffer(outbound_data));

		try {
			boost::asio::write( s, buffers );
		}
		catch( std::exception e ) {
			throw std::runtime_error("message send error | " + std::string( e.what() ) );
		}
	}

    size_t read_size( boost::asio::ip::tcp::socket& s)
    {
        
       char inbound_size[8];
       size_t inbound_header_size = 0;

       try{
			boost::asio::read( s, boost::asio::buffer( inbound_size ) );
            std::istringstream iss(string(inbound_size, 8));
            
            if (!(iss >> std::hex >> inbound_header_size))
            {
                // Header doesn't seem to be valid. Inform the caller.
                boost::system::error_code error(boost::asio::error::invalid_argument);
                return 0;
            }
       }
 		catch( std::exception e ) {
			throw std::runtime_error( "message read error | " + std::string( e.what() ) );
		}
        
        return inbound_header_size;

    }
   
    template<typename T>
    void read_data ( boost::asio::ip::tcp::socket& s, size_t length, T& t )
    {

        vector<char> inbound_data(length);
        try {
            
            boost::asio::read(s, boost::asio::buffer(inbound_data));
            
            std::string archive_data(&inbound_data[0], inbound_data.size());
            std::istringstream archive_stream(archive_data);
            boost::archive::text_iarchive archive(archive_stream);
            archive >> t;

        }
 		catch( std::exception e ) {
			throw std::runtime_error( "message read error | " + std::string( e.what() ) );
		}
    }    

	void read( boost::asio::ip::tcp::socket& s ) 
    {
        size_t header_length = read_size(s);
        read_data( s, header_length, header );

        size_t data_length = read_size(s);    
        read_data( s, data_length, messageBody ); 
	}

	friend std::ostream& operator<<(std::ostream& os, message const & ms ) {
		os << ms.header << "   body{ ";
        os <<  ms.messageBody;
		os << " } " << std::endl;
		return os;
	}

};

#endif
