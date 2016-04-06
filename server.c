//MSSV: 1312084
//	Ticket Server
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types/h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <mutex.h>
#include <Ticket.h>

#define POOL_SIZE 10				//Thread pool size
#define LOCAL_HOST "127.0.0.1"		//Local host IP
#define PORT 60000				//Port mac dinh cho server
#define BACKLOG 10;

typedef struct Type
{
	int name;			//Ma danh dau chuyen xe
	int type;			//Ma loai ve
	int price;			//Gia tien
}

typedef struct Route
{
	int name;			//Ma danh dau tuyen xe
	int n_type_A;		//So ve loai A, B, C
	int n_type_B;
	int n_type_C;
}

void help();


void *do_nothing(void *null)
{
	int i;
	i = 0;
	pthread_exit(NULL);
}


void handle_request()
{
}

int createTCPserverSocket(char *host, int port)
{	
	int mySocket;
	struct addrinfo hints, *p;
	
	memset(&hint, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//hay AF_INET cung dc
	hints.ai_socktype = SOCK_STREAM;
	
	if((getaddrinfo(host, port, &hints, &p)) != 0)
	{
		printf("Failed to getaddrinfo()\n");
		return -1;
	}
	
	if( (mySocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);) == -1)
	{
		printf("Failed to create socket()\n");
		return -1;
	}	
	
	if ( (bind(server_socket, p->ai_addr, p->ai_addrlen)) < 0 )
	{
		close(mySocket);
		printf("Failed to bind socket\n");
		return -1;
	}	
	
	freeaddrinfo(hints);
	return mySocket;
}

int createTCPsocket(char *host, int port)
{	
	int mySocket;
	struct addrinfo hints, *p;
	
	memset(&hint, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//hay AF_INET cung dc
	hints.ai_socktype = SOCK_STREAM;
	
	if((getaddrinfo(host, port, &hints, &p)) != 0)
	{
		printf("Failed to getaddrinfo()\n");
		return -1;
	}
	
	if( (mySocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);) == -1)
	{
		printf("Failed to create socket()\n");
		return -1;
	}	
	
	freeaddrinfo(hints);
	return mySocket;
}



int acceptTCPsocket()
{
	
	return -1;
}


//MO HINH 2: lap song song theo phien lam viec
// + MO HINH 9: MAXTHREADS

//Tao socket
//Bind
//Listen
//while (true) => sock = accept()
// => tao tieu trinh handle request (handle_request, ss)

//Ham xuy ly 1 connection
//close socket cua tieu trinh
int main()
{
	pthread_t threads[POOL_SIZE];
	printf("Server cong ty duong sat X\n");
	
	
	
	int retcode;
	int server_socket;
	int ticket_client;
	
	//Server socket for listening all requests
	if ( (server_socket = createTCPserverSocket(LOCAL_HOST,PORT)) == -1 )
		exit(-1);
	
	listen(server_socket, BACKLOG);
	ticket_client = acceptTCPsocket
	
	
	
	exit(0);
}


void threadSample()
{
	int retcode;
	pthread_t threadID;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	for(int i = 0; i < POOL_SIZE; i++)
	{
		retcode = pthread_create(&tid, &attr, do_nothing, NULL);
		if (retcode)
		{
			printf("Error: return code from pthread_create() is: %d\n",retcode);
			exit(-1);
		}
		//else	
		
		//Wait for thread
		
		retcode = pthread_join(tid, NULL);
		
		if (retcode)
		{
			printf("Error: return code from pthread_join() is: %d\n",retcode);
			exit(-1);
		}
	}
	
	pthread_attr_destroy(&attr);
	pthread_exit(NULL);
}
