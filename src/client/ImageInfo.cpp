// =====================================================================================
// 
//       Filename:  ImageInfo.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  03/15/2011 09:53:40 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Sigitas Dagilis (), sigidagi@gmail.com
//
// =====================================================================================


#include	"ImageInfo.h"
#include	<iostream>
#include	<vector>
#include	<algorithm>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sstream>
#include	<iterator>
#include	<boost/filesystem.hpp>
#include	<boost/progress.hpp>
#include	<fstream>

//#define foreach BOOST_FOREACH

using std::cout;
using std::string;
using std::vector;

namespace jimage {

    string stripToExtension(const string& pathToFile)
    {

        string extensionName;
        string::size_type idx = pathToFile.rfind('.');
    	if (idx != string::npos)
    	{
            extensionName = pathToFile.substr(idx+1, string::npos);
    	}

        return extensionName;
    }

    void removeWhitespace(string& str)
    {
        size_t found;
        found = str.find_last_not_of(' ');
        if (found != string::npos)
            str.erase(found + 1);
    }

        string  ImageInfo::fileExtension(const string& pathToFile)
        {
                string extensionName;
                string::size_type idx = pathToFile.rfind('.');
                if (idx != string::npos)
                {
                    extensionName = pathToFile.substr(idx+1, string::npos);
                }

                return extensionName;
        }
        
        void ImageInfo::execEXIF(Object& obj, const string& file)
        {
            string command = "exiftool " + file + " > tempFile";
            system(command.c_str());
            std::ifstream fileStream;
            std::stringstream ss;
            string line, cell1, cell2;
            msdata_data image_info;

            fileStream.open("tempFile", std::ios::in);
            if (fileStream.is_open())
            {
                while(!fileStream.eof())
                {
                    getline(fileStream, line);
                    ss << line;
                    getline(ss, cell1, ':');
                    getline(ss, cell2, ':');
                    
                    removeWhitespace(cell1);
                    
                    if (cell1 == "File Name")
                    {
                        removeWhitespace(cell2);
                        image_info.fname = cell2;
                    }
                    if (cell1 == "File Size")
                    {
                         removeWhitespace(cell2);
                         image_info.fsize = cell2;
                    }
                    if (cell1 == "Image Size")
                    {
                        removeWhitespace(cell2);
                        image_info.isize = cell2;
                    }

                    obj.push_back( json_spirit::Pair(cell1, cell2) ); 
                    ss.clear();
                    ss.str("");
                }
                
                fileStream.close();
            }
            
            infos_.push_back(image_info);
        }
        
        Object& ImageInfo::addChild(Array& parent)
        {
            parent.push_back( Object() );
            return parent.back().get_obj();
        }
        
        Array& ImageInfo::addArray(Object& obj, const string& name)
        {
            obj.push_back( Pair( name, Array() ) );
            return obj.back().value_.get_array();
        }

        Object& ImageInfo::getIt(const string& directory, const string& rootName)
        {
            Array& collection( addArray(root_, rootName) );

            boost::filesystem::path pathTo(directory.c_str());
            try
            {
                if (!boost::filesystem::exists(pathTo))
                {
                    cout << "No filesysytem exist!\n";
                    return root_;
                }
                
                vector<string> files;
                string fileext;
                typedef boost::filesystem::directory_iterator dir_iter;
                
                for (dir_iter it(pathTo); it != dir_iter(); ++it)
                {
                    if (boost::filesystem::is_regular_file(it->status()) )
                    {
                        fileext = stripToExtension(it->path().filename());
                        if (fileext == "jpg" || fileext == "JPG")
                            files.push_back(it->path().filename());
                    }
                }

                if (files.empty())
                {
                    cout << "No regular files\n";
                    return root_;
                }
                
                chdir(directory.c_str());
                // TODO: check for extensions

                boost::progress_display progr((unsigned long)files.size());
                for (size_t i = 0; i < files.size(); ++i)
                {
                    Object& child( addChild(collection) );            
                    execEXIF(child, files[i]);
                    ++progr;
                }

            }
            catch(const boost::filesystem::filesystem_error& ex)
            {
                std::cout << ex.what() << "\n";
                return root_;
            }
            

            return root_;
        } // function getIt end;
        
        vector<msdata_data>& ImageInfo::getInfo()
        {
            return infos_;
        }

} // namespace foto 

