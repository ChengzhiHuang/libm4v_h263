cmake_minimum_required (VERSION 2.8.4)

PROJECT(libm4v_h263)

FILE (MAKE_DIRECTORY lib bin)

ADD_SUBDIRECTORY (dec/src)

ADD_SUBDIRECTORY (dec)
