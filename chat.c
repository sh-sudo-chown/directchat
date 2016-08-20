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

#define BUFSIZE 4096

//	SYNTAX !!! - while testing AI_PASSIVE flags are currently in place of IP address at argv[1]
//	./chat listen_port connect_port remote_ip
//   e.g.
//	./chat 55 89 "127.0.0.1"


void 	*client(void * ins){
	printf("test1");
	char *args = ins;
//	pthread_detach(pthread_self());
//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	struct sockaddr_in ConnAddr;
	int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);

	memset(&ConnAddr, '\0', sizeof(ConnAddr));
	ConnAddr.sin_family = PF_INET;
	ConnAddr.sin_addr.s_addr = htons(args[3]);
	ConnAddr.sin_port = atoi(&args[2]);

	char sendBuffer[BUFSIZE] = "";
	if(connect(sockfd, (struct sockaddr *) &ConnAddr, sizeof(ConnAddr))==0){
		printf("connected");
		while(strncmp(sendBuffer,"quit\n",5)!=0);{
			scanf(" %s", sendBuffer);
			send(sockfd, sendBuffer, BUFSIZE, '\0');
			printf("-%s", sendBuffer);
		}
	}else if(connect(sockfd, (struct sockaddr *) &ConnAddr, sizeof(ConnAddr))<0){
		perror("ERROR: connection failed");
	}
	close(sockfd);
	pthread_exit(0);
}

void	*server(void * ins){
	printf("test2");
//	pthread_detach(pthread_self());
//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	char *args = ins;
	struct sockaddr_in ListAddr;

	//configure listen socket
	memset(&ListAddr, '\0', sizeof(ListAddr));
	ListAddr.sin_family = PF_INET;
	ListAddr.sin_addr.s_addr = htons(INADDR_ANY);
	ListAddr.sin_port = atoi(&args[1]);

	int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
	char recvBuffer[BUFSIZE] = "";
	ssize_t BytesRecv;
	int connfd;
	socklen_t CliLen = sizeof(ListAddr);
	if(bind(sockfd, (struct sockaddr *) &ListAddr, sizeof(ListAddr))<0){
		perror("ERROR: fail on bind");
		exit(0);
	}
	listen(sockfd, 7);
	if((connfd=accept(sockfd, (struct sockaddr *) &ListAddr, &CliLen))>0 ){
		printf("Connection accepted");
		while(strncmp(recvBuffer,"quit\n",5)!=0);{
			BytesRecv = recv(sockfd, recvBuffer, BUFSIZE, '\0');
			if (recvBuffer >= 0 && recvBuffer != NULL){
				printf(" %s: ", recvBuffer);
			}
		}
	}
	close(sockfd);
	pthread_exit(0);
}


int main(int argc, char *argv[]) {
	struct addrinfo hints, *res;
	struct sockaddr_in ConnAddr, ListAddr;
//	int errcode;	//getaddrinfo
	int result_code;
	int i;
	pthread_t conn_t, listen_t;
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	char ins[argc];
	int args_length = 0;
	for(i=1;i=argc;i=i+1){
		args_length=args_length+sizeof(*argv[i]);
	}

	i=1;
	for(i=1;i=argc;i=i+1){	
		ins[i]=*argv[i];
	}
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
	printf("test");
	//configure threads for client then server socket
//	result_code = 
	pthread_create(&conn_t, NULL, client, (void *) &ins);
//	assert(0 == result_code);
//	printf("%s",result_code);
//	result_code = 
	pthread_create(&listen_t, NULL, server, (void *) &ins);
//	assert(0 == result_code);
	pthread_attr_destroy(&tattr);
//	printf("%s",result_code);
	pthread_join(conn_t,NULL);
	pthread_join(listen_t,NULL);
	return 0;
}
