#include "myserver.h"

Myserver::Myserver()
{
#ifdef DEBUG
    _log = spdlog::stdout_color_mt("BrainStorm");
#else
    _log = spdlog::rotating_logger_mt("BrainStorm", "BrainStorm", 1024*1024*5, 3);
    _log->flush_on(spdlog::level::info);
#endif

    this->m_db = new DB("127.0.0.1","root","12345678","BrainStrom");

    this->initRankMap();
}

void Myserver::connectEvent(tcpsocket * s)
{
    this->_log->info("有一个新连接[{}:{}]",s->getIp(),s->getPort());
}


void Myserver::readEvent(tcpsocket * s)
{
    char buf[1024] = {0};
    while(1)
    {
        int len = 0;
        s->readData(&len,sizeof(len));
        if(len <= 0)
        {
            break;
        }
        s->readData(buf,len);

        //数据解析
        Json::Value root;
        //json解析器
        Json::Reader reader;

        if(!reader.parse(buf,root))
        {
            _log->error("json 数据解析失败");
            return;
        }

        int cmd = root["cmd"].asInt();
        switch(cmd)
        {
        case REGISTER:
            this->Register(s,root);
            break;
        case LOGIN:
            this->Login(s,root);
            break;
        case SINGLE_GETQUESTION:
            this->sigleGetQuestion(s);
            break;
        case RANK:
            this->Rank(s);
            break;
        case ANSWER:
            this->rankAnswerQuestion(s,root);
            break;
        case RANKRESULT:
            this->rankResult(s,root);
            break;
        case UPDATERANK:
            this->updaterank(s,root);
            break;
        }

    }
}


void Myserver::writeEvent(tcpsocket * )
{

}


void Myserver::closeEvent(tcpsocket * s,short)
{
    //将用户从等待　rank　的队列中删除
    {
        std::unique_lock<std::mutex> lock(this->m_rankLock);
        int rank = this->m_user[s->getuserName()]->getRank();
        auto it = this->m_rankQueue.find(rank);
        if(it != this->m_rankQueue.end())
        {
            this->m_rankQueue.erase(it);
        }
    }

    std::unique_lock<std::mutex> lock(this->m_userlock);
    std::map<std::string,user *>::iterator it = m_user.begin();
    while(it != m_user.end())
    {
        if(it->second->getsocket() == s)
        {
            m_user.erase(it);
            this->_log->info("用户{} [{}:{}] logout",it->second->getName(),s->getIp(),s->getPort());

            //释放user
            delete it->second;
            return;
        }
        it++;
    }
    this->_log->info("[{}:{}] logout",s->getIp(),s->getPort());
}

//发送数据
void Myserver::writeData(tcpsocket * s,const Json::Value & inJson)
{
    std::string data = inJson.toStyledString();
    s->writeData(data.c_str(),data.length());
}

//用户注册
void Myserver::Register(tcpsocket * s,const Json::Value & inJson)
{
    std::string userName = inJson["userName"].asString();
    std::string passwd = inJson["passwd"].asString();

    //检测用户是否存在
    char sql[100] = {0};
    sprintf(sql,"select * from usertable where name = '%s' and passwd = '%s'",userName.c_str(),passwd.c_str());

    int result = OK;
    Json::Value outjson;
    bool ret = this->m_db->db_select(sql,outjson);
    if(!ret)
    {
        result = ERROR;
        this->_log->error("Register select user error");
    }

    if(outjson.isMember("name"))
    {
        result = USEREXIST;
    }
    else
    {
        sprintf(sql,"insert into usertable(name,passwd,rrank) values('%s','%s',0)",userName.c_str(),passwd.c_str());
        bool ret = this->m_db->db_exec(sql);
        if(!ret)
        {
            result = ERROR;
            this->_log->error("Register insert usertable error");
        }
        else
        {
            this->_log->info("Register user = {} success",userName);
        }
    }

    Json::Value json;
    json["cmd"] = REGISTER;
    json["result"] = result;

    this->writeData(s,json);
}


