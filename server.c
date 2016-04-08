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
};

typedef struct Route
{
	int name;			//Ma danh dau tuyen xe
	int n_type_A;		//So ve loai A, B, C
	int n_type_B;
	int n_type_C;
};

void usage();

void *do_nothing(void *null)
{
	int i;
	i = 0;
	pthread_exit(NULL);
}


void handle_request(int cli_socket);

int createTCPserverSocket(char *host, int port, struct addrinfo hints);
int createTCPsocket();
int acceptTCPsocket(int serverSock, struct sockaddr_storage addr);


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
	int retcode;
	int server_socket;
	int ticket_client;
	struct addrinfo server_hints;
	
	printf("Server cong ty duong sat X\n");
	
	//Create and bind in this function
	server_socket = createTCPserverSocket(LOCAL_HOST,PORT,hints);
	listen(serverSocket,BACKLOG);
	
	//Accept ket noi tu day
	while(1)
	{
		struct sockaddr_storage client_addr;
		ticket_client = acceptTCPsocket(server_socket, sockaddr_storage);
		
		
		//Thread -> handle_request
		
		//B1: Send thong tin ve cho client
		
		//B2: Nhan request tu client
		
		//B3: Tinh toan va check semaphore
		
		//B4: Tra ket qua (ok hoac ko)
		//Thread
	};
	
	
	
	close(server_socket);
	
	exit(0);
}


int createTCPserverSocket(char *host, int port, struct addrinfo hints)
{	
	int mySocket;
	struct addrinfo *p;
	
	memset(&hint, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//hay AF_INET cung dc
	hints.ai_socktype = SOCK_STREAM;
	
	if(host == NULL)
	{
		printf("Set to default host: localhost\n");
		host = LOCAL_HOST;
	}
	
	if(port <= 1024 || port > 65535)
	{
		printf("Invalid port, set to default port: %d\n",PORT);
		port = port;
	}
		
	
	if((getaddrinfo(host, port, &hints, &p)) != 0)
	{
		printf("Failed to getaddrinfo()\n");
		return -1;
	}
	
	if( (mySocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
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
	
	//freeaddrinfo(hints);
	return mySocket;
}

int createTCPsocket()
{	
	int mySocket;
	//struct addrinfo hints;
	
	//memset(&hint, 0, sizeof(hints));
	//hints.ai_family = AF_UNSPEC;		//hay AF_INET cung dc
	//hints.ai_socktype = SOCK_STREAM;
	
	
	if( (mySocket = socket(AF_UNSPEC, SOCK_STREAM, 0)) == -1)
	{
		printf("Failed to create socket()\n");
		return -1;
	}	
	
	//freeaddrinfo(hints);
	return mySocket;
}



int acceptTCPsocket(int serverSock, struct sockaddr_storage addr)
{
	int newSock = accept(serverSock,(struct sockaddr *)&addr, sizeof(addr));
	if (newSock == -1)
	{
		printf("Failed at accept()\n");
		return -1;
	}
	return newSock;
}

void handle_request(int cli_socket);


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
