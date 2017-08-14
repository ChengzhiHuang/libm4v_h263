PROJECT (libstagefright_soft_mpeg4dec)

SET (CMAKE_C_FLAGS " -DOSCL_EXPORT_REF= -DOSCL_IMPORT_REF= -Werror ") 

SET (libmpeg4_top_srcdir ../)

SET (mpeg4_src_files
	SoftMPEG4.cpp
)
 
INCLUDE_DIRECTORIES(
	${libmpeg4_top_srcdir}/frameworks/av/media/libstagefright/include
	${libmpeg4_top_srcdir}/frameworks/native/include/media/openmax
)

ADD_DEFINITIONS (${CMAKE_C_FLAGS})

ADD_LIBRARY (libstagefright_soft_mpeg4dec SHARED ${mpeg4_src_files} stagefright_m4vh263dec media stagefright_omx stagefright_foundation utils log)

SET(LIBRARY_OUTPUT_PATH ${libmpeg4_top_srcdir}/lib)