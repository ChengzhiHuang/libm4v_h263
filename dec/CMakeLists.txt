cmake_minimum_required(VERSION 3.5)

SET (CMAKE_C_FLAGS " -DOSCL_EXPORT_REF -DOSCL_IMPORT_REF -Werror -std=gnu++14 -fpermissive")

SET (libmpeg4_top_srcdir ../)

SET (mpeg4_src_files
	SoftMPEG4.cpp
)

INCLUDE_DIRECTORIES(
		frameworks/av/media/libstagefright/include
		frameworks/native/include/media/openmax
		src/
		include/
		frameworks/core/libutils/include#for utils/Log.h
		frameworks/core/include#for log/log.h
        frameworks/native/include#for media/hardware/VideoAPI.h
        frameworks/av/include#for media/IOMX.h
        frameworks/libhardware/include#for hardware/hardware.h
        frameworks/native/libs/arect/include#for android/rect.h
        frameworks/libhidl/transport/token/1.0/utils/include#for hidl/HybridInterface.h
        frameworks/core/base/include/#for android-base/unique_fd.h
        frameworks/libhidl/base/include#for hidl/HidlSupport.h
)

ADD_DEFINITIONS (${CMAKE_C_FLAGS})
LINK_DIRECTORIES(
		${libmpeg4_top_srcdir}/lib #libutils
)

ADD_LIBRARY (stagefright_soft_mpeg4dec SHARED ${mpeg4_src_files} )
LINK_LIBRARIES(stagefright_soft_mpeg4dec media stagefright_omx stagefright_foundation utils log)
SET(LIBRARY_OUTPUT_PATH ${libmpeg4_top_srcdir}/lib)


#改变了 hidl/HidlSupport.h hidl_vec 类成员 mbuffer 的初始化
#改变了 native/include/gui/IGraphicBufferProducer.h 中 HGraphicBufferProducer 的def
#改变了 av/media/libstagefright/include/media/stagefright/foundation/ADebug.h 中 inline static bool isExperimentEnabled() 的参数列表
#改变了 av/media/libstagefright/include/media/stagefright/foundation/AUtils.h 中 void ENSURE_UNSIGNED_TYPE() 的实现
# 两个 inline static bool isInRange 的 __attribute__
#改变了 /dec/include/mp4dec_api.h //typedef uint Bool

#修改了dec/include/mp4dec_api.h 中的定义
#	#ifdef LINUX_X86
#	typedef uint Bool;
#	#endif
#解决了在编译stagefright_m4vh263dec和stagefright_soft_mpeg4dec的时候，Bool变量的重定义