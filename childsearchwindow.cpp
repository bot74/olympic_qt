#include "childsearchwindow.h"
#include "ui_childsearchwindow.h"

ChildSearchWindow::ChildSearchWindow(QWidget *parent,
                                     int *CountryNum,
                                     int *EventNum) :
    QWidget(parent),
    ui(new Ui::ChildSearchWindow)
{
    ui->setupUi(this);

    //根据传入的初始CountryNum和EventNum的值，来判断目前总共有多少个国家和项目可供选择
    ui->spinBoxCountry->setRange(1,*CountryNum);
    ui->spinBoxEvent->setRange(1,*EventNum);

}

ChildSearchWindow::~ChildSearchWindow()
{
    delete ui;
}

void ChildSearchWindow::on_pushButton_clicked()
{
    int CNum = ui->spinBoxCountry->value();
    int ENum = ui->spinBoxEvent->value();
    emit sendData(CNum, ENum);
    this->close();
}

