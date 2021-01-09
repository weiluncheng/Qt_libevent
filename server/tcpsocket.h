#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "tcpserver.h"
#include <string>

class TcpServer;

class tcpsocket
{
public:
    tcpsocket( TcpServer * tcpserver,struct bufferevent * bev, char * ip,u_int16_t port);

    //可读事件回调函数
    static void readEventCb(struct bufferevent *bev, void *ctx);
    //可写事件回调函数
    static void writeEventCb(struct bufferevent *bev, void *ctx);
    //关闭事件回调函数
    static void closeEventCb(struct bufferevent *bev, short what, void *ctx);

    //获取IP地址
    char * getIp();
    //获取端口
    u_int16_t getPort();

    //从客户端读数据
    int readData(void * data,int size);
    //向客户端写数据
     int writeData(const void * data,int size);

     //设置用户名
     void setUserName(std::string name);
     //获取用户名
     std::string getuserName();
private:
    //服务器类对象
    static TcpServer * m_tcpserver;
    //与客户端的通信句柄
    struct bufferevent * m_bev;
    //客户端的IP
    char * m_ip;
    //客户端的端口
    u_int16_t m_port;

    std::string userName;    //用户名
};

#endif // TCPSOCKET_H
