#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#define MAX_LISTEN_NUMBER  5 //监听数


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage %s <port>/n", argv[0]);
	    exit(1);	
	}

    struct sockaddr_in client_addr;
    int client_address_len;
    int client_socket;
	
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
    
    while(1) {
        client_address_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr,&client_address_len);
        if (client_socket == -1) {
            printf("client connect failed\n");
            exit(1);
        }
        char message[] = "Hello client how are you nice to meet you";
        write(client_socket,message,sizeof(message));
        close(client_socket);
   }

   close(server_socket);
   exit(0);
}
