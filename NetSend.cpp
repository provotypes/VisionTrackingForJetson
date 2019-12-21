#include "NetSend.hpp"

NetSend::NetSend(int port) {
    // Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	} 

	memset(&servaddr, 0, sizeof(servaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = INADDR_ANY;
}

void NetSend::sendData() {
    char *hello = "Hello from client";
    sendto(sockfd, (const char *)hello, strlen(hello), 
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr));
}

void NetSend::sendData(double x, double y) {
    char dataA[sizeof(x)];
    char dataB[sizeof(y)];

    memcpy(dataA,&x,sizeof(x));
    memcpy(dataB,&y,sizeof(y));

    char data[16];

    for (int i = 0; i < 8; i++) {
        data[i] = dataA[i];
    }
    for (int i = 0; i < 8; i++) {
        data[i + 8] = dataB[i];
    }

    sendto(sockfd, (const char *)data, strlen(data), 
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr));
}

void NetSend::closePort() {
    close(sockfd);
}