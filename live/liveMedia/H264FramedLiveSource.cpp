/*
*  H264FramedLiveSource.cpp
*/

#include "H264FramedLiveSource.hh"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

extern class Cameras Camera; //in mainRTSPServer.cpp

#define WIDTH 320
#define HEIGHT 240
#define widthStep 960
#define ENCODER_TUNE   "zerolatency"
#define ENCODER_PROFILE  "baseline"
#define ENCODER_PRESET "veryfast"
#define ENCODER_COLORSPACE X264_CSP_I420

#define FIFO "/tmp/myfifo"
#define BUFF_SIZE 200000


#define CLEAR(x) (memset((&x),0,sizeof(x)))
unsigned char *mem = NULL;
int length;
int fd = -1;
int real_read;
Fifo *fifo;

void Convert(unsigned char *RGB, unsigned char *YUV, unsigned int width, unsigned int height);


H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment& env, char const* fileName, unsigned preferredFrameSize, unsigned playTimePerFrame) : FramedSource(env)
{
    //fp = fopen(fileName, "rb");
#if 0
    printf("###################\n");
    fp = fopen("test.xxx", "rb");
    if(fp != NULL) {
        fseek(fp,0,SEEK_END);
	    length = ftell(fp);
	    printf("fp  length = %ld\n",length);
	    fseek(fp ,0,SEEK_SET);
        mem = (unsigned char*)malloc(length);
        memset(mem,0,length);
        fread(mem,1,length,fp);
    }
#endif 
    //accessȷ���ļ����ļ��еķ���Ȩ�ޡ��������ĳ���ļ��Ĵ�ȡ��ʽ
    //���ָ���Ĵ�ȡ��ʽ��Ч����������0������������-1
    //��������FIFO���򴴽�һ��
    if(access(FIFO,F_OK)==-1){
        if((mkfifo(FIFO, 0666)<0)&&(errno!=EEXIST)){
            printf("Can NOT create fifo file!\n");
            exit(1);
        }
    }
    //��ֻ����ʽ��FIFO�������ļ�������fd    
    if((fd=open(FIFO,O_RDONLY)) == -1){
        printf("Open fifo error!\n");
        exit(1);
    }
    
    fifo = new Fifo();
    

    mem = (unsigned char*)malloc(BUFF_SIZE);
    if(mem == NULL) {
        printf("[%s:%d] No Memory!",__FUNCTION__, __LINE__);
        exit(1);
    }
    
}

H264FramedLiveSource* H264FramedLiveSource::createNew(UsageEnvironment& env, char const* fileName, unsigned preferredFrameSize /*= 0*/, unsigned playTimePerFrame /*= 0*/)
{
    H264FramedLiveSource* newSource = new H264FramedLiveSource(env, fileName, preferredFrameSize, playTimePerFrame);

    return newSource;
}

H264FramedLiveSource::~H264FramedLiveSource()
{
    if(mem != NULL) {
        free(mem);
        mem = NULL;
    }
    if(fd > 0){
        fifo->close(fd);
    }
    //fclose(fp);
}


void H264FramedLiveSource::doGetNextFrame()
{
    fFrameSize = 200000;
    //��֪��Ϊʲô���༸֡һ����Ч�����һ��㣬Ҳ������������
/*
    for(int i = 0; i < 2; i++)
    {
        //Camera.GetNextFrame();
        for (my_nal = Camera.encoder->nal; my_nal < Camera.encoder->nal + Camera.n_nal; ++my_nal){
            memmove((unsigned char*)fTo + fFrameSize, my_nal->p_payload, my_nal->i_payload);
            fFrameSize += my_nal->i_payload;
        }
    }
*/

    memset(mem,0,BUFF_SIZE);
	if((real_read=read(fd, &length, 4)) > 0){
		printf("Read from fifo length: '%d'\n", length);
	}
		
    if ((real_read = read(fd, mem, length))>0) {
	    //strncpy(buf,buff,10);
        //printf("Read from fifo buf: '%s'\n",buf);
    }
        
    fFrameSize = length;
    if( fFrameSize >  fMaxSize) {
        fNumTruncatedBytes = fFrameSize - fMaxSize;  
        fFrameSize = fMaxSize;  
    }else {  
        fNumTruncatedBytes = 0;  
    }  

    if(mem != NULL) {
        memmove((unsigned char*)fTo, mem, fFrameSize);
        printf("$$$$$$$$$$$$$$$\n");
    }
    nextTask() = envir().taskScheduler().scheduleDelayedTask(1000000,
        (TaskFunc*)FramedSource::afterGetting, this);//��ʾ�ӳ�0�����ִ�� afterGetting ����
    return;
}

