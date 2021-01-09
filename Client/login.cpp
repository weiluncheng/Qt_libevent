#include "login.h"
#include "ui_widget.h"
#include "registerr.h"
#include <QMessageBox>

Widget::Widget(QWidget *parent) : QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->m_com = new Communicate();

    connect(this->m_com,SIGNAL(loginResult(QJsonObject )),this,SLOT(loginResult(QJsonObject)));
}

Widget::~Widget()
{
    delete ui;
}

//登录
void Widget::on_pushButton_clicked()
{
    QString userName = ui->lineEdit->text();
    QString passwd = ui->lineEdit_2->text();

    //把登录信息发送给服务器
    QJsonObject json;
    json["cmd"] = LOGIN;
    json["userName"] = userName;
    json["passwd"] = passwd;

    //给服务器发送数据
    this->m_com->writeData(json);
}

//处理登录信息
void Widget:: loginResult(QJsonObject json)
{
    int result = json["result"].toInt();
    switch(result)
    {
    case OK:
    {
        this->hide();
        BrainStorm * dlg = new BrainStorm(this->m_com,json);
        dlg->show();
        dlg->setAttribute(Qt::WA_DeleteOnClose);
    }
    break;
    case ERROR:
         QMessageBox::critical(this,"登录","登录失败，未知错误");
        break;
    case USERLOGIN:
         QMessageBox::critical(this,"登录","登录失败，不允许重复登录");
        break;
    case NAMEORPASSWD:
         QMessageBox::critical(this,"登录","登录失败，用户名字与密码不匹配");
        break;
    default:
        break;
    }
}

//注册
void Widget::on_pushButton_2_clicked()
{
    this->hide();
    Registerr reg(m_com);
    reg.exec();
    this->show();
}


