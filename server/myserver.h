#ifndef MYSERVER_H
#define MYSERVER_H

#include "tcpserver.h"
#include <spdlog/spdlog.h>
#include "db.h"
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>    //做json数据解析
#include "common.h"
#include <string>
#include <iostream>
using namespace std;

#include <map>
#include "user.h"
#include <mutex>
#include "tcpsocket.h"

#define DEBUG



class Myserver:public TcpServer
{
public:
    Myserver();


    void connectEvent(tcpsocket * );


    void readEvent(tcpsocket * );


    void writeEvent(tcpsocket * );


    void closeEvent(tcpsocket * ,short);

private:
    //发送数据
    void writeData(tcpsocket * s,const Json::Value & inJson);
    //用户注册
    void Register(tcpsocket * s,const Json::Value & inJson);

    //用户登录
    void Login(tcpsocket * s,const Json::Value & inJson);

    //个人训练获取题目
    void sigleGetQuestion(tcpsocket * s);

    //初始化rank积分
    void initRankMap();

    //进行排位
    void Rank(tcpsocket * s);

    //开始对决
    void startRank(tcpsocket * first,tcpsocket * second);

    //排位　回答一道问题
    void rankAnswerQuestion(tcpsocket * s,const Json::Value & inJson);

    //rank　结果
    void rankResult(tcpsocket * s,const Json::Value & inJson);

    //更新数据库各自的段位
    void updaterank(tcpsocket * s,const Json::Value & inJson);

private:
    //记录日志的句柄
    std::shared_ptr<spdlog::logger> _log;

    //数据库句柄
    DB * m_db;


    std::mutex m_userlock;
    //在线用户列表
    //键：用户名
    //值：用户指针
    std::map<std::string,user *> m_user;


    //rank积分比对表 段位
    //键：rank   值：对应的段位说明
    std::map<int,std::string> m_rankMap;


    //排位队列
    //键：rank积分  值：参与rank的用户socket句柄
    std::mutex m_rankLock;
     std::map<int,tcpsocket *> m_rankQueue;
   // std::map<int,tcpsocket * s> m_rankQueue;   //等待排位的队列
};

#endif // MYSERVER_H
