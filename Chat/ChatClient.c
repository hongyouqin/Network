#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>


#define BUF_SIZE 4
int result;

static void *ChatSend(void *arg);
static void *ChatRecv(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("usage %s <ip> <port>/n", argv[0]);
        exit(1);
    }
    
    printf("ip: %s\n", argv[1]);
    printf("port: %s\n", argv[2]);
    printf("name: %s\n", argv[3]);

    char msg[BUF_SIZE];
    char out[BUF_SIZE];
    int read_cnt;
    struct sockaddr_in serv_addr;
    pthread_t thread_id;
   
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1 ) {
        printf("create socket failed\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));  
    if (connect(client_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1 ) {
        printf("connect service failed : %s\n", strerror(errno));
        exit(1);
    }

    result =  pthread_create(&thread_id, NULL, &ChatSend, (void*)&client_socket);
    if (result != 0) {
        exit(1);
    }
    pthread_detach(thread_id);

    result =  pthread_create(&thread_id, NULL, &ChatRecv, (void*)&client_socket);
    if (result != 0) {
        exit(1);
    }
    pthread_detach(thread_id);
    

    while((read_cnt = read(client_socket, msg, BUF_SIZE)) != 0) {
       memcpy(out, msg, read_cnt);
       out[read_cnt] = '\0';
       printf("%s", out);
    }
    printf("\n");
    close(client_socket);

    exit(0);
}

static void *ChatSend(void *arg) {

}

static void *ChatRecv(void *arg) {

}
