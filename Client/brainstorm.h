#ifndef BRAINSTORM_H
#define BRAINSTORM_H

#include <QDialog>
#include "communicate.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>
#include <QString>



namespace Ui {
class BrainStorm;
}

class BrainStorm : public QDialog
{
    Q_OBJECT

public:
    explicit BrainStorm( Communicate * com,QJsonObject  &json, QWidget *parent = 0);
    ~BrainStorm();

private:
    //单人训练从服务器获取题目
    void sigleGetQuestion();

    //单人训练设置问题到相应的控件上
    void sigleSetQuestion();

    //单人训练回答问题
    void sigleAnswerQuestion(int select);

    //设置对手的得分
    void setEnemyScoer();
    //设置自己的得分
    void setSelfScore();

    //rank设置题目
    void rankSetQuestion();

    //设置rank结果
    void ranksetResult(QJsonObject  &json);


private slots:
    void on_singleButton_clicked();

    void on_pushButton_4_clicked();

    void on_sigle_back_clicked();

    void on_sigle_start_clicked();

    //单人训练从服务器接收到题目
    void recvSigleQuestion(QJsonObject json);


    //单人训练的定时器
    void sigleTimerOut();

    //rank训练的定时器
    void ranklTimerOut();

    //排位赛训练回答问题
    void rankAnswerQuestion(int select);


    void on_pushButton_clicked();

    void on_sigleButton1_clicked();

    void on_sigleButton2_clicked();

    void on_sigleButton3_clicked();

    void on_sigleButton4_clicked();

    void on_rankButton_clicked();


    //接收排位
    void Rank(QJsonObject json);


    void on_rankSelectButton1_clicked();

    void on_rankSelectButton2_clicked();

    void on_rankSelectButton3_clicked();

    void on_rankselectButton4_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::BrainStorm *ui;

    //通信类指针，负责和服务器通信
    Communicate * m_com;


    QJsonObject m_sigleQuestionjson;

    //当前回答问题的下标
    int m_currentSigleQuestion;

    //个人训练定时器
    QTimer m_sigleTimer;

    //个人训练答题时间
    int m_sigleSec;

    //个人训练得分
    int m_sigleScore;


    // -------------------------- rank -----------------------
    QJsonObject m_rankQuestion;      //rank的题
    QString m_enemyName;                //对手名字
    QString m_enemyRank;                 //对手段位
    int m_enemyScore;                         //对手得分

    int m_rankSec;  //答题计时
    int m_myScore; //自己的得分

    QString m_userName; //自己的用户名
    QString m_userRank; //自己的段位


     //当前回答问题的下标
    int m_currentRankQuestion;
    //对手当前回答问题的下标
    int m_enemycurrentRankQuestion;

    //rank训练定时器
    QTimer m_rankTimer;
};

#endif // BRAINSTORM_H
