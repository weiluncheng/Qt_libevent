#include "tcpsocket.h"

//服务器类对象
TcpServer * tcpsocket::m_tcpserver = NULL;

tcpsocket::tcpsocket(TcpServer * tcpserver,struct bufferevent * bev, char * ip,u_int16_t port)
{
    this->m_tcpserver = tcpserver;
    this->m_bev = bev;
    this->m_ip = ip;
    this->m_port = port;

}

//可读事件回调函数
void tcpsocket::readEventCb(struct bufferevent *, void *ctx)
{
    tcpsocket * s = (tcpsocket *)ctx;

    s->m_tcpserver->readEvent(s);
}

//可写事件回调函数
void tcpsocket::writeEventCb(struct bufferevent *, void *ctx)
{
    tcpsocket * s = (tcpsocket *)ctx;

    s->m_tcpserver->writeEvent(s);
}

//关闭事件回调函数
void tcpsocket::closeEventCb(struct bufferevent *, short what, void *ctx)
{
    tcpsocket * s = (tcpsocket *)ctx;

    s->m_tcpserver->closeEvent(s,what);

    //客户端关闭，delete,内存回收，防止内存泄露
    delete s;
}

//获取IP地址
char * tcpsocket::getIp()
{
    return this->m_ip;
}

//获取端口
u_int16_t tcpsocket::getPort()
{
    return this->m_port;
}

//从客户端读数据
int tcpsocket::readData(void * data,int size)
{
    return bufferevent_read(this->m_bev,data,size);
}

//向客户端写数据
int tcpsocket::writeData(const void * data,int size)
{
    return bufferevent_write(this->m_bev,data,size);
}

void tcpsocket::setUserName(std::string name)
{
  this->userName = name;
}

 std::string tcpsocket::getuserName()
 {
     return this->userName;
 }
