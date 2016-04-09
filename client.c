//MSSV: 1312084
//	Ticket Client
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types/h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//#include <pthread.h>
//#include <mutex.h>
//#include <Ticket.h>

#define POOL_SIZE 10				//Thread pool size
#define LOCAL_HOST "127.0.0.1"		//Local host IP
#define PORT 60000					//Port mac dinh cho server
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
	Type l[SO_CHUYEN];
}Route;

typedef struct Request
{
	int route;
	int type;	type tu 0 toi 2
	int number;
}Request;

//Bien toan cuc
Route HCM_HN, HCM_HUE, HCM_DALAT;


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

void showInfo()
{
	//Show info bieu do tuyen xe, gia ve
}



int main()
{
	int clientSock;
	struct addrinfo hints, *p;
	
	printf("Hello, this is Ticket Client\n");
	
	if( getaddrinfo(LOCAL_HOST,PORT,&hints,&res) != 0)
	{
		printf("Failed at getaddrinfo()\n");
		freeaddrinfo(hints);
		exit(-1);
	}
	
	clientSock = createTCPsocket();
	
	if(connect(clientSock,p->ai_addr, p->ai_addrlen) < 0)
	{
		printf("Cannot connect to %s - %d\n",LOCAL_HOST,PORT);
		freeaddrinfo(hints);
		exit(-1);
	}
	
	
	
	//recv struct
	
	//send request
	
	//recv result
	exit(0);
}
