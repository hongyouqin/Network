#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
 #include <assert.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc <= 2) {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int server_socket;
    char buf[BUF_SIZE];
    struct sockaddr_in server_addr;
    FILE* readfp;
    FILE* writefp;


    server_socket = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    int ret = connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    assert(ret != -1);
    readfp = fdopen(server_socket, "r");
    writefp = fdopen(server_socket, "w");

    fputs("form client: Thank you!\n", writefp);
    fflush(writefp);


    while(1) {
        if (fgets(buf, sizeof(buf), readfp) == NULL) {
            break;
        }
        fputs(buf, stdout);
        fflush(stdout);
    }
    fclose(writefp);
    fclose(readfp);
    //close(server_socket);
    

    return 0;
}