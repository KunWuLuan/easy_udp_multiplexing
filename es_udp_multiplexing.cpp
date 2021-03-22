#pragma once
#include<sys/socket.h>

int UdpAccept(int fd, void * buf, int * recvLen, int flag, sockaddr * addr, socklen_t * l){
    *recvLen = recvfrom(fd, buf, *recvLen, flag, addr, l);
    int udpSock = socket(AF_INET, SOCK_DGRAM, 0);
    int optVal = 1;
    setsockopt(udpSock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
    setsockopt(udpSock, SOL_SOCKET, SO_REUSEPORT, &optVal, sizeof(optVal));
    connect(udpSock, addr, *l);
    return udpSock;
}