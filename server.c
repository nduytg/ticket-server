//MSSV: 1312084
//Ticket Server

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
//---------------------END OF PROTOTYPES-------------------


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
	/*
	unsigned char *buf;
	printRouteInfo(HCM_HN);
	buf = packMyStruct(HCM_HN);
	HCM_HN = unpackMyStruct(buf);
	printf("\n\nGia tri sau khi pack-unpack\n\n");
	printRouteInfo(HCM_HN);
	free(buf);
	*/
	
	//Chay thu pack-unpack request
	/*
	unsigned char *buf;
	Request rq;
	rq.route = 1;
	rq.type = 1;
	rq.number = 90;
	printRequest(rq);
	buf = packMyRequest(rq);
	
	Request nrq = unpackMyRequest(buf);
	printRequest(nrq);
	free(buf);
	exit(0);
	*/
	//----------------------------------------
		
	int retcode;
	int server_socket;
	int ticket_client;
	struct addrinfo hints;
	printf("Server cong ty duong sat X\n");

	//Create and bind in this function
	server_socket = createTCPserverSocket(LOCAL_HOST,PORT,hints);
	printf("Track 1\n");
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
	pthread_mutex_destroy(&myMutex);
	pthead_exit(NULL);
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
	HCM_HN.l[0].number = 70;	HCM_HN.l[0].price = 120;
	HCM_HN.l[1].number = 50;	HCM_HN.l[1].price = 90;
	HCM_HN.l[2].number = 30;	HCM_HN.l[2].price = 70;
	//--------------
}

int createTCPserverSocket(char *host, int port, struct addrinfo hints)
{	
	int retcode;
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
		
	//char buffer[20];
	//itoa(port,buffer,10);
	if((retcode = getaddrinfo(host, port, &hints, &p)) != 0)
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
	unsigned char *buf;
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
	buf = (unsigned char*)malloc(MAXSIZE);
	recv(socket,buf,MAXSIZE,0);
	
	//Giai ma request ra
	Request rq = unpackMyRequest(buf);
	free(buf);
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
	if(cost > 0)
	{
		send(socket,&remain,sizeof(remain),0);
	}	

	close(socket);
	pthread_exit(NULL);
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
