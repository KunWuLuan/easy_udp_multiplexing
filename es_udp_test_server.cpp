#include"es_udp_multiplexing.h"
#include<arpa/inet.h>
#include<sys/socket.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<memory>
#include<iostream>
#include<netinet/in.h>
#include<memory>
#include<thread>
#include<mutex>
#ifdef __linux__
#include<sys/epoll.h>
#elif __APPLE__
#include<sys/select.h>
#endif

std::mutex m;

void printAddr(sockaddr * cliAddr){
    sockaddr_in * addr = (sockaddr_in *)(cliAddr);
    std::cout<<"ip: "
             <<inet_ntoa(addr->sin_addr)
             <<std::endl;
    std::cout<<"port: "<<ntohs(addr->sin_port)<<std::endl;
}

void handler(void * msg, int msgLen){
    std::cout<<(char*)msg<<std::endl;
    return ; 
}

void serverLoop(int fd, sockaddr cliAddr, void * msg, int msgLen){
    m.lock();
    printAddr(&cliAddr);
    m.unlock();
    send(fd, "hello", 6, 0);
    m.lock();
    handler(msg, msgLen);
    m.unlock();
    while(1){
        int bufSize = 4096;
        char * buf = new char[1024];
        int recvLen = recv(fd, buf, bufSize, 0);
        if(recvLen < 0){
            std::cerr<<strerror(errno)<<std::endl;
            break;
        }
        buf[recvLen] = 0;
        m.lock();
        handler(buf, recvLen);
        m.unlock(); 
    }
}

void test_loop(){
    return;
}

int main() {
    int udpListenSock = socket(AF_INET, SOCK_DGRAM, 0);
    int optVal = 1;
    setsockopt(udpListenSock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
    setsockopt(udpListenSock, SOL_SOCKET, SO_REUSEPORT, &optVal, sizeof(optVal));
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(12345);
    int err = bind(udpListenSock, (sockaddr *)(&servAddr), sizeof(servAddr));
    if (err != 0) {
        std::cout<<"bind error:"<<errno<<std::endl;
    }

#ifdef __linux__
    #define MAX_EVENTS 10
    struct epoll_event ev, events[MAX_EVENTS];   
    int epollfd = epoll_create1(EPOLL_CLOEXEC);
    if(epollfd == -1){
        std::cerr<<"create epoll error"<<std::endl;
        return -1;
    }

    ev.events = EPOLLIN;
    ev.data.fd = udpListenSock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, udpListenSock, &ev) == -1) {
        std::cerr<<"add fd to epoll error"<<std::endl;
        return -1;
    }

    while(1){
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            std::cerr<<errno<<":"<<strerror(errno)<<std::endl;
            return -1;
        }
        socklen_t l;
        sockaddr cliAddr;
        int bufSize = 1024;
        char * buf = new char[bufSize];
        int cliSock = UdpAccept(udpListenSock, buf, &bufSize, 0, &cliAddr, &l);
        if(cliSock < 0){
            std::cout<<"err code:"<<cliSock<<std::endl<<"errno:"<<errno<<":"<<strerror(errno)<<std::endl;
            return -1;
        }
        if(cliSock == 0){
            continue;
        }
        std::thread newRequest = std::thread(serverLoop, cliSock, cliAddr, (void*)buf, bufSize);
        newRequest.detach();
    }
#elif __APPLE__
    while(1){
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(udpListenSock, &fds);
        int ret = select(udpListenSock+1, &fds, nullptr, nullptr, nullptr);
        if(ret > 0){
            if(FD_ISSET(udpListenSock, &fds)){
                socklen_t l;
                sockaddr cliAddr;
                int bufSize = 1024;
                char * buf = new char[bufSize];
                int cliSock = UdpAccept(udpListenSock, buf, &bufSize, 0, &cliAddr, &l);
                if(cliSock < 0){
                    std::cout<<"err code:"<<cliSock<<std::endl<<"errno:"<<errno<<":"<<strerror(errno)<<std::endl;
                    return -1;
                }
                if(cliSock == 0){
                    continue;
                }
                std::thread newRequest = std::thread(serverLoop, cliSock, cliAddr, (void*)buf, bufSize);
                newRequest.detach();
            }
        }else{
            std::cout<<"errno:"<<errno<<" err:"<<strerror(errno)<<std::endl;
        }
    }
#endif
    return 0;
}