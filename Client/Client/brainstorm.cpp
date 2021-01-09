#include "brainstorm.h"
#include "ui_brainstorm.h"

BrainStorm::BrainStorm(Communicate * com,QJsonObject & json,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrainStorm)
{
    ui->setupUi(this);

    this->m_com = com;

    //自己的用户名
    this->m_userName = json["userName"].toString();
    //自己的段位
    this->m_userRank = json["rank"].toString();

    //单人训练定时器
    connect(&this->m_sigleTimer,SIGNAL(timeout()),this,SLOT(sigleTimerOut()));


    //排位赛的定时器
     connect(&this->m_rankTimer,SIGNAL(timeout()),this,SLOT(ranklTimerOut()));


    //接收问题
    connect(this->m_com,SIGNAL(recvSigleQuestion(QJsonObject)),this,SLOT(recvSigleQuestion(QJsonObject)));

    //排位
    connect(this->m_com,SIGNAL(Rank(QJsonObject)),this,SLOT(Rank(QJsonObject)));
}


//个人训练定时器
void  BrainStorm::sigleTimerOut()
{
    -- this->m_sigleSec;
    if( this->m_sigleSec == 0)
    {
        ui->sigleResult->setText("回答错误");
        QString str = QString("本次得分：%1").arg(m_sigleScore);
        //显示得分
        ui->siglescore->setText(str);

        this->m_sigleTimer.stop();
        ui->stackedWidget->setCurrentWidget(ui->sigle_score);
    }
    ui->lcdNumber->display( this->m_sigleSec);
}

BrainStorm::~BrainStorm()
{
    delete ui;
}

//单人训练从服务器获取题目
void BrainStorm::sigleGetQuestion()
{
    QJsonObject json;
    json["cmd"] = SINGLE_GETQUESTION;

    this->m_com->writeData(json);
}


//单人训练设置问题
void BrainStorm::sigleSetQuestion()
{


    ui->sigleQuestion->setText(this->m_sigleQuestionjson["question"].toArray().at(this->m_currentSigleQuestion).toString());

    ui->sigleButton1->setText(this->m_sigleQuestionjson["selection1"].toArray().at(this->m_currentSigleQuestion).toString());
    ui->sigleButton2->setText(this->m_sigleQuestionjson["selection2"].toArray().at(this->m_currentSigleQuestion).toString());
    ui->sigleButton3->setText(this->m_sigleQuestionjson["selection3"].toArray().at(this->m_currentSigleQuestion).toString());
    ui->sigleButton4->setText(this->m_sigleQuestionjson["selection4"].toArray().at(this->m_currentSigleQuestion).toString());
}

//单人训练回答问题
void BrainStorm::sigleAnswerQuestion(int select)
{
    //回答正确
    if(select == this->m_sigleQuestionjson["answer"].toArray().at(this->m_currentSigleQuestion).toString().toInt())
    {
        this->m_sigleScore += 20 * this->m_sigleSec;
        this->m_currentSigleQuestion++;
          //设置下一题
        this->sigleSetQuestion();

        //倒计时10s
        this->m_sigleSec = 10;

        this->m_sigleTimer.stop();

        ui->lcdNumber->display(this->m_sigleSec);

        this->m_sigleTimer.start(1000);

    }
    else
    {
        ui->sigleResult->setText("回答错误");
        QString str = QString("本次得分：%1").arg(m_sigleScore);
        //显示得分
        ui->siglescore->setText(str);


        ui->stackedWidget->setCurrentWidget(ui->sigle_score);
         this->m_sigleTimer.stop();
    }

    //题目结束
    if(this->m_currentSigleQuestion == QUESTION_NUM)
    {
          ui->sigleResult->setText("恭喜全部答对");
        QString str = QString("本次得分：%1").arg(m_sigleScore);
        //显示得分
        ui->siglescore->setText(str);

        ui->stackedWidget->setCurrentWidget(ui->sigle_score);
        this->m_sigleTimer.stop();
    }
}

//从服务器接收题目
void BrainStorm::recvSigleQuestion(QJsonObject json)
{
     this->m_sigleQuestionjson = json;

    //设置定时器的倒计时时间
    this->m_sigleSec = 10;

    //当前回答问题的下标
    this->m_currentSigleQuestion = 0;

    //个人训练得分
    this->m_sigleScore = 0;

    //定时器显示时间
    ui->lcdNumber->display(this->m_sigleSec);

    sigleSetQuestion();

    //开启定时器
    this->m_sigleTimer.start(1000);

    ui->stackedWidget->setCurrentWidget(ui->sigle_running);
}

