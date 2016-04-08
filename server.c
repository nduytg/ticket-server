//MSSV: 1312084
//	Ticket Server
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
//#include <mutex.h>
//#include <Ticket.h>

#define POOL_SIZE 10				//Thread pool size
#define LOCAL_HOST "127.0.0.1"		//Local host IP
#define PORT 60000				//Port mac dinh cho server
#define BACKLOG 10

typedef struct Type
{
	int number;			//So luong loai ve con lai
	int price;			//Gia tien
	
}Type;

typedef struct Route
{
	int code;			//Ma danh dau tuyen xe
	Type A;
	Type B;
	Type C;
}Route;

void usage();


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

void *printHello(void *threadid)
{
	long tid;
	tid = (long)threadid;
	printf("Hello World! It's me, thread #%ld!\n",tid);
	pthread_exit(NULL);
}

void *handle_request(void *cli_socket)
{
	int socket = (int)cli_socket;
	printf("Handle_request() with socket: %d\n",socket);
	
	
	//Thread -> handle_request
	
	//B1: Send thong tin ve cho client
	
	//B2: Nhan request tu client
	
	//B3: Tinh toan va check semaphore
	
	//B4: Tra ket qua (ok hoac ko)
	//Thread
}

//Bien toan cuc
Route HCM_HN, HCM_HUE, HCM_DALAT;

//Ham set cac gia tri mac dinh cho server
void initInfo();

int main()
{
	//Set gia tri cho bien toan cuc
	initInfo();
	
	pthread_t threads[POOL_SIZE];
	int retcode;
	long t;
	

	
	
	//**Chay thu thread voi tham so don gian**
	for(t = 0; t < POOL_SIZE; t++)
	{
		printf("In main: creating thread: %ld\n",t);
		retcode = pthread_create(&threads[t], NULL, printHello, (void*)t);
		if(retcode)
		{
			printf("Error, return code form pthread_create() is %d\n",retcode);
			exit(-1);
		}
	}
	//pthead_exit(NULL);
	exit(1);
	//*********Ket thuc chay thu**************
	
	
	int server_socket;
	int ticket_client;
	struct addrinfo hints;
	
	printf("Server cong ty duong sat X\n");
	
	//Create and bind in this function
	server_socket = createTCPserverSocket(LOCAL_HOST,PORT,hints);
	listen(server_socket,BACKLOG);
	
	//Accept ket noi tu day
	while(1)
	{
		struct sockaddr_storage client_addr;
		ticket_client = acceptTCPsocket(server_socket, client_addr);
		
		
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

void initInfo()
{
	//HCM-HN: 1
	//HCM-Hue: 2
	//HCM-Da Lat: 3
	HCM_HN.code = 1;
	HCM_HUE.code = 2;
	HCM_DALAT = 3;
	//--------------
	
	//HCM-HN
	HCM_HN.A.number = 50;	HCM_HN.A.price = 100;
	HCM_HN.B.number = 80;	HCM_HN.B.price = 80;
	HCM_HN.C.number = 40;	HCM_HN.C.price = 60;
	//--------------
	
	//HCM-HUE
	HCM_HUE.A.number = 35;	HCM_HUE.A.price = 60;
	HCM_HUE.B.number = 54;	HCM_HUE.B.price = 50;
	HCM_HUE.C.number = 48;	HCM_HUE.C.price = 40;
	//--------------
	
	//HCM_DALAT
	HCM_HN.A.number = 70;	HCM_HN.A.price = 120;
	HCM_HN.B.number = 50;	HCM_HN.B.price = 90;
	HCM_HN.C.number = 30;	HCM_HN.C.price = 70;
	//--------------
}


int createTCPserverSocket(char *host, int port, struct addrinfo hints)
{	
	int mySocket;
	struct addrinfo *p;
	
	memset(&hints, 0, sizeof(hints));
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
	
	if ( (bind(mySocket, p->ai_addr, p->ai_addrlen)) < 0 )
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

/*
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
*/
