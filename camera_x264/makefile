APP = test
CC = gcc
CFLAGS = -g
DEP_LIBS = -L.
LIBS = -lpthread -lm -ldl
#OBJS = test.o 
OBJS = main.o h264encoder.o video_capture.o
all: $(OBJS) h264encoder.h video_capture.h
	$(CC) $(CFLAGS) $(OBJS) libx264.a $(LIBS) -o $(APP)

clean:	
	rm -f *.o a.out $(APP) core *~ *.yuv *.h264

