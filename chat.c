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


void 	*client(void * ConnAddr){
	printf("test1");
//	pthread_detach(pthread_self());
//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	struct sockaddr_in *incomming = (struct sockaddr_in *)ConnAddr;
	printf("%s",incomming);
	int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
	char sendBuffer[BUFSIZE] = "";
	while(1){
		if(connect(sockfd, (struct sockaddr *) &incomming, sizeof(incomming))==0){
			printf("connected");
			while(strncmp(sendBuffer,"quit\n",5)!=0);{
				scanf(" %s", sendBuffer);
				send(sockfd, sendBuffer, BUFSIZE, '\0');
				printf("-%s", sendBuffer);
			}

		}else if(connect(sockfd, (struct sockaddr *) &incomming, sizeof(incomming))<0){
			perror("ERROR: connection failed");
		}
	}
	close(sockfd);
	exit(0);
}

void	*server(void * ListAddr){
	printf("test2");
//	pthread_detach(pthread_self());
//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	struct sockaddr_in *incomming = (struct sockaddr_in *)ListAddr;
	int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
	char recvBuffer[BUFSIZE] = "";
	ssize_t BytesRecv;
	int connfd;
	socklen_t CliLen = sizeof(incomming);
	if(bind(sockfd, (struct sockaddr *) &incomming, sizeof(*incomming))<0){
		perror("ERROR: fail on bind");
		exit(0);
	}
	listen(sockfd, 7);
	if((connfd=accept(sockfd, (struct sockaddr *) &incomming, &CliLen))>0 ){
		printf("Connection accepted");
		while(strncmp(recvBuffer,"quit\n",5)!=0);{
			BytesRecv = recv(sockfd, recvBuffer, BUFSIZE, '\0');
			if (recvBuffer >= 0 && recvBuffer != NULL){
				printf(" %s: ", recvBuffer);
			}
		}
	}
	close(sockfd);
	exit(0);
}


int main(int argc, char *argv[]) {
	struct addrinfo hints, *res;
	struct sockaddr_in ConnAddr, ListAddr;
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
	memset(&ConnAddr, '\0', sizeof(ConnAddr));
	ConnAddr.sin_family = PF_INET;
	ConnAddr.sin_addr.s_addr = htons(*argv[3]);	//AI_PASSIVE flag sets host to local
	ConnAddr.sin_port = atoi(argv[2]);

	//configure listen socket
	memset(&ListAddr, '\0', sizeof(ListAddr));
	ListAddr.sin_family = PF_INET;
	ListAddr.sin_addr.s_addr = htons(INADDR_ANY);	//AI_PASSIVE flag sets host to local
	ListAddr.sin_port = atoi(argv[1]);

	//configure threads for client then server socket
//	result_code = 
	pthread_create(&conn_t, NULL, client, (void *) &ConnAddr);
//	assert(0 == result_code);
//	printf("%s",result_code);
//	result_code = 
	pthread_create(&listen_t, NULL, server, (void *) &ConnAddr);
//	assert(0 == result_code);
	pthread_attr_destroy(&tattr);
//	printf("%s",result_code);
	pthread_join(conn_t,NULL);
	pthread_join(listen_t,NULL);
	return 0;
}
