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


void 	*client(void * ServAddr){
	printf("test1");
//	pthread_detach(pthread_self());
//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	struct sockaddr_in *incomming = (struct sockaddr_in *)ServAddr;
	printf("%s",incomming);
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	char sendBuffer[BUFSIZE] = "";
	connect(sockfd, (struct sockaddr *) &incomming, sizeof(incomming));
	printf("server");
	while(strncmp(sendBuffer,"quit\n",5)!=0);{
		scanf(" %s", sendBuffer);
		send(sockfd, sendBuffer, BUFSIZE, '\0');
		printf("-%s", sendBuffer);
	}
	close(sockfd);
	exit(0);
}

void	*server(void * CliAddr){
	printf("test2");
//	pthread_detach(pthread_self());
//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	struct sockaddr_in *incomming = (struct sockaddr_in *)CliAddr;
	printf("%s",incomming);
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	char recvBuffer[BUFSIZE] = "";
	ssize_t BytesRecv;
	int connfd;
	socklen_t CliLen = sizeof(incomming);
	bind(sockfd, (struct sockaddr *) &incomming, sizeof(*incomming));
	listen(sockfd, 7);
	if((connfd=accept(sockfd, (struct sockaddr *) &incomming, &CliLen)) != 0 ){
		printf("server");
		while(strncmp(recvBuffer,"quit\n",5)!=0);{
			BytesRecv = recv(sockfd, recvBuffer, BUFSIZE, '\0');
			if (recvBuffer > 0){
				printf("%s: ", recvBuffer);
			}
		}
	}
	close(sockfd);
	exit(0);
}


int main(int argc, char *argv[]) {
	struct addrinfo hints, *res;
	struct sockaddr_in ServAddr, CliAddr;
//	int errcode;	//getaddrinfo
	int result_code;//, thread_args[NUM_THREADS]; //pthread.h
//	pthread_t *threads[NUM_THREADS];
	pthread_t conn_t, listen_t;
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
	//configure connection socket to server
	memset(&ServAddr, '\0', sizeof(ServAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = AI_PASSIVE;	//AI_PASSIVE flag sets host to local, replace with htons(*argv[3]) after testing
	ServAddr.sin_port = htons(*argv[2]);

	//configure listener socket for client
	memset(&CliAddr, '\0', sizeof(CliAddr));
	CliAddr.sin_family = AF_INET;
	CliAddr.sin_addr.s_addr = AI_PASSIVE;	//AI_PASSIVE flag sets host to local
	CliAddr.sin_port = htons(*argv[1]);

	//configure threads for client then server socket
//	result_code = 
	pthread_create(&conn_t, NULL, client, (void *) &ServAddr);
//	assert(0 == result_code);
//	printf("%s",result_code);
//	result_code = 
	pthread_create(&listen_t, NULL, server, (void *) &CliAddr);
//	assert(0 == result_code);
	pthread_attr_destroy(&tattr);
//	printf("%s",result_code);
	pthread_join(conn_t,NULL);
	pthread_join(listen_t,NULL);
	return 0;
}