#if 0
void Cameras::Init()
{
    int ret;
    //�򿪵�һ������ͷ
    cap = cvCreateCameraCapture(0);
    if (!cap)
    {
        fprintf(stderr, "Can not open camera1.\n");
        exit(-1);
    }
    cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH, WIDTH);
    cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);

    encoder = (my_x264_encoder *)malloc(sizeof(my_x264_encoder));
    if (!encoder){
        printf("cannot malloc my_x264_encoder !\n");
        exit(EXIT_FAILURE);
    }
    CLEAR(*encoder);
    
    strcpy(encoder->parameter_preset, ENCODER_PRESET);
    strcpy(encoder->parameter_tune, ENCODER_TUNE);

    encoder->x264_parameter = (x264_param_t *)malloc(sizeof(x264_param_t));
    if (!encoder->x264_parameter){
        printf("malloc x264_parameter error!\n");
        exit(EXIT_FAILURE);
    }

    /*��ʼ��������*/
    CLEAR(*(encoder->x264_parameter));
    x264_param_default(encoder->x264_parameter);

    if ((ret = x264_param_default_preset(encoder->x264_parameter, encoder->parameter_preset, encoder->parameter_tune))<0){
        printf("x264_param_default_preset error!\n");
        exit(EXIT_FAILURE);
    }

    /*cpuFlags ȥ�ջ���������ʹ�ò�������֤*/
    encoder->x264_parameter->i_threads = X264_SYNC_LOOKAHEAD_AUTO;
    /*��Ƶѡ��*/
    encoder->x264_parameter->i_width = WIDTH;//Ҫ�����ͼ��Ŀ��
    encoder->x264_parameter->i_height = HEIGHT;//Ҫ�����ͼ��ĸ߶�
    encoder->x264_parameter->i_frame_total = 0;//Ҫ�������֡������֪����0
    encoder->x264_parameter->i_keyint_max = 25;
    /*������*/
    encoder->x264_parameter->i_bframe = 5;
    encoder->x264_parameter->b_open_gop = 0;
    encoder->x264_parameter->i_bframe_pyramid = 0;
    encoder->x264_parameter->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;

    /*log����������Ҫ��ӡ������Ϣʱֱ��ע�͵�*/
//    encoder->x264_parameter->i_log_level = X264_LOG_DEBUG;

    encoder->x264_parameter->i_fps_num = 25;//���ʷ���
    encoder->x264_parameter->i_fps_den = 1;//���ʷ�ĸ

    encoder->x264_parameter->b_intra_refresh = 1;
    encoder->x264_parameter->b_annexb = 1;
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    strcpy(encoder->parameter_profile, ENCODER_PROFILE);
    if ((ret = x264_param_apply_profile(encoder->x264_parameter, encoder->parameter_profile))<0){
        printf("x264_param_apply_profile error!\n");
        exit(EXIT_FAILURE);
    }
    /*�򿪱�����*/
    encoder->x264_encoder = x264_encoder_open(encoder->x264_parameter);
    encoder->colorspace = ENCODER_COLORSPACE;

    /*��ʼ��pic*/
    encoder->yuv420p_picture = (x264_picture_t *)malloc(sizeof(x264_picture_t));
    if (!encoder->yuv420p_picture){
        printf("malloc encoder->yuv420p_picture error!\n");
        exit(EXIT_FAILURE);
    }
    if ((ret = x264_picture_alloc(encoder->yuv420p_picture, encoder->colorspace, WIDTH, HEIGHT))<0){
        printf("ret=%d\n", ret);
        printf("x264_picture_alloc error!\n");
        exit(EXIT_FAILURE);
    }

    encoder->yuv420p_picture->img.i_csp = encoder->colorspace;
    encoder->yuv420p_picture->img.i_plane = 3;
    encoder->yuv420p_picture->i_type = X264_TYPE_AUTO;

    /*����YUV buffer*/
    encoder->yuv = (uint8_t *)malloc(WIDTH*HEIGHT * 3 / 2);
    if (!encoder->yuv){
        printf("malloc yuv error!\n");
        exit(EXIT_FAILURE);
    }
    CLEAR(*(encoder->yuv));
    encoder->yuv420p_picture->img.plane[0] = encoder->yuv;
    encoder->yuv420p_picture->img.plane[1] = encoder->yuv + WIDTH*HEIGHT;
    encoder->yuv420p_picture->img.plane[2] = encoder->yuv + WIDTH*HEIGHT + WIDTH*HEIGHT / 4;

    n_nal = 0;
    encoder->nal = (x264_nal_t *)calloc(2, sizeof(x264_nal_t));
    if (!encoder->nal){
        printf("malloc x264_nal_t error!\n");
        exit(EXIT_FAILURE);
    }
    CLEAR(*(encoder->nal));

    RGB1 = (unsigned char *)malloc(HEIGHT * WIDTH * 3);
    
}
void Cameras::GetNextFrame()
{
    img = cvQueryFrame(cap);

    for (int i = 0; i< HEIGHT; i++)
    {
        for (int j = 0; j< WIDTH; j++)            
        {
            RGB1[(i*WIDTH + j) * 3] = img->imageData[i * widthStep + j * 3 + 2];;
            RGB1[(i*WIDTH + j) * 3 + 1] = img->imageData[i * widthStep + j * 3 + 1];                
            RGB1[(i*WIDTH + j) * 3 + 2] = img->imageData[i * widthStep + j * 3];
        }
    }
    Convert(RGB1, encoder->yuv, WIDTH, HEIGHT);
    encoder->yuv420p_picture->i_pts++;
//printf("!!!!!\n");
    if ( x264_encoder_encode(encoder->x264_encoder, &encoder->nal, &n_nal, encoder->yuv420p_picture, &pic_out) < 0){
        printf("x264_encoder_encode error!\n");
        exit(EXIT_FAILURE);
    }
//printf("@@@@@@\n");
    /*for (my_nal = encoder->nal; my_nal < encoder->nal + n_nal; ++my_nal){
        write(fd_write, my_nal->p_payload, my_nal->i_payload);
    }*/
}
void Cameras::Destory()
{
    free(RGB1);
    cvReleaseCapture(&cap);
    free(encoder->yuv);
    free(encoder->yuv420p_picture);
    free(encoder->x264_parameter);
    x264_encoder_close(encoder->x264_encoder);
    free(encoder);
}

#endif
