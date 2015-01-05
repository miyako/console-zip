#include "wazip.h"

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

extern char *optarg;
extern int optind, opterr;

static void usage(void){
    fprintf(stderr, "usage: zip [options]\n");
    fprintf(stderr, " -%c string : %s\n", OPT_PASSWORD , "password"); 
    fprintf(stderr, " -%c string : %s\n", OPT_INPUT , "input"); 
    fprintf(stderr, " -%c string : %s\n", OPT_OUTPUT , "output");     
    fprintf(stderr, " -%c number : %s\n", OPT_COMPRESS_LEVEL , "compression level"); 
    fprintf(stderr, " -%c : %s\n", OPT_IGNORE_DOTFILE , "ignore dotfile");     
    fprintf(stderr, " -%c : %s\n", OPT_VERBOSE , "verbose");   
    
    exit(1);
}

#pragma mark -

int main(int argc, char *argv[])
{
    if(argc == 1) usage();
    
    int ch;
    
    std::string password;
    std::string input; 
    std::string output;
    
    unsigned int level = 0;
    unsigned int ignore_dot = 0;
    unsigned int verbose = 0;
    
    while ((ch = getopt(argc, argv, "p:i:o:l:dv")) != -1){
                
        switch (ch){
                case OPT_PASSWORD:
                    password = std::string(optarg);
                    break;
                case OPT_INPUT:
                    input = std::string(optarg);
                    break;
                case OPT_OUTPUT:
                    output = std::string(optarg);
                    break; 
                case OPT_COMPRESS_LEVEL:
                    level = atoi(optarg);
                    break;
                case OPT_IGNORE_DOTFILE:
                    ignore_dot = 1;
                    break; 
                case OPT_VERBOSE:
                    verbose = 1;
                    break;                 
            default:
                usage();
        }
    } 
    
    if(level == 0){
        level = Z_DEFAULT_COMPRESSION; 
    }else if (level > 10){
        level = 9;
    }
    
    if(!input.length() || !output.length()){
        usage();
    }
    
    typedef std::vector<std::string> strings;
    
    zipFile hZip = zipOpen64(output.c_str(), APPEND_STATUS_CREATE);
    
    unsigned int result = 0; 
    
    if(hZip){
        
        strings paths;
        unsigned long CRC;    
        
        if(fs::is_directory(input)){
            
            fs::recursive_directory_iterator last;
            for (fs::recursive_directory_iterator itr(input); itr != last; ++itr){
                std::string filename, filepath;
#ifdef __WINDOWS__
                std::wstring _filename(itr->path().filename().c_str());
                std::wstring _filepath(itr->path().c_str());
                wchar_to_utf8(_filename, filename);
                wchar_to_utf8(_filepath, filepath);
#else
                filename = std::string(fs::path(relativePath).filename().c_str());
#endif                           
                if(fs::is_directory(itr->path())){
                    if(ignore_dot){
                        if(!(filename.at(0) == '.')){ 
                            if(std::string(filepath).find("/.") == std::string::npos){
                                paths.push_back(filepath.append("/")); 
                            } 
                        }
                    }else{
                        paths.push_back(filepath.append("/"));
                    }
                }else{
                    if(ignore_dot){
                        if(!(filename.at(0) == '.')){ 
                            if(filepath.find("/.") == std::string::npos){
                                paths.push_back(filepath);    
                            }
                        }
                    }else{
                        paths.push_back(filepath);
                    } 
                }
            }
        }else{
            std::cout << fs::path(input).filename();
        }  
        
        zip_fileinfo zi;
        time_t currentTime;
        time(&currentTime);
        struct tm *tm;
        tm=localtime(&currentTime);
        zi.tmz_date.tm_sec=tm->tm_sec;
        zi.tmz_date.tm_min=tm->tm_min;
        zi.tmz_date.tm_hour=tm->tm_hour;
        zi.tmz_date.tm_mday=tm->tm_mday;
        zi.tmz_date.tm_mon=tm->tm_mon;
        zi.tmz_date.tm_year=tm->tm_year;
        zi.external_fa = 0;
        zi.internal_fa = 0;
        zi.dosDate = 0;
        zi.internal_fa = zi.external_fa = 0;
    
        unsigned int len = input.length() + 1;
        for (unsigned int i = 0; i < paths.size(); ++i){
        
            std::string relativePath = paths.at(i).substr(len);
            std::string absoluetePath = paths.at(i);
            
            if(password.length()){
                CRC = crc32(0L, Z_NULL, 0);
                std::ifstream ifs_crc(absoluetePath.c_str(), std::ios::in|std::ios::binary);
                if(ifs_crc.is_open()){
                    std::vector<uint8_t> buf(BUFFER_SIZE);
                    while(ifs_crc.good()){
                        ifs_crc.read((char *)&buf[0], BUFFER_SIZE);
                        CRC = crc32(CRC, (const Bytef *)&buf[0], ifs_crc.gcount());
                    }
                    ifs_crc.close();
                }
                
                if(zipOpenNewFileInZip3_64(hZip,
                                           relativePath.c_str(),
                                           &zi,
                                           NULL, 0,
                                           NULL, 0,
                                           NULL,
                                           Z_DEFLATED,
                                           level,
                                           0, 15, 8, Z_DEFAULT_STRATEGY,
                                           (const char *)password.c_str(), CRC, 1) != 0){
                    std::cout << "error:zipOpenNewFileInZip3_64:" << relativePath.c_str() << "\n";
                    result = result | 1;
                    break;
                }
                
            }else{
                
                if(zipOpenNewFileInZip64(hZip,
                                         relativePath.c_str(),
                                         &zi,
                                         NULL, 0,
                                         NULL, 0,
                                         NULL,
                                         Z_DEFLATED,
                                         level,
                                         0) != 0){
                    std::cout << "error:zipOpenNewFileInZip64:" << relativePath.c_str() << "\n";
                    result = result | 1;
                    break;
                }
                
            }
            
            std::ifstream ifs(absoluetePath.c_str(), std::ios::in|std::ios::binary);
            
            if(ifs.is_open()){
                
                std::vector<uint8_t> buf(BUFFER_SIZE);
                
                while(ifs.good()){
                    
                    ifs.read((char *)&buf[0], BUFFER_SIZE);
                    zipWriteInFileInZip(hZip, (char *)&buf[0], ifs.gcount());
                    
                }
                
                ifs.close();
                
                if(verbose){
                    std::cout << "path: " << relativePath.c_str() << "\n";
                }
                
            }
            
            zipCloseFileInZip(hZip);
            
        }
        
        zipClose(hZip, NULL);
        
    }

	return result;
}
