PROJECT (libstagefright_m4vh263dec)

SET (CMAKE_C_FLAGS " -DOSCL_EXPORT_REF= -DOSCL_IMPORT_REF= -Werror ") 

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
)

ADD_DEFINITIONS (${CMAKE_C_FLAGS})

ADD_LIBRARY (libstagefright_m4vh263dec STATIC ${m4vh263_src_files})

SET(LIBRARY_OUTPUT_PATH ${libm4vh263_top_srcdir}/lib)