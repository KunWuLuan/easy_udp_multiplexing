#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>
#include<thread>
#include<vector>
#include<stdio.h>

using namespace std;

void clientLoop(int num){
    int cliSock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(12345);
    if(connect(cliSock, (sockaddr *)(&servAddr), sizeof(servAddr)) != 0){
        printf("errno:%d, err:%s\n", errno, strerror(errno));
        return;
    }
    for(int i=0;i<9;i++){
        char buf[25] = {0};
        sprintf(buf, "%d-%d", num, i);
        int sendLen = send(cliSock, buf, strlen(buf), 0);
        if(sendLen < 0){
            printf("thread:%d msg: %s, errno:%d, err:%s\n", num, buf, errno, strerror(errno));
        }else{
            printf("thread:%d has sent msg: %s, len:%d\n", num, buf, sendLen);
        }
    }
}

int main(){
    int clientNum = 10;
    vector<thread> threadVec;
    for(int i=0;i<clientNum;i++){
        thread cliThread = thread(clientLoop, i);
        threadVec.emplace_back(move(cliThread));
    }
    for(int i=0;i<threadVec.size();i++){
        threadVec[i].join();
    }
    return 0;
}