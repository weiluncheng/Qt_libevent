#ifndef USER_H
#define USER_H
#include <string>
#include "tcpsocket.h"

class user
{
public:
    user(std::string n,std::string p,int rank,tcpsocket * s);

    tcpsocket * getsocket();

    const char * getName();

    int getRank();

    void setRank(int rank);
private:
    std::string m_userName; //用户名
    std::string m_passwd;   //用户密码
    int m_rank; //用户等级

    //通信套接字
    tcpsocket * s;
};

#endif // USER_H
