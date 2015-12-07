#include<stdio.h>

#include "Fifo.hh"
void Fifo::open() {
    //��ֻ����ʽ��FIFO�������ļ�������fd    
    if((fifo_fd=open("/tmp/cam_fifo",O_RDONLY))==-1){
        printf("Open fifo error!\n");
        exit(1);
    }
}

void Fifo::close() {
    if(fifo_fd > 0) {
        close(fifo_fd);
    }
}
