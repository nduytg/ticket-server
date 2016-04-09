//MSSV: 1312084
//Ticket Client

//------------------------INCLUDE--------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
//--------------------END OF INCLUDE-----------------------


//------------------------DEFINE--------------------------
#define POOL_SIZE 10				//Thread pool size
#define LOCAL_HOST "127.0.0.1"		//Local host IP
#define PORT 60000					//Port mac dinh cho server
#define BACKLOG 10
#define SO_CHUYEN 3
#define SO_LOAI 3
#define MAXSIZE 4096

//~ #define HCM-HaNoi 1
//~ #define HCM-Hue 2
//~ #define HCM-DaLat 3
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
//----------------------END OF STRUCT----------------------


//---------------------GLOBAL VARIABLES--------------------
Route HCM_HN, HCM_HUE, HCM_DALAT;
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
//------------------------END OF GVs-----------------------

//------------------------PROTOTYPES-----------------------
//Ham set cac gia tri mac dinh cho server
void initInfo();
int createTCPserverSocket(char *host, int port, struct addrinfo hints);
int createTCPsocket();
int acceptTCPsocket(int serverSock, struct sockaddr_storage addr);
void *handle_request(void *cli_socket);

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
		//printf("Dest[%d] = %d\t",i+offset,(int)dest[i+offset]);
		//printf("Src[%d] = %d\n",i,(int)src[i]);
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
	//offset += 4;
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
	//pt += 4;
	
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
	
	for(int i=0; i<29; i++)
	{
		printf("Buf[%d] = %d\n",i,(int)buf[i]);
	}
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
	printf("Gia ve: %d\n\n",rq.number);
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
//---------------------END OF PROTOTYPES-------------------


int main()
{
	int clientSock;
	struct addrinfo hints, *p;
	
	printf("Hello, this is Ticket Client\n");
	
	if( getaddrinfo(LOCAL_HOST,PORT,&hints,&p) != 0)
	{
		printf("Failed at getaddrinfo()\n");
		//freeaddrinfo(hints);
		exit(-1);
	}
	
	
	clientSock = createTCPsocket();
	
	if(connect(clientSock,p->ai_addr, p->ai_addrlen) < 0)
	{
		printf("Cannot connect to %s - %d\n",LOCAL_HOST,PORT);
		//freeaddrinfo(hints);
		exit(-1);
	}
	
	unsigned char *buf = (unsigned char *)malloc(sizeof(Route) + 1);
	//recv 3 struct
	recv(clientSock,buf,sizeof(Route) + 1, 0);
	HCM_HN = unpackMyStruct(buf);
	recv(clientSock,buf,sizeof(Route) + 1, 0);
	HCM_HUE = unpackMyStruct(buf);
	recv(clientSock,buf,sizeof(Route) + 1, 0);
	HCM_DALAT = unpackMyStruct(buf);
	
	printRouteInfo(HCM_HN);
	printRouteInfo(HCM_HUE);
	printRouteInfo(HCM_DALAT);
	
	//Ga gam khach hang
	printf("Moi chon chuyen xe can mua\n");
	printf("1/ HCM - Ha Noi\n 2/ HCM - Hue\n 3/ HCM - Da Lat\n\n");
	printf("Moi chon (bam 1, 2 hoac 3): \n");
	int choice = 0;
	scanf("%d",choice);
	
	//send  1 request
	Request rq;
	//send(rq);
	
	//recv 1 (2) result
	//recv
	//recv
	exit(0);
}
