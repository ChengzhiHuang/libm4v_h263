cmake_minimum_required(VERSION 3.5)

SET (CMAKE_C_FLAGS " -DOSCL_EXPORT_REF= -DOSCL_IMPORT_REF=  -Werror -DLINUX_X86")

SET (libm4vh263_top_srcdir ../../)

SET (m4vh263_src_files
        adaptive_smooth_no_mmx.cpp
        bitstream.cpp
        block_idct.cpp
        cal_dc_scaler.cpp
        chvr_filter.cpp
        chv_filter.cpp
        combined_decode.cpp
        conceal.cpp
        datapart_decode.cpp
        dcac_prediction.cpp
        dec_pred_intra_dc.cpp
	deringing_chroma.cpp
	deringing_luma.cpp
	find_min_max.cpp
	get_pred_adv_b_add.cpp
	get_pred_outside.cpp
	idct.cpp
	idct_vca.cpp
	mb_motion_comp.cpp
	mb_utils.cpp
	packet_util.cpp
	post_filter.cpp
	post_proc_semaphore.cpp
	pp_semaphore_chroma_inter.cpp
	pp_semaphore_luma.cpp
	pvdec_api.cpp
	scaling_tab.cpp
	vlc_decode.cpp
	vlc_dequant.cpp
	vlc_tab.cpp
	vop.cpp
	zigzag_tab.cpp
)

INCLUDE_DIRECTORIES(
        ${libm4vh263_top_srcdir}/frameworks/av/media/libstagefright/include
        ${libm4vh263_top_srcdir}/frameworks/native/include/media/openmax
        ${libm4vh263_top_srcdir}/dec/src
        ${libm4vh263_top_srcdir}/dec/include
	/usr/include/android/
)
#删除了dec/include/mp4dec_api.h里面使用的宏定义error: ‘OSCL_IMPORT_REF’
#OSCL_IMPORT_REF是用来隐藏某些第三方库的使用
#位于opencore/oscl/oscl/config/linux 的osclconfig.h中
#define OSCL_EXPORT_REF __attribute__ ((visibility("default")))
#define OSCL_IMPORT_REF __attribute__ ((visibility("default")))


ADD_DEFINITIONS (${CMAKE_C_FLAGS})

ADD_LIBRARY (stagefright_m4vh263dec STATIC ${m4vh263_src_files})

SET(LIBRARY_OUTPUT_PATH ${libm4vh263_top_srcdir}/lib)
