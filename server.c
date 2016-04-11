//MSSV: 1312084
//Ticket Server

//------------------------INCLUDE--------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
//--------------------END OF INCLUDE-----------------------


//------------------------DEFINE--------------------------
#define POOL_SIZE 10				//Thread pool size
#define LOCAL_HOST "127.0.0.1"		//Local host IP
#define PORT "16000"				//Port mac dinh cho server
#define BACKLOG 10
#define SO_CHUYEN 3
#define SO_LOAI 3
#define MAXSIZE 4096

//--------------------END OF DEFINE------------------------


//------------------------STRUCT--------------------------
typedef struct Type
{
	int number;			//So luong loai ve con lai
	int price;			//Gia tien
	
}Type;

typedef struct Route
{
	int code;			//Ma danh dau tuyen xe
	Type l [SO_LOAI];
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
//----------------------END OF STRUCT----------------------


//---------------------GLOBAL VARIABLES--------------------
Route HCM_HN, HCM_HUE, HCM_DALAT;
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
//------------------------END OF GVs-----------------------


//------------------------PROTOTYPES-----------------------
//Ham set cac gia tri mac dinh cho server
void initInfo();

//int createTCPserverSocket(char *host, char* port, struct addrinfo hints);
int createTCPsocket();
int acceptTCPsocket(int serverSock, struct sockaddr_storage addr);
void *handle_request(void *cli_socket);

//Ham memcpy tu che ^^
void cp(char *dest, char *src, int offset, int len);

//Cac ham dong goi du lieu de goi qua Socket
unsigned char *packInt(int a);
int unpackInt(unsigned char *buf);
unsigned char *packMyRequest(Request rq);
Request unpackMyRequest(unsigned char *buf);
unsigned char *packMyStruct(Route route);
Route unpackMyStruct(unsigned char *buf);
void printRouteInfo(Route rt);
void printRequest(Request rq);
//---------------------END OF PROTOTYPES-------------------


int main()
{
	//Khoi tao gia tri cac bien toan cuc
	initInfo();
	printf("Server cong ty duong sat ABC\n");
		
	int retcode = 0;
	int server_socket;
	int ticket_client;
	struct addrinfo hints, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if( (retcode = getaddrinfo(NULL, PORT, &hints, &p)) != 0 )
	{
		fprintf(stderr, "Failed at getaddrinfo(): %s\n", gai_strerror(retcode));
		exit(-1);
	}
	
	server_socket = socket(p->ai_family,p->ai_socktype, p->ai_protocol);
	
	if ( server_socket == -1 )
	{
		fprintf(stderr, "Failed at socket(): %s\n", gai_strerror(retcode));
		exit(-1);
	}
	
	int yes = 1;
	if( (retcode = setsockopt(server_socket,SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1 ))
	{
		fprintf(stderr, "Failed at socket(): %s\n", gai_strerror(retcode));
		close(server_socket);
		exit(-1);
	}
	
	if ( (retcode = bind(server_socket,p->ai_addr, p->ai_addrlen)) == -1)
	{
		fprintf(stderr, "Failed at bind(): %s\n", gai_strerror(errno));
		close(server_socket);
		exit(-1);
	}
	
	listen(server_socket, BACKLOG);
	
	//Accept ket noi tu day
	pthread_t thread_id;
	int count = 0;
	while(1)
	{
		count++;
		struct sockaddr_storage client_addr;
		socklen_t len;
		ticket_client = accept(server_socket, (struct sockaddr*)&client_addr, &len);
		
		
		if(ticket_client < 0)
		{
			printf("Failed at accept()\n");
			exit(-1);
			
		}
		printf("Client #%d connected\n",count);
		
		printf("Ticket client socket: %d\n",ticket_client);
		//Thread -> handle_request
		if (pthread_create(&thread_id, NULL, handle_request, (void*) ticket_client) < 0)
		{
			printf("Failed when created thread\n");
			exit(1);
		}
		//Join thread
		//pthread_join(thread_id, NULL);
	};
	
	close(server_socket);
	//pthread_mutex_destroy(&myMutex);
	//pthead_exit(NULL);
	exit(0);
}
//-------------------Het ham main--------------------------------

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
	HCM_DALAT.l[0].number = 70; HCM_DALAT.l[0].price = 120;
	HCM_DALAT.l[1].number = 50;	HCM_DALAT.l[1].price = 90;
	HCM_DALAT.l[2].number = 30;	HCM_DALAT.l[2].price = 70;
	//--------------
}

int createTCPsocket()
{	
	int mySocket;
	//struct addrinfo hints;
	
	//memset(&hint, 0, sizeof(hints));
	//hints.ai_family = AF_UNSPEC;		//hay AF_INET cung dc
	//hints.ai_socktype = SOCK_STREAM;
	
	if( (mySocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Failed to create socket()\n");
		return -1;
	}	
	
	//freeaddrinfo(hints);
	return mySocket;
}

int acceptTCPsocket(int serverSock, struct sockaddr_storage addr)
{
	socklen_t addr_size = sizeof(addr);
	int newSock = accept(serverSock,(struct sockaddr *)&addr, &addr_size);
	if (newSock < 0)
	{
		printf("Failed at accept()\n");
		return -1;
	}
	return newSock;
}

void *handle_request(void *cli_socket)
{
	int retcode = 0;
	int socket = (int)cli_socket;
	printf("Handle_request() with socket: %d\n",socket);
	
	//B1: Send thong tin ve cho client
	//Send 3 struct da duoc dong goi san!!
	unsigned char buf, *buf1, *buf2, *buf3;
	
	retcode = send(socket, &HCM_HN, sizeof(Route), 0);
	printf("Sent: %d\n",retcode);

	retcode = send(socket, &HCM_HUE, sizeof(Route), 0);
	printf("Sent: %d\n",retcode);
	
	retcode = send(socket, &HCM_DALAT, sizeof(Route), 0);
	printf("Sent: %d\n",retcode);

	//B2: Nhan request tu client
	Request rq;
	recv(socket, &rq,sizeof(rq),0);

	int cost = 0, remain = 0;
	
	//B3: Tinh toan va check semaphore
	switch (rq.route)
	{
		case 1:
		
			printf("\nClient requests HCM-HN\n");
			for(int i=0; i<SO_LOAI; i++)
			{
				if(rq.type == i)
				{
					if(rq.number <= HCM_HN.l[i].number)
					{
						pthread_mutex_lock(&myMutex);
						HCM_HN.l[i].number -= rq.number;
						pthread_mutex_unlock(&myMutex);
						cost = rq.number * HCM_HN.l[i].price;
					}
					else
						remain = HCM_HN.l[i].number;
				}
			}
			break;
			
		case 2:
		
			printf("\nClient requests HCM-HUE\n");
			for(int i=0; i<SO_LOAI; i++)
			{
				if(rq.type == i)
				{
					if(rq.number <= HCM_HUE.l[i].number)
					{
						pthread_mutex_lock(&myMutex);
						HCM_HUE.l[i].number -= rq.number;
						pthread_mutex_unlock(&myMutex);
						cost = rq.number * HCM_HUE.l[i].price;
					}
					else
						remain = HCM_HUE.l[i].number;
				}
			}
			break;
			
		case 3:
		
			printf("\nClient requests HCM-DALAT\n");
			for(int i=0; i<SO_LOAI; i++)
			{
				if(rq.type == i)
				{
					if(rq.number <= HCM_DALAT.l[i].number)
					{
						pthread_mutex_lock(&myMutex);
						HCM_DALAT.l[i].number -= rq.number;
						pthread_mutex_unlock(&myMutex);
						cost = rq.number * HCM_DALAT.l[i].price;
					}
					else
						remain = HCM_DALAT.l[i].number;
				}
			}
			break;
		default:
			printf("Request is invalid\n");
			printf("Plz try again\n");
	}
	printRequest(rq);
	
	if(cost != 0)
	{
		printf("Client da mua thanh cong: %d ve\n",rq.number);
		printf("Gia tien: %d USD\n",cost);	
	}
	else
	{
		printf("Het ve!! :'(\n");
		printf("Bao lai cho client biet la het ve\n");
		//cost = -1;		//Bat flag len
	}

	//B4: Tra ket qua (ok hoac ko)
	send(socket,&cost,sizeof(cost),0);
	if(cost <= 0)
	{
		send(socket,&remain,sizeof(remain),0);
	}	

	close(socket);
	printf("Closed thread - Flag 1!\n");
	pthread_exit(NULL);
	printf("Closed thread - Flag 2!\n");
}

unsigned char *packInt(int a)
{
	char *buf = (char*)malloc(sizeof(int) + 1);
	buf[0] = a>>24;
	buf[1] = a>>16;
	buf[2] = a>>8;
	buf[3] = a;
	buf[4] = '\0';
	return buf;
}

int unpackInt(unsigned char *buf)
{
	return ((buf[0]<<24) | buf[1]<<16 | (buf[2]<<8) | buf[3]);
}

//Ham memcpy tu che ^^
void cp(char *dest, char *src, int offset, int len)
{
	for(int i=0; i<len; i++)
	{
		dest[i+offset] = src[i];
	}
}

unsigned char *packMyRequest(Request rq)
{
	unsigned char *buf = (unsigned char*)malloc(sizeof(Request) + 1);
	unsigned char *temp;
	int offset = 0;
	
	temp = packInt(rq.route);
	cp(buf,temp,offset,4);
	offset += 4;
	free(temp);
	
	temp = packInt(rq.type);
	cp(buf,temp,offset,4);
	offset += 4;
	free(temp);
	
	temp = packInt(rq.number);
	cp(buf,temp,offset,4);
	free(temp);
	return buf;
}

Request unpackMyRequest(unsigned char *buf)
{
	Request rq;
	unsigned char *tmp = (unsigned char*)malloc(sizeof(int) + 1);
	unsigned char *pt;		//con tro de luu vi tri unpack
	pt = buf;
	
	cp(tmp,pt,0,4);
	rq.route = unpackInt(tmp);
	pt += 4;
	
	cp(tmp,pt,0,4);
	rq.type = unpackInt(tmp);
	pt += 4;
	
	cp(tmp,pt,0,4);
	rq.number = unpackInt(tmp);
	
	free(tmp);
	return rq;
}

unsigned char *packMyStruct(Route route)
{
	unsigned char *buf = (unsigned char*)malloc(sizeof(Route) + 1);
	unsigned char *temp;
	int offset = 0;
	
	temp = packInt(route.code);
	cp(buf,temp,offset,4);
	offset += 4;
	free(temp);
	
	for(int i = 0; i < SO_LOAI; i++)
	{
		temp = packInt(route.l[i].number);
		cp(buf,temp,offset, 4);
		offset+=4;
		free(temp);

		temp = packInt(route.l[i].price);
		cp(buf,temp,offset, 4);
		offset+=4;
		free(temp);
	}
	
	buf[sizeof(Route)] = '\0';
	
	return buf;
}

Route unpackMyStruct(unsigned char *buf)
{
	Route rt;
	unsigned char *tmp = (unsigned char*)malloc(sizeof(int) + 1);
	unsigned char *pt;		//con tro de luu vi tri unpack
	pt = buf;
	
	cp(tmp,pt,0,4);
	pt += 4;
	rt.code = unpackInt(tmp);
	//printf("Rt.code = %d\n",rt.code);
	
	for(int i = 0; i < SO_LOAI; i++)
	{
		cp(tmp,pt,0,4);
		pt+=4;
		rt.l[i].number = unpackInt(tmp);
		
		cp(tmp,pt,0,4);
		pt+=4;
		rt.l[i].price = unpackInt(tmp);
	}
	free(tmp);
	return rt;
}

void printRouteInfo(Route rt)
{
	if(rt.code == 1)
		printf("Tuyen HCM - Ha Noi\n");
	if(rt.code == 2)
		printf("Tuyen HCM - Hue\n");
	if(rt.code == 3)
		printf("Tuyen HCM - Da Lat\n");
	//printf("Route %d\n",rt.code);
	printf("Bieu gia ve\n");
	
	for(int i=0; i<SO_LOAI; i++)
	{
		if(i==0)
			printf("Ve loai A: ");
		if(i==1)
			printf("Ve loai B: ");
		if(i==2)
			printf("Ve loai C: ");
			
		printf("%d ve - Gia: %d USD\n",rt.l[i].number, rt.l[i].price);
	}
	printf("----------------------------------\n\n");
}

void printRequest(Request rq)
{
	printf("\n----REQUEST----\n");
	printf("Loai chuyen: %d\n",rq.route);
	printf("Loai ve: %d\n",rq.type);
	printf("Luong ve: %d\n\n",rq.number);
}

