#ifndef REGISTERR_H
#define REGISTERR_H

#include <QDialog>
#include <QJsonObject>
#include "communicate.h"
#include "common.h"

#include <QMessageBox>

namespace Ui {
class Registerr;
}

class Registerr : public QDialog
{
    Q_OBJECT

public:
    explicit Registerr(Communicate * com,QWidget *parent = 0);
    ~Registerr();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void rstResult(int);

private:
    Ui::Registerr *ui;

    Communicate * m_com;
};

#endif // REGISTERR_H
