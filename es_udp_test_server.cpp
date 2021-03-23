#include"es_udp_multiplexing.h"
#include<arpa/inet.h>
#include<sys/socket.h>
#include<errno.h>
#include<unistd.h>
#include<memory>
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
             <<inet_ntoa(addr->sin_addr)
             <<std::endl;
    std::cout<<"port: "<<ntohs(addr->sin_port)<<std::endl;
}

void handler(void * msg, int msgLen){
    std::cout<<(char*)msg<<std::endl;
    return ; 
}

void serverLoop(int fd, sockaddr cliAddr, void * msg, int msgLen){
    printAddr(&cliAddr);
    send(fd, "hello", 6, 0);
    handler(msg, msgLen);
    while(1){
        int bufSize = 4096;
        char * buf = new char[1024];
        int recvLen = recv(fd, buf, bufSize, 0);
        if(recvLen < 0){
            std::cerr<<strerror(errno)<<std::endl;
            break;
        }
        buf[recvLen] = 0;
        handler(buf, recvLen);
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
                std::thread newRequest = std::thread(serverLoop, cliSock, cliAddr, (void*)buf, bufSize);
                newRequest.detach();
                sleep(1);
            }
        }else{
            std::cout<<"errno:"<<errno<<" err:"<<strerror(errno)<<std::endl;
        }
    }
#endif
    return 0;
}