//用户登录
void Myserver::Login(tcpsocket * s,const Json::Value & inJson)
{
    std::string userName = inJson["userName"].asString();
    std::string passwd = inJson["passwd"].asString();

    //检测用户是否已经注册
    char sql[100] = {0};
    sprintf(sql,"select * from usertable where name = '%s' and passwd = '%s'",userName.c_str(),passwd.c_str());

    int result = OK;
    int rank;
    Json::Value outjson;
    bool ret = this->m_db->db_select(sql,outjson);
    if(!ret)
    {
        result = ERROR;
        this->_log->error("Login select user error");
    }

    //用户存在，表明已经注册过了
    if(outjson.isMember("name"))
    {
        std::unique_lock<std::mutex> lock(this->m_userlock);

        if(this->m_user.find(userName) != this->m_user.end())   //m_user是在线用户表，！＝　说明用户已经登录
        {
            result = USERLOGIN;
        }
        else
        {
            int i = 0;
            rank = atoi(outjson["rrank"][i].asString().c_str());
            printf("rank:%d\n",rank);
            user * use = new user(userName,passwd,rank,s);
            this->m_user.insert(make_pair(userName,use));
            this->_log->info("用户{}[{}:{}] login",userName,s->getIp(),s->getPort());
            s->setUserName(userName);
        }
    }
    else
    {
        result = NAMEORPASSWD;
    }


    Json::Value json;
    json["cmd"]     = LOGIN;
    json["result"]  = result;
    json["userName"] = userName;
    json["rank"]   = this->m_rankMap[rank];

    this->writeData(s,json);
}


//个人训练获取题目
void Myserver::sigleGetQuestion(tcpsocket * s)
{
    char sql[100] = {0};
    sprintf(sql,"select * from question order by rand() limit %d",QUESTION_NUM);

    int result = OK;
    Json::Value outjson;
    bool ret = this->m_db->db_select(sql,outjson);
    if(!ret || outjson["question"].size() != QUESTION_NUM)
    {
        result = ERROR;
        this->_log->error("sigleGetQuestion select user error");
    }

    Json::Value json;
    json["cmd"]     = SINGLE_GETQUESTION;
    json["result"]  = result;
    json["question"] = outjson;
    this->_log->info("用户{} [{}:{}] 获取题目：{}\n",s->getuserName(),s->getIp(),s->getPort(),json.toStyledString().c_str());

   this->writeData(s,json);
}


//初始化rank积分
void Myserver::initRankMap()
{
    char buf[100] = {0};
    int rank = 0;
    int starnum = 0;

    for(int i = 0;i < 100;i++)
    {
        if(i < 9)
        {
            rank = i / 3;
            starnum = i % 3;
            sprintf(buf,"青铜%d %d颗星",3-rank,starnum+1);
        }
        else if(i >= 9 && i < 18)
        {
            rank = (i-9) / 3;
            starnum = (i -9) % 3;
            sprintf(buf,"白银%d %d颗星",3-rank,starnum+1);
        }
        else if(i >= 18 && i < 34)
        {
            rank = (i-18) / 4;
            starnum = (i -18) % 4;
            sprintf(buf,"黄金%d %d颗星",4-rank,starnum+1);
        }
        else if(i >= 34 && i < 50)
        {
            rank = (i-34) / 4;
            starnum = (i -34) % 4;
            sprintf(buf,"铂金%d %d颗星",4-rank,starnum+1);
        }
        else if(i >= 50 && i < 75)
        {
            rank = (i-50) / 5;
            starnum = (i -50) % 5;
            sprintf(buf,"钻石%d %d颗星",5-rank,starnum+1);
        }
        else if(i >= 75 && i < 100)
        {
            rank = (i-75) / 5;
            starnum = (i -75) % 5;
            sprintf(buf,"星耀%d %d颗星",5-rank,starnum+1);
        }
        this->m_rankMap.insert(std::make_pair(i,buf));
    }
}


