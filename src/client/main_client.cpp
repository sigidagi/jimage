#include   "client.h"
#include   "../message.h"
#include	"ImageInfo.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include	<vector>

using std::vector;
using namespace jimage;

int main( int argc, char** argv )
{

    if (argc < 2)
    {
        std::cout << "Missing filepath as argument!\n";
        return 1;
    }
        
    ImageInfo info;
    string directory = argv[1];
    string rootName = "Images";

    //Object& imagesInfo = info.getIt(directory, rootName);
    info.getIt(directory, rootName);
    vector<msdata_data>& im = info.getInfo();
    
  try
  {
    std::cout << "Synchronous TCP asio CLIENT " << std::endl;

    boost::asio::io_service io_service;
    client c( io_service );
    c.connect( argc, argv );

    // Read ACK from Host
    message ms;
    ms.read( c.socket() );
    std::cout << " client# Received message from Host: "<< ms << std::endl;

    std::cout << " client# Sending Image Info to Host. " << std::endl;
    for(size_t n=0; n<im.size();++n)
    {
        // Send some DATA stuff to Host
        std::cout << "Image " << n << ": filename=" << im[n].fname << " size=" << im[n].fsize << " image=" << im[n].isize << "\n";
        message ms_d( message::MS_DATA);
        ms_d.setData( im[n] );
        ms_d.send( c.socket() );

        boost::posix_time::millisec t(1200);
        boost::this_thread::sleep( t );
    }

    // Send END to Host
    std::cout << " client# Send MS_END to Host. " << std::endl;
    message ms_e( message::MS_END);
    ms_e.send( c.socket() );

  }
  catch (std::runtime_error& e)  {
    std::cerr << "Client Exception: " << e.what() << "\n";
  }
  catch (std::exception& e)  {
      std::cerr << "Client Exception: " << e.what() << "\n";
    }
  return 0;
}

