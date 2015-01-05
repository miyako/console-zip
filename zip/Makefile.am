AUTOMAKE_OPTIONS = foreign subdir-objects
CFLAGS=-Wall -I/usr/local/include -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\"" 
LDFLAGS=-L/usr/local/lib -L/usr/lib -lstdc++ -lz -lboost_filesystem -lboost_system
CPPFLAGS=-Wall -I/usr/local/include -DNDEBUG
bin_PROGRAMS = wazip
wazip_SOURCES = wazip.cc zip/zip.c zip/ioapi.c
