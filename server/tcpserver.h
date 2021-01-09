#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "thread.h"
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "tcpsocket.h"

class tcpsocket;

//Tcp服务器的基类
class TcpServer
{
    friend class tcpsocket;
public:
    TcpServer(int threadNum = 8);

    int listen(int port,const char * ip = NULL);

    //服务器开始运行
    void start();

    //获取事件集合
     struct event_base * getBase();

protected:
    //监听回调函数，有客户端连接的时候会调用这个函数
    static void listener_cb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *);

    //监听处理函数
    void listenEvent(evutil_socket_t fd,struct sockaddr_in *);


    //---------------虚函数 去具体处理客户端逻辑------------
    //客户端连接事件
    virtual void connectEvent(tcpsocket * ){}

    //客户端可读事件
    virtual void readEvent(tcpsocket * ){}

    //客户端刻写事件
    virtual void writeEvent(tcpsocket * ){}

    //客户端关闭事件
    virtual void closeEvent(tcpsocket * ,short){}

private:
    //线程个数
    int m_threadNum;
    //线程池
   Thread * m_threadpool;

   //事件集合
   struct event_base * m_base;

   //用来监听客户端的套接字
   struct evconnlistener *m_listener;

   int m_nextThread;    //记录下一个线程的下标
};

#endif // TCPSERVER_H
