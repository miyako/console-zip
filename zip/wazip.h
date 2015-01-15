#ifndef __WAZIP_H__
#define __WAZIP_H__

#define OPT_PASSWORD        'p'
#define OPT_INPUT           'i'
#define OPT_OUTPUT          'o'
#define OPT_COMPRESS_LEVEL  'l'
#define OPT_IGNORE_DOTFILE  'd'
#define OPT_VERBOSE         'v'

#define OPT_LIST            "p:i:o:l:dv"

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>

#include "zlib.h"
#include "zconf.h"
#include "zip/zip.h"

#define BUFFER_SIZE 8192

#ifndef __linux__
#ifndef __APPLE__
#define __WINDOWS__
#include <string>
#include <vector>
#ifndef strings
typedef std::vector<std::string> strings;
#endif
void get_subpaths(std::string &path, std::string &inputfilename, strings *paths, strings *posix_paths, int ignore_dot);
#endif
#endif

#endif