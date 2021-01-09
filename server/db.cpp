#include "db.h"

DB::DB(const char * host,const char * username,const char * passwd,const char * dbname)
{
    //初始化数据库句柄
    this->m_mysql = mysql_init(NULL);
    if(this->m_mysql == NULL)
    {
        spdlog::get("BrainStorm")->error("mysql_init error\n");
        exit(-1);
    }

    //连接mysql数据库
    MYSQL * con = mysql_real_connect(this->m_mysql,host,username,passwd,dbname,0,NULL,0);
    if(con == NULL)
    {
        spdlog::get("BrainStorm")->error("连接数据库失败:{}",mysql_error(this->m_mysql));
        exit(-1);
    }
    this->m_mysql = con;

    //设置字符集
    int ret = mysql_query(this->m_mysql,"set names utf8");
    if(ret != 0)
    {
        spdlog::get("BrainStorm")->error ("设置字符集失败:%s",mysql_error(this->m_mysql));
        exit(-1);
    }
}

//执行数据库语句
bool DB::db_exec(const char * sql)
{
    //数据库句柄上锁
    std::unique_lock<std::mutex> lock(this->m_mutex);

    int ret = mysql_query(this->m_mysql,sql);
    if(ret != 0)
    {
        spdlog::get("BrainStorm")->error("mysql_query error:%s",mysql_error(this->m_mysql));
        return false;
    }
    return true;
}

//数据库的查询
//[in]  sql查询语句
//[out] outjson 查询结果保存到Json变量中
bool DB::db_select(const char * sql,Json::Value & outJson)
{
    //数据库句柄上锁
    std::unique_lock<std::mutex> lock(this->m_mutex);

    //执行sql语句
    int ret = mysql_query(this->m_mysql,sql);
    if(ret != 0)
    {
        spdlog::get("BrainStorm")->error("mysql_query error:%s",mysql_error(this->m_mysql));
        return false;
    }

    //从mysql服务器下载查询结果
    MYSQL_RES * sql_res = mysql_store_result(this->m_mysql);
    if(sql_res == NULL)
    {
        if(mysql_errno(this->m_mysql) == 0)\
        {
            return true;
        }
        else
        {
             spdlog::get("BrainStorm")->error("mysql_store_result error:%s",mysql_error(this->m_mysql));
        }
    }

    //从结果集中一行一行的取出数据
    MYSQL_ROW row;
    //获取列数
    unsigned int num_fileds = mysql_num_fields(sql_res);
    //获取表头
    MYSQL_FIELD * fetch_field = mysql_fetch_field(sql_res);


    //一行一行的获取数据
    while((row = mysql_fetch_row(sql_res)))
    {
        for(unsigned int i = 0;i<num_fileds;i++)
        {
            outJson[fetch_field[i].name].append(row[i]);
        }
    }
    mysql_free_result(sql_res);
    return true;
}
