#pragma once
#include<sys/socket.h>
#include<arpa/inet.h>
#include<thread>
#include<map>
#include<set>
#include<list>

using namespace std;

thread bgThread;

set<int> listenFd;
list<pair<int, int>> activeList;
map<pair<int,short>, int> addr2Fd;
map<int, pair<int,short>> fd2Addr;

int breakTime = 10;

void findActiveSocket(){

}

void subThreadWorkingLoop(){

}

void MultiplexingUdpInit(){
    bgThread = thread(subThreadWorkingLoop);
}

int UdpListen(int fd){
    if(listenFd.count(fd) != 0){
        return -1;
    }
    listenFd.insert(fd);
    return 0;
}

int UdpAccept(int fd, void * buf, int * recvLen, int flag, sockaddr * addr, socklen_t * l){
    *l = sizeof(sockaddr);
    *recvLen = recvfrom(fd, buf, *recvLen, flag, addr, l);
    int udpSock = socket(AF_INET, SOCK_DGRAM, 0);
    int optVal = 1;
    if(setsockopt(udpSock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal))==-1)return -1;
    if(setsockopt(udpSock, SOL_SOCKET, SO_REUSEPORT, &optVal, sizeof(optVal))==-1)return -1;
    sockaddr addr_in;
    socklen_t l_in = sizeof(sockaddr);
    if(getsockname(fd, &addr_in, &l_in) == -1){
        return -2;
    }
    if(::bind(udpSock, &addr_in, sizeof(addr_in)) == -1){
        return -3;
    }
    if(connect(udpSock, addr, *l)==-1){
        return -4;
    }
    return udpSock;
}