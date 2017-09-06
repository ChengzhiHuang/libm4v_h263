cmake_minimum_required(VERSION 3.5)

SET (CMAKE_C_FLAGS "-DOSCL_EXPORT_REF= -DOSCL_IMPORT_REF= -DBX_RC -std=gnu++14")

SET (mpeg4_src_files
        m4v_h263_enc_test.cpp
)

INCLUDE_DIRECTORIES(
        ../src
        ../include
)

LINK_DIRECTORIES(
        ../../lib
)

ADD_DEFINITIONS (${CMAKE_C_FLAGS})

ADD_EXECUTABLE(stagefright_m4vh263enc_test ${mpeg4_src_files})

TARGET_LINK_LIBRARIES (stagefright_m4vh263enc_test stagefright_m4vh263enc)

SET (EXECUTABLE_OUTPUT_PATH ../../bin)

#delete all OSCL_EXPORT_REF in enc/include/mp4enc_api.h