//进入单人训练
void BrainStorm::on_singleButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->single_menu);
}

//单人训练返回按钮
void BrainStorm::on_sigle_back_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->mainMenu);
}

//开始答题
void BrainStorm::on_sigle_start_clicked()
{
    this->sigleGetQuestion();
}






void BrainStorm::on_pushButton_clicked()
{
     ui->stackedWidget->setCurrentWidget(ui->mainMenu);
}

//选项A
void BrainStorm::on_sigleButton1_clicked()
{
    this->sigleAnswerQuestion(1);
}

//选项B
void BrainStorm::on_sigleButton2_clicked()
{
     this->sigleAnswerQuestion(2);
}

//选项C
void BrainStorm::on_sigleButton3_clicked()
{
     this->sigleAnswerQuestion(3);
}

//选项D
void BrainStorm::on_sigleButton4_clicked()
{
     this->sigleAnswerQuestion(4);
}

// -------------------------------  rank  ----------------------------------
// -------------------------------  rank  ----------------------------------

//排位赛按钮
void BrainStorm::on_rankButton_clicked()
{
    QJsonObject json;
    json["cmd"] = RANK;

    this->m_com->writeData(json);

    //跳到等待页面
    ui->stackedWidget->setCurrentWidget(ui->rank_wait);
}

//设置对手的得分
void BrainStorm::setEnemyScoer()
{
    QString str = QString("%1(%2)：%3").arg(this->m_enemyName,-5).arg(this->m_enemyRank).arg(this->m_enemyScore);
    ui->enemyStatus->setText(str);
}

//设置自己的得分
void BrainStorm::setSelfScore()
{
     QString str = QString("%1(%2)：%3").arg(this->m_userName,-5).arg(this->m_userRank).arg(this->m_myScore);
     ui->selfStatus->setText(str);
}

//rank设置题目
void BrainStorm::rankSetQuestion()
{
    ui->rankQuestion->setText(this->m_rankQuestion["question"].toArray().at(this->m_currentRankQuestion).toString());

    ui->rankSelectButton1->setText(this->m_rankQuestion["selection1"].toArray().at(this->m_currentRankQuestion).toString());
    ui->rankSelectButton2->setText(this->m_rankQuestion["selection2"].toArray().at(this->m_currentRankQuestion).toString());
    ui->rankSelectButton3->setText(this->m_rankQuestion["selection3"].toArray().at(this->m_currentRankQuestion).toString());
    ui->rankselectButton4->setText(this->m_rankQuestion["selection4"].toArray().at(this->m_currentRankQuestion).toString());



    ui->rankSelectButton1->setEnabled(true);
    ui->rankSelectButton2->setEnabled(true);
    ui->rankSelectButton3->setEnabled(true);
    ui->rankselectButton4->setEnabled(true);


    //样式清空
    ui->rankSelectButton1->setStyleSheet("");
    ui->rankSelectButton2->setStyleSheet("");
    ui->rankSelectButton3->setStyleSheet("");
    ui->rankselectButton4->setStyleSheet("");

    if(this->m_currentRankQuestion == QUESTION_NUM)
    {
        this->m_rankTimer.stop();

        //将结果发给服务器
        QJsonObject json;
        json["cmd"] =  RANKRESULT;
        json["score"] = this->m_myScore;
        json["enemyName"] = this->m_enemyName;
        json["enemyScore"] = this->m_enemyScore;

        this->m_com->writeData(json);
    }
}

//rank训练的定时器
void  BrainStorm::ranklTimerOut()
{
    this->m_rankSec--;
    if(this->m_rankSec == 0)
    {
        if(ui->rankSelectButton1->isEnabled())
        {
            this->m_currentRankQuestion++;
        }

        this->rankSetQuestion();

        this->m_rankSec = 10;
    }
    ui->lcdNumber_2->display(this->m_rankSec);
}


//设置rank结果
void BrainStorm::ranksetResult(QJsonObject  &json)
{
    QString newRank = json["newRank"].toString();
    if(this->m_myScore == m_enemyScore)
    {
        ui->rankResult->setText("平局");
    }
    if(this->m_myScore > m_enemyScore)
    {
        ui->rankResult->setText("胜利");
    }
    if(this->m_myScore < m_enemyScore)
    {
        ui->rankResult->setText("失败");
    }

    QString str = QString("%1 ---> %2").arg(this->m_userRank).arg(newRank);
    ui->newrank->setText(str);

    this->m_userRank = newRank;

    ui->stackedWidget->setCurrentWidget(ui->rank_result);
}


