#ifndef __WAZIP_H__
#define __WAZIP_H__

#define OPT_PASSWORD        'p'
#define OPT_INPUT           'i'
#define OPT_OUTPUT          'o'
#define OPT_COMPRESS_LEVEL  'l'
#define OPT_IGNORE_DOTFILE  'd'
#define OPT_VERBOSE         'v'

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
int wchar_to_utf8(std::wstring &wstr, std::string &str);
int utf8_to_wchar(std::string &str, std::wstring &wstr);
#endif
#endif

#endif