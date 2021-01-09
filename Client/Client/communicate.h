#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include "common.h"

class Communicate : public QObject
{
    Q_OBJECT
public:
    explicit Communicate(QObject *parent = 0);

    void writeData(const QJsonObject & json);

signals:
    void rstResult(int);

    void loginResult(QJsonObject json);

    void recvSigleQuestion(QJsonObject json);

    void Rank(QJsonObject json);

public slots:
    void readData();

private:
    QTcpSocket  s;

};

#endif // COMMUNICATE_H
