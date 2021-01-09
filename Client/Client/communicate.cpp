#include "communicate.h"
#include <QHostAddress>

Communicate::Communicate(QObject *parent) :QObject(parent)
{
    s.connectToHost(QHostAddress( "192.168.12.13"),8888);


    connect(&s,SIGNAL(readyRead()),this,SLOT(readData()));
}



void Communicate::readData()
{
    QByteArray data;
    while(s.bytesAvailable())
    {
        data += s.readAll();
    }

    //数据解析
    QJsonDocument dt = QJsonDocument::fromJson(data);
    if(dt.isNull())
    {
        return;
    }
    QJsonObject root =  dt.object();


    //具体的逻辑处理
    int cmd = root["cmd"].toInt();
    switch (cmd) {
    case REGISTER:
        emit rstResult(root["result"].toInt());
        break;
    case LOGIN:
        emit loginResult(root);
        break;
    case SINGLE_GETQUESTION:
        emit this->recvSigleQuestion(root["question"].toObject());
        break;
    case RANK:
        emit this->Rank(root);
        break;
    case ANSWER:
        emit this->Rank(root);
        break;
    case RANKRESULT:
        emit this->Rank(root);
        break;
    default:
        break;
    }

}

void Communicate::writeData(const QJsonObject &json)
{
    QJsonDocument d(json);
    QByteArray sendData = d.toJson();

    int len = sendData.size();

    //发送数据长度
    s.write((char *)&len,sizeof(len));
    //发送数据
    s.write(sendData);

}
