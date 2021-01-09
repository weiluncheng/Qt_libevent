#include "user.h"

user::user(std::string n,std::string p,int rank,tcpsocket * s)
    :m_userName(n),m_passwd(p),m_rank(rank),s(s)
{

}

tcpsocket * user::getsocket()
{
  return s;
}

const char * user::getName()
{
   return this->m_userName.c_str();
}

int user::getRank()
{
    return this->m_rank;
}

void user::setRank(int rank)
{
    if(rank < 0)
    {
        rank = 0;
    }
    this->m_rank = rank;
}
