#include"es_udp_multiplexing.h"
#include<arpa/inet.h>
#include<sys/socket.h>
#include<errno.h>
#include<unistd.h>
#include<memory>
#include<iostream>
#include<netinet/in.h>
#include<memory>
#include<thread>
#include<mutex>

int main(){
    int udpListenSock = socket(AF_INET, SOCK_DGRAM, 0);
    int optVal = 1;
    setsockopt(udpListenSock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
    setsockopt(udpListenSock, SOL_SOCKET, SO_REUSEPORT, &optVal, sizeof(optVal));
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(12345);
    bind(udpListenSock, (sockaddr *)(&servAddr), sizeof(servAddr));
    servAddr.sin_port = htons(12366);
    connect(udpListenSock, (sockaddr *)(&servAddr), sizeof(servAddr));

    char buf[10];
    recv(udpListenSock, buf, 10, 0);
    return 0;
}