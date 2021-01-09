#ifndef COMMON_H
#define COMMON_H

//每次出题的数量
#define QUESTION_NUM 5


//5XXX  用户操作指令
#define OK                          5000
#define REGISTER                    5001   //注册
#define LOGIN                       5002   //登录
#define SINGLE_GETQUESTION          5003   //个人训练获取题目
#define RANK                        5004   //排位
#define ANSWER                      5005   //排位回答一个问题
#define RANKRESULT                  5006   //rank结束，发送对决结果
#define UPDATERANK                  5007   //rank结束，更新自己和对手的段位




//8XXX　错误
#define ERROR        8001   //未知错误
#define USEREXIST    8002   //用户已经存在
#define NAMEORPASSWD 8003   //用户名或者密码错误
#define USERLOGIN    8004   //用户已经登录


#endif // COMMON_H
