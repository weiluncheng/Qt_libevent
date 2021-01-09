#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>
#include <mutex>
#include <spdlog/spdlog.h>
#include <jsoncpp/json/json.h>

class DB
{
public:
    DB(const char * host,const char * username,const char * passwd,const char * dbname);

    //执行数据库语句
    bool db_exec(const char * sql);

    //数据库的查询
    //[in]  sql查询语句
    //[out] outjson 查询结果保存到Json变量中
    bool db_select(const char * sql,Json::Value & outJson);
private:
    //数据库句柄的互斥锁
    std::mutex m_mutex;
    //数据库句柄
    MYSQL * m_mysql;
};

#endif // DB_H