//进行排位
void Myserver::Rank(tcpsocket * s)
{
    //对手
    tcpsocket * other = NULL;

    int rank = this->m_user[s->getuserName()]->getRank();

    std::unique_lock<std::mutex> lock(this->m_rankLock);
    //查找同一段位的对手
    std::map<int,tcpsocket *>::iterator it = this->m_rankQueue.find(rank);
    if(it != this->m_rankQueue.end())
    {
        other = it->second;
        this->m_rankQueue.erase(it);
    }
    else
    {
        //查找其他段位的选手，积分差值的绝对值要小于５的值都可以进行对决
        for(int i = 1;i< 5;i++)
        {
            it = this->m_rankQueue.find(rank +i);
            if(it != this->m_rankQueue.end())
            {
                other = it->second;
                this->m_rankQueue.erase(it);
                break;
            }

             it = this->m_rankQueue.find(rank - i);
             if(it != this->m_rankQueue.end())
             {
                 other = it->second;
                 this->m_rankQueue.erase(it);
                 break;
             }
        }
    }

    this->_log->info("rank 分数:{}",rank);
    //没有匹配的用户，加入到匹配队列中
    if(other == NULL)
    {
         this->m_rankQueue.insert((make_pair(rank,s)));
         this->_log->info("当前等候　rank　的人数{} ",this->m_rankQueue.size());
    }
    else
    {
        //开始对决
        this->startRank(s,other);
    }
}

//开始对决
void Myserver::startRank(tcpsocket * first,tcpsocket * second)
{
    char sql[100] = {0};
    sprintf(sql,"select * from question order by rand() limit %d",QUESTION_NUM);

    int result = OK;
    Json::Value outjson;
    bool ret = this->m_db->db_select(sql,outjson);
    if(!ret || outjson["question"].size() != QUESTION_NUM)
    {
        result = ERROR;
        this->_log->error("startRank select user error");
    }

    Json::Value json;
    json["cmd"]     = RANK;
    json["result"]  = result;
    json["question"] = outjson;

    //first user
    json["enemyName"] = second->getuserName();
    json["enemyRank"] = this->m_rankMap[this->m_user[second->getuserName()]->getRank()];
    json["enemyScore"] = second->getuserName();

    this->writeData(first,json);

    //second　user
    json["enemyName"] = first->getuserName();
    json["enemyRank"] = this->m_rankMap[this->m_user[first->getuserName()]->getRank()];
    json["enemyScore"] = second->getuserName();
    this->writeData(second,json);

    this->_log->info("获取 rank 题目:{}\n",json.toStyledString());
}


//排位　回答一道问题
void Myserver::rankAnswerQuestion(tcpsocket * s,const Json::Value & inJson)
{
   std::string enemyName = inJson["enemyName"].asString();
   user * use = this->m_user[enemyName];

    Json::Value json;
    json["cmd"] =  ANSWER;
    json["enemyName"] = enemyName;
    json["enemyscore"] = inJson["score"].asInt();
    json["enemyQuestionId"] = inJson["questionId"].asInt();

    this->writeData(use->getsocket(),json);
}


//rank　结果
void Myserver::rankResult(tcpsocket * s,const Json::Value & inJson)
{
    std::unique_lock<std::mutex> lock(this->m_userlock);
    user * use = this->m_user[s->getuserName()];

    int  score = inJson["score"].asInt();
    int enemyscore = inJson["enemyScore"].asInt();

    if(score < enemyscore)
    {
        use->setRank(use->getRank() - 1);
    }
    else
    {
        use->setRank(use->getRank() + 1);
    }

    Json::Value json;
    json["cmd"] =  RANKRESULT;
    json["newRank"] = this->m_rankMap[use->getRank()];

    this->writeData(s,json);
}

//更新数据库各自的段位
void Myserver::updaterank(tcpsocket * s,const Json::Value & inJson)
{
    int newRank = inJson["selfrank"].asInt();

    const char * name = s->getuserName().c_str();

    char sql[100] = {0};
    sprintf(sql,"update usertable set rrank = %d where name = '%s'",newRank,name);

    this->m_db->db_exec(sql);
}








