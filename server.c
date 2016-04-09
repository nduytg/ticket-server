//MSSV: 1312084
//Ticket Server

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
#define SO_CHUYEN 3
#define MAXSIZE 4096

typedef struct Type
{
	int number;			//So luong loai ve con lai
	int price;			//Gia tien
	
}Type;

typedef struct Route
{
	int code;			//Ma danh dau tuyen xe
	Type l [SO_CHUYEN];
	//Type A = l[0]
	//Type B = l[1]
	//Type C = l[2]
}Route;

typedef struct Request
{
	//HCM-HN: 1
	//HCM-Hue: 2
	//HCM-Da Lat: 3
	int route;	//Type tu 1 toi 3
	int type;	//type tu 0 toi 2
	int number;
}Request;

//Bien toan cuc
Route HCM_HN, HCM_HUE, HCM_DALAT;
pthread_mutex_t myMutex;

void initInfo()
{
	//HCM-HN: 1
	//HCM-Hue: 2
	//HCM-Da Lat: 3
	HCM_HN.code = 1;
	HCM_HUE.code = 2;
	HCM_DALAT.code = 3;
	//--------------
	
	//HCM-HN
	HCM_HN.l[0].number = 50;	HCM_HN.l[0].price = 100;
	HCM_HN.l[1].number = 80;	HCM_HN.l[1].price = 80;
	HCM_HN.l[2].number = 40;	HCM_HN.l[2].price = 60;
	//--------------
	
	//HCM-HUE
	HCM_HUE.l[0].number = 35;	HCM_HUE.l[0].price = 60;
	HCM_HUE.l[1].number = 54;	HCM_HUE.l[1].price = 50;
	HCM_HUE.l[2].number = 48;	HCM_HUE.l[2].price = 40;
	//--------------
	
	//HCM_DALAT
	HCM_HN.l[0].number = 70;	HCM_HN.l[0].price = 120;
	HCM_HN.l[1].number = 50;	HCM_HN.l[1].price = 90;
	HCM_HN.l[2].number = 30;	HCM_HN.l[2].price = 70;
	//--------------
}

int createTCPserverSocket(char *host, int port, struct addrinfo hints);
int createTCPsocket();
int acceptTCPsocket(int serverSock, struct sockaddr_storage addr);
void *handle_request(void *cli_socket);

char *packInt(int a)
{
	char *buf = (char*)malloc(sizeof(int) + 1);
	buf[0] = a>>24;
	buf[1] = a>>16;
	buf[2] = a>>8;
	buf[3] = a;
	buf[4] = '\0';
	return buf;
}

int unpackInt(char *buf)
{
	return ((buf[0]<<24) | buf[1]<<16 | (buf[2]<<8) | buf[3]);
}

char *packMyStruct(Route route)
{
	char *buf = (char*)malloc(sizeof(Route) + 1);
	char *temp;
	strcpy(buf,"");
	
	temp = packInt(route.code);
	strcat(buf,temp);
	free(temp);
	
	for(int i = 0; i < SO_CHUYEN; i++)
	{
		temp = packInt(route.l[i].number);
		strcat(buf,temp);
		free(temp);
		temp = packInt(route.l[i].price);
		strcat(buf,temp);
		free(temp);
	}
	buf[sizeof(Route)] = '\0';
	return buf;
}

Route unpackMyStruct(char *buf)
{
	Route rt;
	char *tmp = (char*)malloc(sizeof(int) + 1);
	char *pt;		//con tro de luu vi tri unpack
	tmp ="";
	pt = buf;
	
	strncpy(tmp,pt,4);
	pt += 4;
	rt.code = unpackInt(tmp);
	
	for(int i = 0; i < SO_CHUYEN; i++)
	{
		strncpy(tmp,pt,4);
		pt+=4;
		rt.l[i].number = unpackInt(tmp);
		strncpy(tmp,pt,4);
		pt+=4;
		rt.l[i].price = unpackInt(tmp);
	}
	free(tmp);
}

void printRouteInfo(Route rt)
{
	printf("Route %d\n",rt.code);
	printf("Bieu gia ve\n\n");
	
	for(int i=0; i<SO_CHUYEN; i++)
	{
		if(i==0)
			printf("Ve loai A: ");
		if(i==1)
			printf("Ve loai B: ");
		if(i==2)
			printf("Ve loai C: ");
			
		printf("%d ve - Gia: %d USD\n",rt.l[i].number, rt.l[i].price);
	}
	printf("-------------------------------\n\n");
}

//Ham set cac gia tri mac dinh cho server
void initInfo();

int main()
{
	//Khoi tao cac bien toan cuc
	initInfo();
	
	//**Chay thu thread voi tham so don gian**
	/*
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
	* */
	//*********Ket thuc chay thu**************
	
	//----Chay thu pack voi unpackMystruct----
	//char *buf = (char*)malloc(sizeof(Route) + 1);
	char *buf;
	
	printRouteInfo(HCM_HN);
	buf = packMyStruct(HCM_HN);
	HCM_HN = unpackMyStruct(buf);
	printf("Gia tri sau khi pack-unpack\n\n");
	printRouteInfo(HCM_HN);
	free(buf);
	//----------------------------------------
		

	//printf("Size of int: %d\n",sizeof(int));		//Size of (int) = 4
	//printf("Size of Route: %d\n",sizeof(Route));	//Size of (Route) = 28
	//Set gia tri cho bien toan cuc
	int retcode;
	int server_socket;
	int ticket_client;
	struct addrinfo hints;
	
	printf("Server cong ty duong sat X\n");

	
	//Create and bind in this function
	server_socket = createTCPserverSocket(LOCAL_HOST,PORT,hints);
	listen(server_socket, BACKLOG);
	
	//Accept ket noi tu day
	pthread_t thread_id;
	while(1)
	{
		struct sockaddr_storage client_addr;
		ticket_client = acceptTCPsocket(server_socket, client_addr);
		
		
		//Thread -> handle_request
		if (pthread_create(&thread_id, NULL, handle_request, (void*) &ticket_client) < 0)
		{
			printf("Failed when created thread\n");
			//exit(1);
		}
		
		//Join thread
		pthread_join(thread_id, NULL);
		
	};
	
	close(server_socket);
	pthead_exit(NULL);
	exit(0);
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

void *handle_request(void *cli_socket)
{
	int socket = (int)cli_socket;
	printf("Handle_request() with socket: %d\n",socket);
	
	//B1: Send thong tin ve cho client
	char *buf;
	buf = packMyStruct(HCM_HN);
	send(socket,buf,sizeof(buf),0);
	free(buf);
	
	buf = packMyStruct(HCM_HUE);
	send(socket,buf,sizeof(buf),0);
	free(buf);
	
	buf = packMyStruct(HCM_DALAT);
	send(socket,buf,sizeof(buf),0);
	free(buf);
	
	//B2: Nhan request tu client
	buf = (char*)malloc(MAXSIZE);
	recv(socket,buf,MAXSIZE,0);
	
	//Giai ma request ra
	//B3: Tinh toan va check semaphore
	
	//B4: Tra ket qua (ok hoac ko)
	//Thread
}


/*
void *printHello(void *threadid)
{
	long tid;
	tid = (long)threadid;
	printf("Hello World! It's me, thread #%ld!\n",tid);
	pthread_exit(NULL);
}
* 
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
