#include<sys/socket.h>

int UdpAccept(int fd, void * buf, int * recvLen, int flag, sockaddr * addr, socklen_t * l);