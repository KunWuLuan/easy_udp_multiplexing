#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<thread>
#include<vector>
#include<stdio.h>

using namespace std;

void clientLoop(int num){
    int cliSock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in servAddr;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(64081);
    connect(cliSock, (sockaddr *)(&servAddr), sizeof(servAddr));
    for(int i=0;i<9;i++){
        char buf[25] = {0};
        sprintf(buf, "%d-%d", num, i);
        send(cliSock, buf, strlen(buf), 0);
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