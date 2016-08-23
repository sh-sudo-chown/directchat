#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/tcp.h>

//	SYNTAX !!!
//	./chat port ip
//   e.g.
//	./chat 55 127.0.0.1


#define BUFSIZE 4096
pthread_mutex_t mutex;
char sendBuffer[BUFSIZE], recvBuffer[BUFSIZE];

int	sock_option_parser(int socket){
	if(setsockopt(socket,SOL_SOCKET,SO_REUSEPORT,&(int){ 1 }, sizeof(int))<0)
		perror("setsockopt(SO_REUSEPORT) failed");
		//to connect to another local instance of ./chat on setsockopt SO_RESUSEPORT to 0(default)
	if(setsockopt(socket,SOL_SOCKET,SO_RCVBUF, &(int){ 4096 }, sizeof(int))<0)
		perror("setsockopt(SO_RCVBUF) failed");
	if(setsockopt(socket,SOL_SOCKET,SO_RCVLOWAT,&(int){ 1 }, sizeof(int))<0)
		perror("setsockopt(SO_RCVLOWAT) failed");
	if(setsockopt(socket,SOL_SOCKET,SO_SNDBUF, &(int){ 4096 }, sizeof(int))<0)
		perror("setsockopt(SO_SNDBUF) failed");
/*	if(setsockopt(socket,SOL_TCP,TCP_MAXSEG, &(int){ 4095 }, sizeof(int))<0)
		perror("setsockopt(MAX_SEG) failed");
	if(setsockopt(socket,SOL_TCP,TCP_NODELAY, &(int){ 1 }, sizeof(int))<0)
		perror("setsockopt(TCP_NODELAY) failed");*/
	return socket;
}



void 	*client(void *arg){
	pthread_mutex_lock(&mutex);
	char **args = (char**)arg; //create local array of argvs
	pthread_mutex_unlock(&mutex);
	struct sockaddr_in ConnAddr;
	int attempts;
	int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
	sock_option_parser(sockfd);

	//configure listen socket
	memset(&ConnAddr, '\0', sizeof(ConnAddr));
	ConnAddr.sin_family = PF_INET;
	ConnAddr.sin_addr.s_addr = inet_addr(args[2]);
	ConnAddr.sin_port = atoi(args[1]);

	//socket functions
	CONN_loop: attempts=attempts+1;
	if(connect(sockfd, (struct sockaddr *) &ConnAddr, sizeof(ConnAddr))!=0){
		printf("connection attempt %d failed, retrying\n", attempts);
		sleep(3);
		goto CONN_loop;
	}
	printf("connection to %s established\n",args[2]);
	while(strncmp(sendBuffer,"quit",4)!=0){
		scanf("%s ", sendBuffer);
		pthread_mutex_lock(&mutex);
		send(sockfd, sendBuffer, strlen(sendBuffer), 0);
		pthread_mutex_unlock(&mutex);
	}
	close(sockfd);
	printf("connect thread closed\n");
	pthread_exit(0);
}

void	*server(void *arg){
	pthread_mutex_lock(&mutex);
	char **args = (char**)arg; //create local array of argvs
	pthread_mutex_unlock(&mutex);

	struct sockaddr_in ListAddr;

	//configure listen socket
	memset(&ListAddr, '\0', sizeof(ListAddr));
	ListAddr.sin_family = PF_INET;
	ListAddr.sin_addr.s_addr = htons(INADDR_ANY);
	ListAddr.sin_port = atoi(args[1]);

	int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
	int newfd;
	sockfd = sock_option_parser(sockfd);
	ssize_t BytesRecv;
	socklen_t CliLen = sizeof(ListAddr);

	//socket functions
	if(bind(sockfd, (struct sockaddr *) &ListAddr, sizeof(ListAddr))<0){
		perror("ERROR: fail on bind");
		exit(0);
	}
	if(listen(sockfd, 1)==-1){
		perror("listen error");
	}
	printf("listening for incomming connections\n");
	for(;;){
		if(newfd = accept(sockfd, (struct sockaddr *) &ListAddr, &CliLen)>0)
			printf("Connection accepted from %s",args[2]);
		while (BytesRecv >= 0 && strncmp(recvBuffer,"quit",4)!=0){
			pthread_mutex_lock(&mutex);
			BytesRecv = recv(newfd, recvBuffer, sizeof(recvBuffer), 0);
			printf("%s\n", recvBuffer);
			pthread_mutex_unlock(&mutex);
			bzero(recvBuffer,sizeof(recvBuffer));
		}
	}
	printf("listener thread closed\n");
	close(sockfd);
	pthread_exit(NULL);
}


int	main(int argc, char *argv[]) {
//	struct addrinfo hints, *res;
	struct sockaddr_in ConnAddr, ListAddr;
	int errorcode;	//getaddrinfo
	int result_code;
	pthread_t conn_t, listen_t;
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_mutex_init(&mutex, NULL);

	//test host lookup
/*	memset(&hints, '\0', sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if((errorcode=getaddrinfo(NULL,NULL,&hints,&res)) != 0){
		printf("%s",errorcode);
		return -1;
	}
	freeaddrinfo(&hints);
*/

	//configure threads for client then server socket
	result_code = pthread_create(&conn_t, &tattr, client, (void *) argv);
	assert(0 == result_code);
	result_code = pthread_create(&listen_t, &tattr, server, (void *) argv);
	assert(0 == result_code);
	pthread_attr_destroy(&tattr);
	pthread_mutex_destroy(&mutex);
	pthread_join(conn_t,NULL);
	pthread_join(listen_t,NULL);
	printf("program exiting");
	return 0;
}
