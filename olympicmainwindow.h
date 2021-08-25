#ifndef OLYMPICMAINWINDOW_H
#define OLYMPICMAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include "childsearchwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class OlympicMainWindow; }
QT_END_NAMESPACE

class OlympicMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    OlympicMainWindow(QWidget *parent = nullptr);
    ~OlympicMainWindow();

private:
    Ui::OlympicMainWindow *ui;
    QFile curFile;
    int CountryScore[300];//[0]不使用
    int CountryScoreMale[300];//[0]不使用
    int CountryScoreFemale[300];//[0]不使用
    int CountrySum;
    int MaleSum;
    int FemaleSum;
    int EventSum;
    QStringList EventList[400];//EventList[0]不使用
    QStringList header;//用于存放水平表头数据
    void saveFile(const QString &name);
    void CalculateBackground();//后台计算
    void openDefault();
    ChildSearchWindow* CSW; //指向子查询窗口的指针

signals:
    void sectionClicked(int);
public slots:
    void sortTableByCol(int col);
    void receiveData(int CountryNum, int EventNum);
    void receiveDataEventNumOnly(int CountryNum, int EventNum);
private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionCalculate_triggered();
    void on_actionByCountryNum_triggered();
    void on_actionAbout_triggered();
    void on_actionByEventNum_triggered();
};
#endif // OLYMPICMAINWINDOW_H
