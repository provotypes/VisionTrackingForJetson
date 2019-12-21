#ifndef NET_SEND_H
#define NET_SEND_H

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define MAXLINE 1024

//udp broadcaster
class NetSend {
    public:
        NetSend(int port);

        void sendData();
        void sendData(double x, double y);

        void closePort();

    private:
        int sockfd;
	    struct sockaddr_in	 servaddr;
};

#undef MAXLINE

#endif