//接收排位
void BrainStorm::Rank(QJsonObject json)
{
    int cmd = json["cmd"].toInt();

    if(cmd == RANK)
    {
        this->m_rankQuestion = json["question"].toObject();
        this->m_enemyName = json["enemyName"].toString();
        this->m_enemyRank = json["enemyRank"].toString();
        this->m_enemyScore = json["enemyScore"].toInt();

        this->m_rankSec = 10;


        this->m_myScore = 0;

          //当前回答问题的下标
         this->m_currentRankQuestion = 0;
        this->m_enemycurrentRankQuestion = 0;

        //计时器
        ui->lcdNumber_2->display(this->m_rankSec);


        this->setEnemyScoer();
        this->setSelfScore();

        //设置题目
        this->rankSetQuestion();

        this->m_rankTimer.start(1000);

        //跳转界面
        ui->stackedWidget->setCurrentWidget(ui->rank_runing);
    }
    else if(cmd == ANSWER)
    {
        this->m_enemyScore = json["enemyscore"].toInt();
        this->m_enemycurrentRankQuestion = json["enemyQuestionId"].toInt();

        setEnemyScoer();

        if(this->m_currentRankQuestion == this->m_enemycurrentRankQuestion)
        {
            this->m_rankSec = 10;
            this->m_rankTimer.stop();

            ui->lcdNumber_2->display(this->m_rankSec);

            this->m_rankTimer.start(1000);

            this->rankSetQuestion();
        }
    }
    else if(cmd == RANKRESULT)
    {
        this->ranksetResult(json);
    }

}

//排位赛训练回答问题
void BrainStorm::rankAnswerQuestion(int select)
{
    //计算得分
    if(select == this->m_rankQuestion["answer"].toArray().at(this->m_currentRankQuestion).toString().toInt())
    {
        this->m_myScore += 20 * this->m_rankSec;
    }

    this->setSelfScore();
    this->m_currentRankQuestion++;

    //判断是否跳到下一题
    if(this->m_currentRankQuestion == this->m_enemycurrentRankQuestion)
    {
        this->m_rankSec = 10;
        //计时器重置
        this->m_rankTimer.stop();

        //数码管重置
        ui->lcdNumber_2->display(this->m_rankSec);

        this->m_rankTimer.start(1000);


        this->rankSetQuestion();
    }

    QJsonObject json;
    json["cmd"] =  ANSWER;
    json["enemyName"] = this->m_enemyName;
    json["score"] = this->m_myScore;
    json["questionId"] = this->m_currentRankQuestion;

    this->m_com->writeData(json);
}



//排位赛按钮1
void BrainStorm::on_rankSelectButton1_clicked()
{
    ui->rankSelectButton1->setStyleSheet("background-color:rgb9170，170，255");
    ui->rankSelectButton1->setEnabled(false);
    ui->rankSelectButton2->setEnabled(false);
    ui->rankSelectButton3->setEnabled(false);
    ui->rankselectButton4->setEnabled(false);

    this->rankAnswerQuestion(1);
}

//排位赛按钮2
void BrainStorm::on_rankSelectButton2_clicked()
{
     ui->rankSelectButton2->setStyleSheet("background-color:rgb9170，170，255");
    ui->rankSelectButton1->setEnabled(false);
    ui->rankSelectButton2->setEnabled(false);
    ui->rankSelectButton3->setEnabled(false);
    ui->rankselectButton4->setEnabled(false);

      this->rankAnswerQuestion(2);
}

//排位赛按钮3
void BrainStorm::on_rankSelectButton3_clicked()
{
     ui->rankSelectButton3->setStyleSheet("background-color:rgb9170，170，255");
    ui->rankSelectButton1->setEnabled(false);
    ui->rankSelectButton2->setEnabled(false);
    ui->rankSelectButton3->setEnabled(false);
    ui->rankselectButton4->setEnabled(false);

      this->rankAnswerQuestion(3);
}

//排位赛按钮4
void BrainStorm::on_rankselectButton4_clicked()
{
     ui->rankselectButton4->setStyleSheet("background-color:rgb9170，170，255");
    ui->rankSelectButton1->setEnabled(false);
    ui->rankSelectButton2->setEnabled(false);
    ui->rankSelectButton3->setEnabled(false);
    ui->rankselectButton4->setEnabled(false);

      this->rankAnswerQuestion(4);
}

//排位赛返回按钮（返回主菜单）
void BrainStorm::on_pushButton_6_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->mainMenu);
}
