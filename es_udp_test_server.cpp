#include"es_udp_multiplexing.h"
#include<arpa/inet.h>
#include<sys/socket.h>
#include<errno.h>
#include<iostream>
#include<netinet/in.h>
#include<memory>
#ifdef __linux__
#include<sys/epoll.h>
#elif __APPLE__
#include<thread>
#include<sys/select.h>
#endif

void printAddr(sockaddr * cliAddr){
    sockaddr_in * addr = (sockaddr_in *)(cliAddr);
    std::cout<<"ip: "
             <<((char *)(&addr->sin_addr))[0]
             <<((char *)(&addr->sin_addr))[1]
             <<((char *)(&addr->sin_addr))[2]
             <<((char *)(&addr->sin_addr))[3]
             <<std::endl;
    std::cout<<"port: "<<ntohs(addr->sin_port)<<std::endl;
}

void handler(void * msg, int msgLen){
    return ; 
}

void serverLoop(int fd, sockaddr cliAddr, void * msg, int msgLen){
    printAddr(&cliAddr);
    handler(msg, msgLen);
    while(1){
        int bufSize = 4096;
        std::unique_ptr<char> buf = std::make_unique<char>(bufSize);
        int recvLen = recv(fd, buf.get(), bufSize, 0);
        buf.get()[recvLen] = 0;
        handler(msg, msgLen);

        std::cout<<fd<<" "<<msg<<std::endl;
    }
}

int main() {
    int udpListenSock = socket(AF_INET, SOCK_DGRAM, 0);
    int optVal = 1;
    setsockopt(udpListenSock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
    setsockopt(udpListenSock, SOL_SOCKET, SO_REUSEPORT, &optVal, sizeof(optVal));
    sockaddr_in servAddr;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(64081);
    bind(udpListenSock, (sockaddr *)(&servAddr), sizeof(servAddr));

#ifdef __linux__

#elif __APPLE__
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(udpListenSock, &fds);
    while(1){
        int ret = select(udpListenSock+1, &fds, nullptr, nullptr, nullptr);
        if(ret > 0){
            if(FD_ISSET(udpListenSock, &fds)){
                socklen_t l;
                sockaddr cliAddr;
                int bufSize = 1024;
                std::unique_ptr<char> buf = std::make_unique<char>(bufSize);
                int cliSock = UdpAccept(udpListenSock, buf.get(), &bufSize, 0, &cliAddr, &l);
                std::thread newRequest = std::thread(serverLoop, cliSock, cliAddr, buf.get(), bufSize);
            }
        }else{
            std::cout<<"errno:"<<errno<<" err:"<<strerror(errno)<<std::endl;
        }
    }
#endif
    return 0;
}