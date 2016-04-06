#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types/h>
#include <arpa/inet.h>


#include <pthread.h>
#include <mutex.h>

#define POOL_SIZE 10				//Thread pool size
#define LOCAL_HOST "127.0.0.1"		//Local host IP
#define PORT 18000				//Port mac dinh cho server
#define BACKLOG 10;

typedef struct LoaiVe
{
	char loai;
	int giave;
}

typedef struct ChuyenXe
{
	int machuyen;
	int soluong;
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

int createTCPsocket()
{	
	return -1;
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
	printf("Server cong ty duong sat X\n");
	
	int retcode;
	int server_socket;
	struct sockaddr_in servport;
	if ( (server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		printf("Failed at socket()\n");
		return -1;
	}
	
	servport.sin_family = AF_INET;
	servport.sin_port = htons(PORT);
	servport.sin_addr.s_addr = htonl(INADDR_ANY);
	
	
	if (bind(server_socket, (struct sockaddr *) &servport, sizeof(servport)) < 0)
	{
		printf("Failled at bind()\n");
		return -1;
	}
	
	retcode = listen(server_socket, BACKLOG);

	
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
