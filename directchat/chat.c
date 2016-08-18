#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <assert.h>
#include <netdb.h>

#define NUM_THREADS 3
#define BUFSIZE 128

//	SYNTAX !!! - while testing AI_PASSIVE flags are currently in place of IP address at argv[1]
//	./chat remote_ip connect_port listen_port
//   e.g.
//	./chat 127.0.0.1 55 89


void 	*client(void* ServAddr){
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	char sendBuffer[BUFSIZE];
	while(sendBuffer != "quit"){
		if(connect(sockfd, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) != 0){
			for(;;){
				scanf("%s", sendBuffer);
				send(sockfd, sendBuffer, BUFSIZE, '\0');
				printf("-%s", sendBuffer);
			}
		}
	}
	close(sockfd);
	exit(0);
}

void	*server(void* CliAddr){
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	char rcvBuffer[BUFSIZE];
	ssize_t BytesRecv;
	int connfd;
	socklen_t CliLen = sizeof(CliAddr);
	bind(sockfd, (struct sockaddr *) &CliAddr, sizeof(*CliAddr));
	listen(sockfd, 7);
	if((connfd=accept(sockfd, (struct sockaddr *) &CliAddr, &CliLen)) != 0 ){
		for (;;){
			BytesRecv = recv(sockfd, rcvBuffer, BUFSIZE, '\0');
			if (rcvBuffer > 0){
				printf("%s: ", rcvBuffer);
			}
		}
	}
	close(sockfd);
	exit(0);
}


int main(int argc, char *argv[]) {
	struct addrinfo hints, *res;
	struct sockaddr_in ServAddr, CliAddr;
	int errcode;	//getaddrinfo
	int result_code, thread_args[NUM_THREADS]; //pthread.h
	pthread_t *threads[NUM_THREADS];

	//test host lookup
	memset(&hints, '\0', sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;	//AI_PASSIVE flag sets host to local, netdb.h
	if((errcode=getaddrinfo(NULL,NULL,&hints,&res)) != 0){
		perror("getaddrinfo");
		return -1;
	}
	freeaddrinfo(&hints);

	//configure connection to server
	memset(&ServAddr, '\0', sizeof(ServAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = AI_PASSIVE;	//AI_PASSIVE flag sets host to local
	ServAddr.sin_port = htons(*argv[2]);
		//add test function for ServAddr

	//configure connection to client
	memset(&CliAddr, '\0', sizeof(CliAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = AI_PASSIVE;	//AI_PASSIVE flag sets host to local
	ServAddr.sin_port = htons(*argv[2]);
	
	//configure threads for client then server socket
	result_code = pthread_create(&threads[0], NULL, server, (void *) &ServAddr);
	assert(0 == result_code);
	
	result_code = pthread_create(&threads[1], NULL, client, (void *) &CliAddr);
	assert(0 == result_code);
}
