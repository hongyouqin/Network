#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/epoll.h>
#include <unistd.h>

 #define EPOLL_SIZE 2
 #define CLIENT_SIZE 100



void addfd(int event_fp, int fd) {
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    epoll_ctl(event_fp, EPOLL_CTL_ADD, fd, &event);
}

int main(int argc, char* argv[]) {
    if (argc <= 2) {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int ret;
    struct epoll_event* ep_events;
    int epoll_fp;
    int event_cnt;
    int client_sock;
    int client_array[CLIENT_SIZE];
    int client_count = 0; //客户端计数
    struct sockaddr_in clr_addr;
    char buf[1024];

    memset(&client_array, 0, CLIENT_SIZE);

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);
    
    int listen_socket = socket(PF_INET, SOCK_STREAM, 0);
    assert(listen_socket >= 0);

    ret = bind(listen_socket, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listen_socket, 5);
    assert(ret != -1);

    epoll_fp = epoll_create(EPOLL_SIZE);
    ep_events = (struct epoll_event*) malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    
    addfd(epoll_fp, listen_socket);

    while(1) {
        event_cnt = epoll_wait(epoll_fp, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1) {
            printf("epoll_wait() error\n");
            break;
        }

        printf("epoll wait \n");

        for(int i = 0; i < event_cnt; i++) {
            if (ep_events[i].data.fd == listen_socket) {
                //建立连接
                int adr_sz = sizeof(clr_addr);
                client_sock = accept(listen_socket, (struct sockaddr*)&clr_addr, (socklen_t*)&adr_sz);
                if (client_count+1 >= CLIENT_SIZE) {
                    close(client_sock);
                    break;
                }

                addfd(epoll_fp, client_sock);
                client_array[client_count++] = client_sock;

                printf("connected client: %d \n", client_sock);
            } else {
                //epoll默认是LT模式
                
               int len = read(ep_events[i].data.fd, buf, 100);
               if (len == 0) {
                   //关闭 request
                   epoll_ctl(epoll_fp, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                   close(ep_events[i].data.fd);

                    //删除指定的客户端
                    for (int i = 0; i < client_count; ++i) {
                        if (client_array[i] == ep_events[i].data.fd) {
                            //移动数据
                            if (i == client_count -1) {
                                client_count--;
                                break;
                            }
                            for (int j = i; j < client_count; ++j) {
                                client_array[j] = client_array[j+1];
                            }
                            client_count--;
                            break;
                        }
                    }

                    //通知其他
                   for (int i = 0; i < client_count; ++i) {
                       if (client_array[i] != ep_events[i].data.fd) {
                           write(client_array[i], "有人下线", len);
                       }
                   }

                   printf("closed client: %d\n", ep_events[i].data.fd);
               } else {
                   //广播给其他客户端
                   for (int i = 0; i < client_count; ++i) {
                       if (client_array[i] != ep_events[i].data.fd) {
                           write(client_array[i], buf, len);
                       }
                   }
                   
               }
            }
        }
    }
    close(listen_socket);
    close(epoll_fp);

    return 0;
}