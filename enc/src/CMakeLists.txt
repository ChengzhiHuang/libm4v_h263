cmake_minimum_required(VERSION 3.5)

#SET(CMAKE_C_FLAGS " -DBX_RC -DOSCL_IMPORT_REF= -DOSCL_UNUSED_ARG(x)=(void)(x) -DOSCL_EXPORT_REF=  -Werror  ")
SET(CMAKE_C_FLAGS " -DBX_RC -DOSCL_IMPORT_REF= -DOSCL_EXPORT_REF=  -Werror  ")

SET(libm4vh263_top_srcdir ../../)

SET(m4vh263_src_files
        bitstream_io.cpp
	combined_encode.cpp
	datapart_encode.cpp
	dct.cpp
	findhalfpel.cpp
	fastcodemb.cpp
	fastidct.cpp
	fastquant.cpp
	me_utils.cpp
	mp4enc_api.cpp
	rate_control.cpp
	motion_est.cpp
	motion_comp.cpp
	sad.cpp
	sad_halfpel.cpp
	vlc_encode.cpp
	vop.cpp
)

INCLUDE_DIRECTORIES(
        ${libm4vh263_top_srcdir}/frameworks/av/media/libstagefright/include
        ${libm4vh263_top_srcdir}/frameworks/native/include/media/openmax
        ${libm4vh263_top_srcdir}/enc/src
        ${libm4vh263_top_srcdir}/enc/include
	#/usr/include/android/
)

ADD_DEFINITIONS(${CMAKE_C_FLAGS})

ADD_LIBRARY(stagefright_m4vh263enc STATIC ${m4vh263_src_files})

SET(LIBRARY_OUTPUT_PATH ${libm4vh263_top_srcdir}/lib)

#OSCL_UNUSED_ARG enc/src/fastquant.cpp 中的全部