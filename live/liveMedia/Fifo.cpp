#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Fifo.hh"

void Fifo::open_fd() {
    //��ֻ����ʽ��FIFO�������ļ�������fd    
    if((fifo_fd=open("/tmp/cam_fifo",O_RDONLY))==-1){
        printf("Open fifo error!\n");
    }
}

void Fifo::close_fd() {
    if(fifo_fd > 0) {
        close(fifo_fd);
    }
}

void Fifo::close_fd(int fd) {
    if(fd > 0) {
        close(fd);
    }
}
