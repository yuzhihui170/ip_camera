#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "h264encoder.h"

FILE* h264_fp_t;

void compress_begin(Encoder *en, int width, int height) {
	en->param = (x264_param_t *) malloc(sizeof(x264_param_t));
	en->picture = (x264_picture_t *) malloc(sizeof(x264_picture_t));
	x264_param_default(en->param); //set default param
	//en->param->rc.i_rc_method = X264_RC_CQP;//设置为恒定码率
	// en->param->i_log_level = X264_LOG_NONE;
	en->param->i_threads  = X264_SYNC_LOOKAHEAD_AUTO;//取空缓存区使用不死锁保证
	en->param->i_width = width; //set frame width
	en->param->i_height = height; //set frame height
	en->param->i_frame_total = 0;
	en->param->i_keyint_max = 10;
	en->param->rc.i_lookahead = 0; //表示i帧向前缓冲?	en->param->rc.i_qp_constant=0;
	en->param->rc.i_qp_max=0;
	en->param->rc.i_qp_min=0;
	en->param->i_bframe = 5; //两个参考帧之间b帧的数目
	en->param->b_open_gop = 0;
	//en->param->i_bframe_pyramid = 0; //允许部分B为参考帧
	en->param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
	//en->param->rc.i_bitrate = 1024 * 10;//rate 为10 kbps
	en->param->i_fps_num = 5; //帧率分子
	en->param->i_fps_den = 1; //帧率分母
	en->param->i_timebase_den = en->param->i_fps_num;
	en->param->i_timebase_num = en->param->i_fps_den;
	en->param->b_intra_refresh = 1;
	en->param->b_annexb = 1;
	en->param->i_csp = X264_CSP_I422; //设置压缩时的图片格式

	en->picture->img.i_csp = X264_CSP_I422; //设置如何的图片格式 两者要一致

	x264_param_apply_profile(en->param, x264_profile_names[0]); //使用baseline
	if((en->handle = x264_encoder_open(en->param)) == 0) {
		fprintf(stderr, "[%s|%s|%d] error!\n",__FILE__, __FUNCTION__, __LINE__);
		return;
	}
		
	/* Create a new pic */
	x264_picture_alloc(en->picture, X264_CSP_I422, en->param->i_width, en->param->i_height); //422
	
	//x264_picture_alloc(en->picture, X264_CSP_I420, en->param->i_width,  en->param->i_height); //420
	//en->picture->img.i_csp = X264_CSP_I420;
	//en->picture->img.i_plane = 3;

	h264_fp_t = fopen("yzh.264", "wb");
	if(NULL == h264_fp_t) {
		fprintf(stderr, "[%s|%s|%d][%s] can't open yzh.264\n",__FILE__, __FUNCTION__, __LINE__, strerror(errno));
	}
}

int compress_frame(Encoder *en, int type, uint8_t *in, FILE* h264_fp) {
	x264_picture_t pic_out;
	int nNal = -1;
	int result = 0;
	int i = 0;
	x264_picture_t* pPic_out = (x264_picture_t*)malloc(sizeof(x264_picture_t));

	if(h264_fp_t == NULL) {
		fprintf(stderr, "[ %s|%s|%d ] h264_fp is null\n",__FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

	x264_picture_init(pPic_out);

#if 0 
	en->picture->img.plane[0] = in;
	en->picture->img.plane[1] = in + en->param->i_width * en->param->i_height;
	en->picture->img.plane[2] = in + (en->param->i_width * en->param->i_height * 3 / 2);
#endif

#if 1
	char *y = en->picture->img.plane[0];
	char *u = en->picture->img.plane[1];
	char *v = en->picture->img.plane[2];

	int is_y = 1, is_u = 1;
	int y_index = 0, u_index = 0, v_index = 0;

	int yuv422_length = 2 * en->param->i_width * en->param->i_height;

	//序列为YU YV YU YV，一个yuv422帧的长度 width * height * 2 个字节
	for (i = 0; i < yuv422_length; ++i) {
		if (is_y) {
			*(y + y_index) = *(in + i);
			++y_index;
			is_y = 0;
		} else {
			if (is_u) {
				*(u + u_index) = *(in + i);
				++u_index;
				is_u = 0;
			} else {
				*(v + v_index) = *(in + i);
				++v_index;
				is_u = 1;
			}
			is_y = 1;
		}
	}
#endif 

	switch (type) {
	case 0:
		en->picture->i_type = X264_TYPE_P;
		break;
	case 1:
		en->picture->i_type = X264_TYPE_IDR;
		break;
	case 2:
		en->picture->i_type = X264_TYPE_I;
		break;
	default:
		en->picture->i_type = X264_TYPE_AUTO;
		break;
	}

	if (x264_encoder_encode(en->handle, &(en->nal), &nNal, en->picture,	&pic_out) < 0) {
		fprintf(stderr, "[ %s|%s|%d ] x264_encoder_encode error!\n",__FILE__, __FUNCTION__, __LINE__);
	}
	
	printf("Succeed encode frame: nNal = %d\n",nNal);
	for (i = 0; i < nNal; i++) {
		fwrite(en->nal[i].p_payload, 1, en->nal[i].i_payload, h264_fp_t);
		printf("----> Succeed encode frame: %5d  en->nal[i].i_payload = %d\n", i, en->nal[i].i_payload);
		
	}
	en->picture->i_pts++;
	x264_picture_clean(pPic_out);		
	return result;
}

void compress_end(Encoder *en) {
	if (en->picture) {
		x264_picture_clean(en->picture);
		free(en->picture);
		en->picture = 0;
	}
	if (en->param) {
		free(en->param);
		en->param = 0;
	}
	if (en->handle) {
		x264_encoder_close(en->handle);
	}
	fclose(h264_fp_t);
}
