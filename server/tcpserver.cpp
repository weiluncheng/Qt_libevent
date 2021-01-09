#include "tcpserver.h"

TcpServer::TcpServer(int threadNum):m_nextThread(0)
{
    if(threadNum < 0)
    {
        printf("threadNum < 0\n");
        exit(-1);
    }

    //创建线程池
    this->m_threadNum = threadNum;
    this->m_threadpool = new Thread[this->m_threadNum];
    if(this->m_threadpool == NULL)
    {
        printf("create threadpoolm error\n");
        exit(-1);
    }

    this->m_base = event_base_new();
    if(this->m_base == NULL)
    {
        printf("event_base_new error");
        exit(-1);
    }
}

int TcpServer::listen(int port,const char * ip)
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    if(ip != NULL)
    {
       inet_aton(ip, &sin.sin_addr);
    }

    m_listener = evconnlistener_new_bind(
                this->m_base,
                listener_cb,
                this,
                LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
                -1,
                (struct sockaddr*)&sin,
                sizeof(sin));
    if (!m_listener) {
        fprintf(stderr, "Could not create a listener!\n");
        return -1;
    }

    //开启线程池
    for(int i = 0;i< this->m_threadNum;i++)
    {
        this->m_threadpool[i].start();
        printf("%d线程开启\n",i+1);
    }
    return 1;
}

void TcpServer::listener_cb(struct evconnlistener *, evutil_socket_t fd, struct sockaddr * client_addr, int , void * data)
{
    TcpServer * p = (TcpServer *)data;

    p->listenEvent(fd,(struct sockaddr_in *)client_addr);
}

//监听处理函数
void TcpServer::listenEvent(evutil_socket_t fd,struct sockaddr_in * clientaddr)
{
    //客户端的ip地址
    char * ip = inet_ntoa(clientaddr->sin_addr);
    //客户端的端口
    uint16_t port =  ntohs(clientaddr->sin_port);

    //printf("ip:%s,port:%d\n",ip,port);


    //从线程池中选择一个线程去处理客户端的请求
    //以轮询的方式选择线程
    struct event_base * base = this->m_threadpool[this->m_nextThread].getBase();
    //类似与循环队列
    this->m_nextThread = (this->m_nextThread + 1) % this->m_threadNum;
    struct bufferevent * bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev)
    {
         fprintf(stderr, "Error constructing bufferevent!");
         event_base_loopbreak(base);
         return;
    }

    //创建一个通信对象
    tcpsocket * s = new tcpsocket(this,bev,ip,port);

    //单独封装一个类来和客户端进行通信
    bufferevent_setcb(bev, s->readEventCb, s->writeEventCb, s->closeEventCb, s);
    bufferevent_enable(bev, EV_WRITE);
    bufferevent_enable(bev, EV_READ);
    bufferevent_enable(bev, EV_SIGNAL);

    //调用客户端连接事件
    this->connectEvent(s);
}

//服务器开始运行
void TcpServer::start()
{
    event_base_dispatch(this->m_base);

    evconnlistener_free(this->m_listener);

    event_base_free(this->m_base);
}



