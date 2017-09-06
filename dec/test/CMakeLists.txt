cmake_minimum_required(VERSION 3.5)

SET (CMAKE_C_FLAGS " -DOSCL_EXPORT_REF= -DOSCL_IMPORT_REF= -DBX_RC -DLINUX_X86 -fpermissive -std=c++11")

SET (mpeg4_src_files
        m4v_h263_dec_test.cpp
)

INCLUDE_DIRECTORIES(
        ../src
        ../include
)

LINK_DIRECTORIES(
        ../../lib
)

ADD_DEFINITIONS (${CMAKE_C_FLAGS})

ADD_EXECUTABLE(stagefright_m4vh263dec_test ${mpeg4_src_files})

TARGET_LINK_LIBRARIES (stagefright_m4vh263dec_test stagefright_m4vh263dec)

SET (EXECUTABLE_OUTPUT_PATH ../../bin)

#comment all the ALOGE in src/*.cpp
