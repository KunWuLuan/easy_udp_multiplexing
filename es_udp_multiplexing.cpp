#pragma once
#include<sys/socket.h>
#include<arpa/inet.h>
#include<map>
#include<set>
#include<unistd.h>

using namespace std;

set<int> listenFd;
map<pair<int, short>, int> addr2Fd;
map<int, sockaddr> fd2Addr;

void MultiplexingUdpInit(){
}

int UdpListen(int fd){
    if(listenFd.count(fd) != 0){
        return -1;
    }
    listenFd.insert(fd);
    return 0;
}

int UdpAccept(int fd, void * buf, int * recvLen, int flag, sockaddr * addr, socklen_t * l){
    if(listenFd.count(fd) == 0){
        return -5;
    }

    if(buf == nullptr || recvLen == nullptr || addr == nullptr || l == nullptr){
        return -1;
    }

    *l = sizeof(sockaddr);
    *recvLen = recvfrom(fd, buf, *recvLen, flag, addr, l);
    
    pair<int, short> mpKey = pair<int, short>(int(((sockaddr_in*)addr)->sin_addr.s_addr), short(((sockaddr_in*)addr)->sin_port));
    if(addr2Fd.count(mpKey) == 1){
        send(addr2Fd[mpKey], buf, *recvLen, 0);
        return 0;
    }
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

    addr2Fd[mpKey] = udpSock;
    fd2Addr[udpSock] = *addr;
    return udpSock;
}

void UdpClose(int fd){
    if(listenFd.count(fd) == 1){
        listenFd.erase(fd);
        close(fd);
        return ;
    }

    sockaddr addr = fd2Addr[fd];
    pair<int, short> mpKey = pair<int, short>(int(((sockaddr_in*)&addr)->sin_addr.s_addr), short(((sockaddr_in*)&addr)->sin_port));
    fd2Addr.erase(fd);
    addr2Fd.erase(mpKey);
    close(fd);
    return ;
}