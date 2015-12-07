#include<stdio.h>

#include "Fifo.hh"
void Fifo::open() {
    //以只读方式打开FIFO，返回文件描述符fd    
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
