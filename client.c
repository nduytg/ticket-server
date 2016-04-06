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
#include <pthread.h>
#include <mutex.h>
#include <Ticket.h>

#define POOL_SIZE 10				//Thread pool size
#define LOCAL_HOST "127.0.0.1"		//Local host IP
#define PORT 60000				//Port mac dinh cho server

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



int main()
{
	
	exit(0);
}
