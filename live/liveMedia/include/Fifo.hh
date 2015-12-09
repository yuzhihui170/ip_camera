#ifndef __FIFO_HH__
#define __FIFO_HH__

class Fifo {
public:
	void open_fd();
	void close_fd();
	void close_fd(int fd);
private:
	int fifo_fd;
};

#endif



