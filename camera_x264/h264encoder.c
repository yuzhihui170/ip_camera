#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "h264encoder.h"

#define CLEAR(p,size) (memset(p,0,sizeof(size)))

FILE* h264_fp_t;

int iNal   = 0;
x264_nal_t* pNals = NULL;
x264_t* pHandle   = NULL;
x264_picture_t* pPic_in; 
x264_picture_t* pPic_out; 
x264_param_t* pParam;


void compress_begin(Encoder *en, int width, int height) {
	int ret;
	pPic_in = (x264_picture_t*)malloc(sizeof(x264_picture_t));
	pPic_out = (x264_picture_t*)malloc(sizeof(x264_picture_t));
	pParam = (x264_param_t*)malloc(sizeof(x264_param_t));

	CLEAR(pParam,sizeof(x264_param_t));
	CLEAR(pPic_in, sizeof(x264_picture_t));
	
	x264_param_default(pParam); //set default param
	pParam->i_width   = width;
	pParam->i_height  = height;
	pParam->i_log_level  = X264_LOG_DEBUG;
	pParam->i_threads  = X264_SYNC_LOOKAHEAD_AUTO;
	pParam->i_frame_total = 0;
	pParam->i_keyint_max = 10;
	pParam->i_bframe  = 5;
	pParam->b_open_gop  = 0;
	pParam->i_bframe_pyramid = 0;
	pParam->rc.i_qp_constant=0;
	pParam->rc.i_qp_max=0;	
	pParam->rc.i_qp_min=0;
	pParam->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
	pParam->i_fps_den  = 1;
	pParam->i_fps_num  = 30;
	pParam->i_timebase_den = pParam->i_fps_num;
	pParam->i_timebase_num = pParam->i_fps_den;
	pParam->b_intra_refresh = 0;
	pParam->b_annexb = 1;
	pParam->i_csp=X264_CSP_I420;
	x264_param_apply_profile(pParam, x264_profile_names[0]); //x264_profile_names[5]

	pHandle = x264_encoder_open(pParam);

	x264_picture_init(pPic_out);
	x264_picture_alloc(pPic_in, X264_CSP_I420, pParam->i_width, pParam->i_height);
	
	h264_fp_t = fopen("yzh.h264\0", "wb");
	if(NULL == h264_fp_t) {
		fprintf(stderr, "[%s|%s|%d][%s] can't open yzh.264\n",__FILE__, __FUNCTION__, __LINE__, strerror(errno));
	}
}

int compress_frame(Encoder *en, int type, uint8_t *in, FILE* h264_fp) {
	int ret = 0;
	int i = 0;
	int j = 0;

	if(h264_fp_t == NULL) {
		fprintf(stderr, "[ %s|%s|%d ] h264_fp is null\n",__FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	
	pPic_in->img.plane[0] = in;
	pPic_in->img.plane[1] = in + pParam->i_width * pParam->i_height;
	pPic_in->img.plane[2] = in + (pParam->i_width * pParam->i_height * 5 / 4);

	//fprintf(stdout, "[ %s|%s|%d ] %p / %p / %p \n",__FILE__, __FUNCTION__, __LINE__, pPic_in->img.plane[0],pPic_in->img.plane[1],pPic_in->img.plane[2]); 
	
	switch (type) {
	case 0:
		pPic_in->i_type = X264_TYPE_P;
		break;
	case 1:
		pPic_in->i_type = X264_TYPE_IDR;
		break;
	case 2:
		pPic_in->i_type = X264_TYPE_I;
		break;
	default:
		pPic_in->i_type = X264_TYPE_AUTO;
		break;
	}

	ret = x264_encoder_encode(pHandle, &pNals, &iNal, pPic_in, pPic_out);
	if (ret< 0){
		fprintf(stderr, "[%s|%s|%d] x264_encoder_encode error\n",__FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	
	printf("Succeed encode frame: %5d  iNal = %d\n", i, iNal);
	for ( j = 0; j < iNal; ++j){
		fwrite(pNals[j].p_payload, 1, pNals[j].i_payload, h264_fp_t);
		printf("Succeed encode frame: %5d  pNals[j].i_payload = %d\n", j, pNals[j].i_payload);
	}
		
	pPic_in->i_pts++;
	
	return ret;
}

void compress_end(Encoder *en) {
	if (pPic_in != NULL) {
		x264_picture_clean(pPic_in);
		free(pPic_in);
		pPic_in = NULL;
	}
	
	if(pPic_out != NULL) {
		free(pPic_out);
		pPic_out = NULL;
	}

	if (pParam != NULL) {
		free(pParam);
		pParam = NULL;
	}
	
	if (pHandle != NULL) {
		x264_encoder_close(pHandle);
		pHandle = NULL;
	}
	fclose(h264_fp_t);
}

