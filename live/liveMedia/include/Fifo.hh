#ifndef __FIFO_HH__
#define __FIFO_HH__

class Fifo {
public:
	void open();
	void close();
private:
	int fifo_fd = 0;
}

#endif



