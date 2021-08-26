#include "modifiedata.h"
#include "ui_modifiedata.h"

ModifieData::ModifieData(QWidget *parent,
                         int *CountryNum,
                         int *EventNum) :
    QWidget(parent),
    ui(new Ui::ModifieData)
{
    ui->setupUi(this);

    //根据传入的初始CountryNum和EventNum的值，来判断目前总共有多少个国家和项目可供选择
    ui->spinBox->setRange(1,*CountryNum);
    ui->spinBox_2->setRange(1,*EventNum);
    ui->spinBox_3->setRange(1, *CountryNum);
    ui->spinBox_4->setRange(1, *CountryNum);
    ui->spinBox_5->setRange(1, *CountryNum);
    ui->spinBox_Event->setRange(1, *EventNum);

}

ModifieData::~ModifieData()
{
    delete ui;
}

void ModifieData::hideLableAndSpineShort()
{
    //用于隐藏第四名第五名相关项目
    ui->label_4->setHidden(true);
    ui->label_5->setHidden(true);
    ui->spinBox_4->setHidden(true);
    ui->spinBox_5->setHidden(true);
}

void ModifieData::on_pushButton_clicked()
{
    int cnt[6];//[0]不用来存储国家数据,用来传出项目编号
    cnt[0]=ui->spinBox_Event->value();
    cnt[1]=ui->spinBox->value();
    cnt[2]=ui->spinBox_2->value();
    cnt[3]=ui->spinBox_3->value();
    cnt[4]=ui->spinBox_4->value();
    cnt[5]=ui->spinBox_5->value();
#ifdef QT_DEBUG
    for (int i = 0; i < 6; i++){//经测试能正常获取数据
        qDebug() << "cnt[" << i << "] = "
                 << cnt[i];
    }
#endif

    emit sendModifiedData(cnt);
    this->close();
}

