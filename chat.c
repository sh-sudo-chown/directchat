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

#define BUFSIZE 4096
pthread_mutex_t mutex;

//	SYNTAX !!! - while testing AI_PASSIVE flags are currently in place of IP address at argv[1]
//	./chat connect_port listen_port remote_ip
//   e.g.
//	./chat 55 89 "127.0.0.1"


void 	*client(void * argv){
	puts("test1");
	pthread_mutex_lock(&mutex);
	char *args = argv; //create local array of argvs
	pthread_mutex_unlock(&mutex);
//	pthread_detach(pthread_self());
//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	struct sockaddr_in ConnAddr;
	int attempts = 1;
	int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);

	//configure listen socket
	memset(&ConnAddr, '\0', sizeof(ConnAddr));
	ConnAddr.sin_family = PF_INET;
	ConnAddr.sin_addr.s_addr = htons(args[3]);
	ConnAddr.sin_port = atoi(&args[2]);

	char sendBuffer[BUFSIZE] = "";

	//socket functions
	CONN_loop: attempts = attempts + 1;
	if(connect(sockfd, (struct sockaddr *) &ConnAddr, sizeof(ConnAddr))==0){
		puts("connected");
		while(strncmp(sendBuffer,"quit\n",5)!=0);{
			scanf(" %s", sendBuffer);
			send(sockfd, sendBuffer, BUFSIZE, '\0');
			printf("-%s", sendBuffer);
		}
	}else if(connect(sockfd, (struct sockaddr *) &ConnAddr, sizeof(ConnAddr))<0){
		printf("connection attempt %d failed, retrying\n", attempts);
		sleep(3);
		goto CONN_loop;	}
	close(sockfd);
	pthread_exit(0);
}

void	*server(void * argv){
	printf("test2");
	pthread_mutex_lock(&mutex);
	char *args = argv; //create local array of argvs
	pthread_mutex_unlock(&mutex);

//	pthread_detach(pthread_self());
//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	struct sockaddr_in ListAddr;

	//configure listen socket
	memset(&ListAddr, '\0', sizeof(ListAddr));
	ListAddr.sin_family = PF_INET;
	ListAddr.sin_addr.s_addr = htons(INADDR_ANY);
	ListAddr.sin_port = atoi(&args[1]);

	int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
	char recvBuffer[BUFSIZE] = "";
	ssize_t BytesRecv;
	socklen_t CliLen = sizeof(ListAddr);

	//socket functions
	if(bind(sockfd, (struct sockaddr *) &ListAddr, sizeof(ListAddr))<0){
		perror("ERROR: fail on bind");
		exit(0);
	}
	puts("listening");
	listen(sockfd, 1);
	for(;;){
		if(accept(sockfd, (struct sockaddr *) &ListAddr, &CliLen)>0){
			puts("Connection accepted");
			while(strncmp(recvBuffer,"quit\n",5)!=0);{
				BytesRecv = recv(sockfd, recvBuffer, BUFSIZE, '\0');
				if (recvBuffer >= 0 && recvBuffer != NULL){
					printf(" %s: ", recvBuffer);
				}
			}
		}
	}
	close(sockfd);
	pthread_exit(0);
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
	puts("test");
	//configure threads for client then server socket
	result_code = pthread_create(&conn_t, &tattr, client, (void *) &argv);
	assert(0 == result_code);
	result_code = pthread_create(&listen_t, &tattr, server, (void *) &argv);
	assert(0 == result_code);
	pthread_attr_destroy(&tattr);
	pthread_mutex_destroy(&mutex);
	pthread_join(conn_t,NULL);
	pthread_join(listen_t,NULL);
	return 0;
}
