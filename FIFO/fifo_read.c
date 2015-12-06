#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO "myfifo"
#define BUFF_SIZE 1024

int main() {
    char buff[BUFF_SIZE];
    int real_read;
    int fd;

//accessȷ���ļ����ļ��еķ���Ȩ�ޡ��������ĳ���ļ��Ĵ�ȡ��ʽ
//���ָ���Ĵ�ȡ��ʽ��Ч����������0������������-1
//��������FIFO���򴴽�һ��
    if(access(FIFO,F_OK)==-1){
        if((mkfifo(FIFO,0666)<0)&&(errno!=EEXIST)){
            printf("Can NOT create fifo file!\n");
            exit(1);
        }
    }

//��ֻ����ʽ��FIFO�������ļ�������fd    
    if((fd=open(FIFO,O_RDONLY))==-1){
        printf("Open fifo error!\n");
        exit(1);
    }

//����read��fdָ���FIFO�����ݣ�����buff�У�����ӡ
    while(1){
        memset(buff,0,BUFF_SIZE);
        if ((real_read=read(fd,buff,BUFF_SIZE))>0) {
            printf("Read from pipe: '%s' read size = %d.\n",buff,real_read);
        }
    }
    
    close(fd);
    exit(0);
}