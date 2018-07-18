# -Network
网络编程相关

## Hello文件夹
    是一个简单的tcp客户端／服务端socket程序.
    用法:
    gcc SimpleClient.c -o client
    gcc SimpleServer.c -o server
    ./server 9999
    ./client 127.0.0.1 9999
 
## Chat聊天室
   是一个简单的聊天室，用法如下：  
   gcc -o server -pthread ChatServer.c  
   gcc -o client -pthread ChatClient.c
   
   ./server 8989  
   ./client 127.0.0.1 9999 [chat one]  
   ./client 127.0.0.1 9999 [chat two]  
   
   最大人数5人，可以去程序里修改人数
   
## unix_web 
   里面的内容是我学习unix网络第2版（1+2卷）的例子，这些例子都可以单独编译。不需要像书里那么多依赖  
   
   daytimetcp_client.c 功能是连接开启daytime服务的机子，服务器那边返回日期信息。

