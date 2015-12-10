#include "yuv.h"

int YUV422To420(unsigned char *yuv422, unsigned char *yuv420, int width, int height)
{        

       int ynum=width*height;
	   int i,j,k=0;
	//得到Y分量
	   for(i=0;i<ynum;i++){
		   yuv420[i]=yuv422[i*2];
	   }
	//得到U分量
	   for(i=0;i<height;i++){
		   if((i%2)!=0)continue;
		   for(j=0;j<(width/2);j++){
			   if((4*j+1)>(2*width))break;
			   yuv420[ynum+k*2*width/4+j]=yuv422[i*2*width+4*j+1];
			  		   }
		    k++;
	   }
	   k=0;
	//得到V分量
	   for(i=0;i<height;i++){
		   if((i%2)==0)continue;
		   for(j=0;j<(width/2);j++){
			   if((4*j+3)>(2*width))break;
			   yuv420[ynum+ynum/4+k*2*width/4+j]=yuv422[i*2*width+4*j+3];
			  
		   }
		    k++;
	   }
	   
       return 1;
}

