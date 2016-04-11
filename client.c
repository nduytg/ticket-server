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
void printRouteInfo(Route rt);
void printRequest(Request rq);
int createTCPsocket();
int acceptTCPsocket(int serverSock, struct sockaddr_storage addr);

unsigned char *packInt(int a);
int unpackInt(unsigned char *buf);
unsigned char *packMyRequest(Request rq);
Request unpackMyRequest(unsigned char *buf);
unsigned char *packMyStruct(Route route);
Route unpackMyStruct(unsigned char *buf);
//---------------------END OF PROTOTYPES-------------------


int main()
{
	printf("Hello, this is Ticket Client\n");
	
	int clientSock;
	int retcode;
	struct addrinfo hints, *p;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;		//hay AF_INET cung dc
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if( (retcode = getaddrinfo(NULL,PORT,&hints,&p)) != 0)
	{
		fprintf(stderr,"Failed at getaddrinfo(): %s\n", gai_strerror(retcode));
		exit(-1);
	}
	
	clientSock = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
	if(clientSock == -1)
	{
		fprintf(stderr,"Failed at socket(): %s\n",gai_strerror(errno));
	}
	
	if(connect(clientSock,p->ai_addr, p->ai_addrlen) < 0)
	{
		printf("Cannot connect to %s - %s\n",LOCAL_HOST,PORT);
		exit(-1);
	}
	printf("Connected to server!\n\n");
	
	//recv 3 struct
	Route tmpRoute;
	retcode = recv(clientSock,&tmpRoute,sizeof(Route), 0);
	printf("Sizeof (recv): %d\n",retcode);
	HCM_HN = tmpRoute;
	
	retcode = recv(clientSock,&tmpRoute,sizeof(Route), 0);
	printf("Sizeof (recv): %d\n",retcode);
	HCM_HUE = tmpRoute;
	
	retcode = recv(clientSock,&tmpRoute,sizeof(Route), 0);
	printf("Sizeof (recv): %d\n",retcode);
	HCM_DALAT = tmpRoute;
	
	printRouteInfo(HCM_HN);
	printRouteInfo(HCM_HUE);
	printRouteInfo(HCM_DALAT);
	
	//Ga gam khach hang
	printf("Moi chon chuyen xe can mua\n");
	printf("1/ HCM - Ha Noi\n2/ HCM - Hue\n3/ HCM - Da Lat\n\n");
	printf("Moi chon (bam 1, 2 hoac 3): ");
	Request rq;
	scanf("%d",&rq.route);
	
	printf("Moi chon loai ve muon mua (0, 1 hoac 2): ");
	scanf("%d",&rq.type);
	
	printf("Moi chon so luong ve muon mua: ");
	scanf("%d",&rq.number);
	
	if(rq.number == 0)
	{
		printf("Luong ve dat mua khong hop le!!\n");
		exit(0);
	}
	
	//send 1 request
	send(clientSock,&rq,sizeof(rq),0);
	
	int cost, remain;
	recv(clientSock,&cost,sizeof(int),0);
	if(cost <= 0)
	{
		recv(clientSock,&remain,sizeof(int),0);
		printf("So luong ve muon mua vuot qua gioi han\n");
		printf("So luong ve con lai la: %d\n",remain);
	}
	else
	{
		printf("----Hoa don cua ban----\n");
		printRequest(rq);
		printf("Tong tien: %d\n",cost);
	}

	printf("Hen gap lai quy khach ^^\n");
	exit(0);
}
//-----------------------Het ham main()------------------------

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





