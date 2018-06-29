#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_LISTEN_NUMBER  2 //监听数
#define SOCKET_CONNECT_NUMBER 5 //socket 连接数
#define BUFFER_SIZE 256

pthread_mutex_t mutex;
int connect_count; //连接数达到上限
int client_socket_array[SOCKET_CONNECT_NUMBER];

static void *ChatRoom(void *arg);


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage %s <port>/n", argv[0]);
	    exit(1);	
	}

    struct sockaddr_in client_addr;
    int client_address_len;
    pthread_t thread_id;
    int result;
	
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("create socket failed\n");
        exit(1);    
    }
    
    struct sockaddr_in server_addr;
    memset((void*)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));
    if (bind(server_socket, (struct  sockaddr*)&server_addr,sizeof(server_addr)) == -1) {
        printf("bind socket failed\n");
        exit(1);
    }
    
    if (listen(server_socket,MAX_LISTEN_NUMBER) == -1) {
        printf("listen socket failed\n");
        exit(1);
    }

    printf("服务已经启动\n");

    connect_count = 0;
    pthread_mutex_init(&mutex, NULL);
    
    while(1) {
        client_address_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr,&client_address_len);
        if (client_socket == -1) {
            printf("client connect failed\n");
            continue;
        }
	
	    if (connect_count >= SOCKET_CONNECT_NUMBER) {
	         //连接数达到上限
             printf("client connect limit\n");
             close(client_socket);
             continue;
	    }
        
        pthread_mutex_lock(&mutex);
        client_socket_array[connect_count++] = client_socket;
        pthread_mutex_unlock(&mutex);
        
        result =  pthread_create(&thread_id, NULL, &ChatRoom, (void*)&client_socket);
        if (result != 0) {
             close(client_socket);
             continue;
        }
        pthread_detach(thread_id);        
        printf("Connect client IP: %s\n", inet_ntoa(client_addr.sin_addr));
   }

   pthread_mutex_destroy(&mutex);

   close(server_socket);
   exit(0);
}

static void *ChatRoom(void *arg) {
   int user_socket = *((int*)arg);
   char msg[BUFFER_SIZE];
   ssize_t read_byte;

   while (1) {
       read_byte = read(user_socket, msg, BUFFER_SIZE);
       if (read_byte <= 0) {
            printf("%d退出聊天\n",user_socket);
            break;
       }
       msg[read_byte] = '\0';
       if (strcmp(msg, "q\n") == 0) {
            printf("%d退出聊天\n",user_socket);
            break;
       }

       pthread_mutex_lock(&mutex);
       //广播
       printf("目前的聊天人数:%d\n", connect_count);
       for (int i = 0; i < connect_count; ++i) {
            write(client_socket_array[i], msg, read_byte);
       }
       pthread_mutex_unlock(&mutex);
       
   }

   printf("%d client socket close\n", user_socket);
   close(user_socket);
   //移除socket
   pthread_mutex_lock(&mutex);
   for (int i = 0; i < connect_count; ++i) {
        if (user_socket == client_socket_array[i]) {
            for (int j = i; j < connect_count; j++) {
                client_socket_array[j] = client_socket_array[j+1];
            }
            connect_count--;
            break;
        }
   }
   pthread_mutex_unlock(&mutex);

}
