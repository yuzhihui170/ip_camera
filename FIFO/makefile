CC = gcc
CFLAGS = -g 

FIFO_WRITE = fifo_write
FIFO_READ = fifo_read

OBJS = 
 
ALL:$(FIFO_WRITE) $(FIFO_READ)

$(FIFO_WRITE):fifo_write.o
	$(CC) $(CFLAGS) -o $@ $^
	
$(FIFO_READ):fifo_read.o
	$(CC) $(CFLAGS) -o $@ $^

install:
	cp fifo_write fifo_read /home/yuzhihui/work
clean:
	rm -rf *.o $(FIFO_WRITE) $(FIFO_READ)