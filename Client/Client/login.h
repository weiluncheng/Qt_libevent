#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include "communicate.h"
#include <QJsonObject>

#include "brainstorm.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    //处理登录信息
    void loginResult(QJsonObject json);
private:
    Ui::Widget *ui;

    //通信类指针，负责和服务器通信
    Communicate * m_com;
};

#endif // WIDGET_H
