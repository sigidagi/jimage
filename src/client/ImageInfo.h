// =====================================================================================
// 
//       Filename:  ImageInfo.h
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  03/15/2011 09:44:05 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Sigitas Dagilis (), sigidagi@gmail.com
//
// =====================================================================================


#ifndef  IMAGEINFO_INC
#define  IMAGEINFO_INC

#include	<string>
#include	<vector>
#include	"../message.h"
#include    "json/json_spirit.h"

using std::string;
using std::vector;
using json_spirit::Object;
using json_spirit::Array;
using json_spirit::Pair;

namespace jimage {
    

    class ImageInfo
    {
         private:
            Object root_;
            vector<msdata_data> infos_;

            // private methods
            string  fileExtension(const string& pathToFile);
            void execEXIF(Object& obj, const string& file);
            
            Object& addChild(Array& parent);
            Array& addArray(Object& obj, const string& name);

         public:
            Object& getIt(const string& directory, const string& rootName);
            vector<msdata_data>& getInfo();
    };

}

#endif   // ----- #ifndef IMAGEINFO_INC  -----
