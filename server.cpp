#include "server.hpp"

#include <iostream>
#include <cstring>

using namespace std;

Server::Server(int port) {
    this->port = port;
}

void Server::init() {
    if ((listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        throw "socket() error";
    cout << "socket() succeed" << endl;

    struct sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        ::close(listenSocket);
        throw "bind() error";
    }
    cout << "bind() succeed" << endl;

    if (listen(listenSocket, SOMAXCONN) == -1) {
        ::close(listenSocket);
        throw "listen() error";
    }
    cout << "listen() succeed" << endl;

    // for udp
    if (listenSocketU = socket(PF_INET, SOCK_DGRAM, IPPROTO_TCP) == -1)
        throw "udp socket() error!"
    cout << "udp socket() success"

    struct sockaddr_in toAddr = {}
	toAddr.sin_family = AF_INET;
	toAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	toAddr.sin_port = htons(port);
     


    bThreadRun = true;
    loopThread = new thread(&Server::loop, this);
}
void Server::close() {
    for (auto clientSocket : clientSocketList)
        ::close(clientSocket);
    clientSocketList.clear();

    if (listenSocket != -1)
        ::close(listenSocket);
        
    bThreadRun = false;
    if (loopThread) {
		if (loopThread->joinable())
            loopThread->join();
        delete loopThread;
        loopThread = nullptr;
    }
    
    for (auto iter = eventThreadList.begin(); iter != eventThreadList.end(); ) {
        if ((*iter)->joinable()) {
            (*iter)->join();
            iter = eventThreadList.erase(iter);
        }
    }
}

void Server::loop() {
    struct timeval timeout;
    fd_set reads;
    int fdNum;
    
    FD_ZERO(&targetReads);
    FD_SET(listenSocket, &targetReads);
    fdMax = listenSocket;

    cout << "server loop start" << endl;
    while (bThreadRun) {
        reads = targetReads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 50000;

        if((fdNum = select(fdMax+1, &reads, 0, 0, &timeout)) == -1) {
            perror("select() error");
            break;
        }
        if (fdNum == 0)
            continue;

        for (int fd = 0; fd <= fdMax; fd++) {				
            if (!FD_ISSET(fd, &reads)) continue;
            
            if (fd == listenSocket)
                accept(fd);
            else if (clientSocketList.find(fd) != clientSocketList.end())
                recv(fd);
        }
        
        // Event Thread 정리
        for (auto iter = eventThreadList.begin(); iter != eventThreadList.end(); ) {
            if (!(*iter)->joinable())
                iter = eventThreadList.erase(iter);
            else
                iter++;
        }
    }

    cout << "server loop end" << endl;
}
void Server::accept(int clientSocket) {
    struct sockaddr_in clientAddr = {};
    socklen_t clientSize = sizeof(clientAddr);

    clientSocket = ::accept(listenSocket, (struct sockaddr *)&clientAddr, &clientSize);

    FD_SET(clientSocket, &targetReads);
    if (fdMax < clientSocket)
        fdMax = clientSocket;
    
    clientSocketList.insert(clientSocket);
    if (onAccept != nullptr) {
        eventThreadList.push_back(new thread ([=]() {
            this->onAccept(clientSocket);
        }));
    }
}

void Server::recv(int clientSocket) {
    char buff[BUFF_SIZE];
    int recvLen;

    recvLen = read(clientSocket, buff, BUFF_SIZE);

    if(recvLen <= 0) {
        ::close(clientSocket);

        FD_CLR(clientSocket, &targetReads);

        clientSocketList.erase(clientSocket);
        
        if (onClose != nullptr) {
            eventThreadList.push_back(new thread ([=]() {
                this->onClose(clientSocket);
            }));
        }
    }
    else {
        if (onRecv != nullptr) {
            eventThreadList.push_back(new thread ([=]() {
                this->onRecv(clientSocket, buff, recvLen);
            }));
        }
    }
}
