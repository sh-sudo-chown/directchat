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
#define BUFSIZE 4096

//	SYNTAX !!! - while testing AI_PASSIVE flags are currently in place of IP address at argv[1]
//	./chat connect_port listen_port remote_ip
//   e.g.
//	./chat '55' '89' '127.0.0.1'


void 	*client(void *ServAddr){
	pthread_detach(pthread_self());
//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	struct sockaddr_in *incomming = (struct sockaddr_in *)ServAddr;
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	char sendBuffer[BUFSIZE];
	connect(sockfd, (struct sockaddr *) &incomming, sizeof(incomming));
		do{
			scanf("%s", sendBuffer);
			send(sockfd, sendBuffer, BUFSIZE, '\0');
			printf("-%s", sendBuffer);
		}while(strcmp(sendBuffer,"quit\n"));
	close(sockfd);
	exit(0);
}

void	*server(void *CliAddr){
	pthread_detach(pthread_self());
//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	struct sockaddr_in *incomming = (struct sockaddr_in *)CliAddr;
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	char rcvBuffer[BUFSIZE];
	ssize_t BytesRecv;
	int connfd;
	socklen_t CliLen = sizeof(incomming);
	bind(sockfd, (struct sockaddr *) &incomming, sizeof(*incomming));
	listen(sockfd, 7);
	if((connfd=accept(sockfd, (struct sockaddr *) &incomming, &CliLen)) != 0 ){
		do{
			BytesRecv = recv(sockfd, rcvBuffer, BUFSIZE, '\0');
			if (rcvBuffer > 0){
				printf("%s: ", rcvBuffer);
			}
		}while(strcmp(rcvBuffer,"quit\n"));
	}
	close(sockfd);
	exit(0);
}


int main(int argc, char *argv[]) {
	struct addrinfo hints, *res;
	struct sockaddr_in ServAddr, CliAddr;
//	int errcode;	//getaddrinfo
	int result_code, thread_args[NUM_THREADS]; //pthread.h
//	pthread_t *threads[NUM_THREADS];
	pthread_t thread1, thread2;
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);

	//test host lookup
/*	memset(&hints, '\0', sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;	//AI_PASSIVE flag sets host to local, netdb.h
	if((errcode=getaddrinfo(NULL,NULL,&hints,&res)) != 0){
		perror("getaddrinfo");
		return -1;
	}
	freeaddrinfo(&hints);
*/
	//configure connection to server
	memset(&ServAddr, '\0', sizeof(ServAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = AI_PASSIVE;	//AI_PASSIVE flag sets host to local
	ServAddr.sin_port = htons(*argv[2]);

	//configure connection to client
	memset(&CliAddr, '\0', sizeof(CliAddr));
	CliAddr.sin_family = AF_INET;
	CliAddr.sin_addr.s_addr = AI_PASSIVE;	//AI_PASSIVE flag sets host to local
	CliAddr.sin_port = htons(*argv[1]);

	//configure threads for client then server socket
	result_code = pthread_create(&thread1, &tattr, client, (void *) &ServAddr);
	assert(0 == result_code);
	result_code = pthread_create(&thread2, &tattr, server, (void *) &CliAddr);
	assert(0 == result_code);
}
