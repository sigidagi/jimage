// =====================================================================================
// 
//       Filename:  main.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  2011-03-11 13:32:19
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Sigitas Dagilis (), sigidagi@gmail.com
//
// =====================================================================================


#include	"ImageInfo.h"
#include	<iostream>
#include	<fstream>

using std::cout;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Missing filepath as argument!\n";
        return 1;
    }
        
    jimage::ImageInfo info;
    string directory = argv[1];
    string rootName = "Images";

    Object& imagesInfo = info.getIt(directory, rootName);
    
    // write to a file
    string name = "image_info.json";
    std::ofstream os(name.c_str());
    if (os.is_open())
    {
        json_spirit::write_formatted(imagesInfo, os);
        os.close();
    }

    return 0;
}
