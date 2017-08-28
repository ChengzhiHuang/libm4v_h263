cmake_minimum_required(VERSION 3.5)

SET (CMAKE_C_FLAGS " -DBX_RC -DOSCL_IMPORT_REF= -DOSCL_EXPORT_REF= -std=gnu++14 -Werror=multichar")

SET (libmpeg4_top_srcdir ../)

SET (mpeg4_src_files
        SoftMPEG4Encoder.cpp
)

ADD_DEFINITIONS (${CMAKE_C_FLAGS})

INCLUDE_DIRECTORIES(
        frameworks/av/media/libstagefright/include
        frameworks/native/include/media/openmax
        frameworks/native/include/media/hardware
        src
        include
        frameworks/core/libutils/include#for utils/Log.h
        frameworks/core/include#for log/log.h
        frameworks/native/include#for media/hardware/OMXPluginBase.h
        frameworks/av/include#for media/MediaDefs.h
        frameworks/core/base/include/#for android-base/unique_fd.h
        frameworks/media/audio/include  #for system/audio.h
        frameworks/libhardware/include #for hardware/audio_effect.h
        frameworks/native/libs/arect/include#for android/rect.h
        frameworks/libhidl/transport/token/1.0/utils/include/#for hidl/HybridInterface.h
        frameworks/libhidl/base/include#for hidl/HidlSupport.h
)

LINK_DIRECTORIES(
        ${libmpeg4_top_srcdir}/lib #libutils
)
ADD_LIBRARY (stagefright_soft_mpeg4enc SHARED ${mpeg4_src_files} )
LINK_LIBRARIES(stagefright_soft_mpeg4enc media stagefright_omx utils log)
SET(LIBRARY_OUTPUT_PATH ${libmpeg4_top_srcdir}/lib)

# native/include/binder/Parcel.h  add #include <iostream>  #include <memory> #include <limits>
# av/media/libstagefright/include/media/stagefright/MediaBuffer.h  commet int32_t unused[6] __attribute__((__unused__));