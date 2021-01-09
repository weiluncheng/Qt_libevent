#include "registerr.h"
#include "ui_registerr.h"
#include <QMessageBox>
#include <stdio.h>

Registerr::Registerr(Communicate * com,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Registerr)
{
    ui->setupUi(this);

    this->m_com = com;


    connect(this->m_com,SIGNAL(rstResult(int)),this,SLOT(rstResult(int)));
}

Registerr::~Registerr()
{
    delete ui;
}

//立即注册
void Registerr::on_pushButton_clicked()
{

    QString userName = ui->lineEdit->text();
    QString passwd = ui->lineEdit_2->text();

    //发送注册信息
    QJsonObject json;
    json["cmd"] = REGISTER;
    json["userName"] = userName;
    json["passwd"] = passwd;

    //给服务器发送数据
    this->m_com->writeData(json);
}


 void Registerr::rstResult(int ret)
 {
     switch (ret)
     {
     case OK:
         QMessageBox::information(this,"注册","注册成功");
         break;
     case ERROR:
          QMessageBox::critical(this,"注册","注册失败，未知错误");
         break;
     case USEREXIST:
          QMessageBox::critical(this,"注册","注册失败，用户已存在");
         break;
     default:
         break;
     }
 }

//返回主界面
void Registerr::on_pushButton_2_clicked()
{
    close();
}
