/*
* H264FramedLiveSource.hh
*/

#ifndef _H264FRAMEDLIVESOURCE_HH
#define _H264FRAMEDLIVESOURCE_HH

#include <FramedSource.hh>
#include <UsageEnvironment.hh>
//#include <opencv/highgui.h>

/*
extern "C"
{
#include "encoder.h"
} */


class H264FramedLiveSource : public FramedSource
{
public:
    static H264FramedLiveSource* createNew(UsageEnvironment& env, char const* fileName, unsigned preferredFrameSize = 0, unsigned playTimePerFrame = 0);
    //x264_nal_t * my_nal;

protected:
    H264FramedLiveSource(UsageEnvironment& env, char const* fileName, unsigned preferredFrameSize, unsigned playTimePerFrame); // called only by createNew()
    ~H264FramedLiveSource();

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();
    int TransportData(unsigned char* to, unsigned maxSize);
    //static int nalIndex;

protected:
    FILE *fp;

};

/*
class Cameras
{
public:
    void Init();
    void GetNextFrame();
    void Destory();
public:
    CvCapture * cap ;
    my_x264_encoder*  encoder;
    int n_nal;
    x264_picture_t pic_out;

    IplImage * img;
    unsigned char *RGB1;
};
*/

#endif