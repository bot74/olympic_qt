#include "olympicmainwindow.h"
#include "ui_olympicmainwindow.h"
#include "tablewidgetitem.h"
#include <QFile>
#include <QTextStream>
#include <QHeaderView>//用于隐藏表格第一列小数字
#include <QFileDialog>    //用于读取用户选择的文件
#include <QMessageBox>
//下面是输出Debug信息时用到的头文件
#include <QDebug>
#include <QDir>
OlympicMainWindow::OlympicMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OlympicMainWindow)
{
    ui->setupUi(this);

    //ui->tableWidgetScoreBoard->setColumnCount(4);动态调整列数的方法是setColumnCount(int)

    //建立初始表头
    QStringList header;
    header<<tr("CountryNum")<<tr("CountryScore")<<tr("MaleScore")<<tr("FemaleScore");
    ui->tableWidgetScoreBoard->setHorizontalHeaderLabels(header);

    /*
    header<<tr("我是动态第五列");
    ui->tableWidgetScoreBoard->setColumnCount(5);
    ui->tableWidgetScoreBoard->setHorizontalHeaderLabels(header);//证明了可以在设置完label以后再次重新修改
    */

    qDebug() << "表头设置完毕";
    qDebug() << "表头为：" << header;

    //设置表格格式
    ui->tableWidgetScoreBoard->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableWidgetScoreBoard->verticalHeader()->hide();
    //ui->tableWidgetScoreBoard->horizontalHeader()->setSortIndicatorShown(true);//This property holds whether the sort indicator is shown. By default, this property is false.
    ui->tableWidgetScoreBoard->horizontalHeader()->setSectionsClickable(true);//If clickable is true, the header will respond to single clicks.
    ui->tableWidgetScoreBoard->setEditTriggers(QAbstractItemView::NoEditTriggers);//初始主页表格禁用编辑
    //ui->tableWidgetScoreBoard.cell
    for (int i = 4; i < 401; i++){//隐藏多余列
        ui->tableWidgetScoreBoard->setColumnHidden(i, true);
    }


    qDebug() << "表格样式设置完毕";

    openDefault();
    connect(ui->tableWidgetScoreBoard->horizontalHeader(),SIGNAL(sectionClicked(int)),
            this, SLOT(sortTableByCol(int)));//用于排序的信号连接

}

void OlympicMainWindow::sortTableByCol(int col)
{
    static bool m_bUpOrDown = false;//自定义的一个bool变量，表示升序还是降序,初值为false
    if(m_bUpOrDown)	{
        qDebug() << "正在尝试升序排列数据";
        ui->tableWidgetScoreBoard->sortItems(col, Qt::AscendingOrder);
        m_bUpOrDown=false;
    }
    else {
        qDebug() << "正在尝试降序排列数据";
        ui->tableWidgetScoreBoard->sortItems(col, Qt::DescendingOrder);
        m_bUpOrDown=true;
    }
    return;
}


OlympicMainWindow::~OlympicMainWindow()
{
    delete ui;
}


void OlympicMainWindow::on_actionOpen_triggered()
{
    //QString file_open;
    curFile.setFileName(QFileDialog::getOpenFileName(this,
                                                     tr("Open Input Text"), "/", tr("Text files (*.txt)")));
    if (! curFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "读取文件失败";
        qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
        qDebug()<<"current currentPath: "<<QDir::currentPath();
    }
    QTextStream output(&curFile);
    QHash<QString, QString> varHash;
    QString lineStr;    //文件的每一行的字符串
    QStringList lineList;   //整行字符串分割处理为单个字符串存入到表格中
    varHash.clear();
    lineList.clear();

    //附加步骤，清空根据之前文件数据创建的表格数据,并重置行数
    ui->tableWidgetScoreBoard->clearContents();
    ui->tableWidgetScoreBoard->setRowCount(0);

    //从文件中读取数据，第一步，初始化
    //QFile f("DefaultInput.txt");
    curFile.setFileName("DefaultInput.txt");
    if (! curFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "读取文件失败";
        qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
        qDebug()<<"current currentPath: "<<QDir::currentPath();
    }

    //从文件中读取数据，第二步，遍历文件
    //定义文件格式如下：
    //第 1 行 3个整数国家编号 n,男子项目总数 m,女子项目总数 w，用/t分隔
    //第 2 行到第 m + 1 行，每行开头一个对应的项目名称，之后是 5 个整数 Cij，表示对应的项目 mi 取前五的国家编号 Cij，用/t分隔
    //第 m + 2 行到第 m + w + 1行，每行开头一个对应的项目名称，之后是 3 个整数 Cij, 表示对应的项目 wi 取前三的国家编号 Cij，用/t分隔
    //规定 n >= 5 && n <= 300, m >= 0, w >= 0

    //读取n,m,w并存储到对应变量中
    lineStr = output.readLine();
    lineList = lineStr.split('\t');

    CountrySum = lineList[0].toInt();
    MaleSum = lineList[1].toInt();
    FemaleSum = lineList[2].toInt();
    EventSum = MaleSum + FemaleSum;
    int i = 0;
    //QStringList EventList[400];//最多读取400行，放弃tempLine[0]不使用，也就是最多存储399个子项目数据
    //int CountryScore[300];

    //初始化国家分数
    for (i=0;i<300;i++){
        CountryScore[i] = 0;
        CountryScoreFemale[i] = 0;
        CountryScoreMale[i] = 0;
    }

    //根据国家总数添加对应行
    for (i = 1; i <= CountrySum; i++){
        int rowCount = ui->tableWidgetScoreBoard->rowCount();
        ui->tableWidgetScoreBoard->insertRow(rowCount);
    }

    //读取小项目奖牌数据到EventList以及header中
    for (i = 1; i <= EventSum ; i++){
        lineStr = output.readLine();
        EventList[i] = lineStr.split('\t');
        header << EventList[i][0];  //每一行开头第一个元素是项目名称，加入到header表头中
    }

    //调用calculateBackground()来计算国家分数、男女团体分数
    CalculateBackground();

    //根据计算结果填充主页榜单表格,共CountrySum行，4列，4列分别为：国家编号，国家总分，男子总分，女子总分
    for (i = 1; i <= CountrySum; i++){
        //i+1即为对应行编号
        //将计算结果包裹到QStringList lineList中
        lineList={QString::number(i), QString::number(CountryScore[i]),
                  QString::number(CountryScoreMale[i]),QString::number(CountryScoreFemale[i])};

        qDebug() << lineList;
        //用lineList填充表格
        for (int col = 0; col < 4; col++){
            ui->tableWidgetScoreBoard->setItem(i-1, col, new TableWidgetItem(lineList[col]));
        }
    }
    curFile.close();//关闭文件
}


void OlympicMainWindow::on_actionSave_triggered()
{
    QFile fileToSave;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    "/UntitledOlympicsScoreBoardOutput.txt",
                                                    tr("Texts (*.txt)"));
    fileToSave.setFileName(fileName);
    //对于QIODevice::truncate,官网文档描述为：
    //If possible, the device is truncated before it is opened. All earlier contents of the device are lost.
    //我的猜测是用于覆盖文件，也就是说如果文件原来存在，加上这句话就会覆盖掉原始内容。
    if (!fileToSave.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, tr("Error"),tr("Save Failed"),QMessageBox::Ok);
        return;
    }
    else{
        QTextStream data(&fileToSave);
        QStringList strList;
        //将EventList重新组合为输出
        //定义文件格式如下：
        //第 1 行 3个整数国家编号 n,男子项目总数 m,女子项目总数 w，用/t分隔
        //第 2 行到第 m + 1 行，每行开头一个对应的项目名称，之后是 5 个整数 Cij，表示对应的项目 mi 取前五的国家编号 Cij，用/t分隔
        //第 m + 2 行到第 m + w + 1行，每行开头一个对应的项目名称，之后是 3 个整数 Cij, 表示对应的项目 wi 取前三的国家编号 Cij，用/t分隔
        //规定 n >= 5 && n <= 300, m >= 0, w >= 0

        //组合第一行
        strList << QString::number(CountrySum) << QString::number(MaleSum) << QString::number(FemaleSum);
        data << strList.join("\t") << "\n";

        //组合剩余行
        for (int i = 1; i <= EventSum; i++){//按照头文件定义，EventList[0]没有存放实际数据
            data << EventList[i].join("\t") << "\n";
        }
        statusBar()->showMessage(tr("File saved successfully."), 3000);
    }
    fileToSave.close();
}


void OlympicMainWindow::on_actionCalculate_triggered()
{
    CalculateBackground();//GUI计算按钮按下的话，除了后台计算，可能还要做一些用户方面的交互
    QMessageBox msgBox(QMessageBox::Icon::Information,     //图标
                           "成功",                        //标题
                           "已更新国家得分情况",                      //内容
                           QMessageBox::Ok,                 //按钮
                           this);                           //停靠父窗口
        //显示消息窗口
        msgBox.exec();
    return;
}


void OlympicMainWindow::openDefault()
{
    //从文件中读取数据，第一步，初始化
    //QFile f("DefaultInput.txt");
    curFile.setFileName("DefaultInput.txt");
    if (! curFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "读取文件失败";
        qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
        qDebug()<<"current currentPath: "<<QDir::currentPath();
    }
    QTextStream output(&curFile);
    QHash<QString, QString> varHash;
    QString lineStr;    //文件的每一行的字符串
    QStringList lineList;   //整行字符串分割处理为单个字符串存入到表格中
    varHash.clear();
    lineList.clear();

    //从文件中读取数据，第二步，遍历文件
    //定义文件格式如下：
    //第 1 行 3个整数国家编号 n,男子项目总数 m,女子项目总数 w，用/t分隔
    //第 2 行到第 m + 1 行，每行开头一个对应的项目名称，之后是 5 个整数 Cij，表示对应的项目 mi 取前五的国家编号 Cij，用/t分隔
    //第 m + 2 行到第 m + w + 1行，每行开头一个对应的项目名称，之后是 3 个整数 Cij, 表示对应的项目 wi 取前三的国家编号 Cij，用/t分隔
    //规定 n >= 5 && n <= 300, m >= 0, w >= 0

    //读取n,m,w并存储到对应变量中
    lineStr = output.readLine();
    lineList = lineStr.split('\t');

    CountrySum = lineList[0].toInt();
    MaleSum = lineList[1].toInt();
    FemaleSum = lineList[2].toInt();
    EventSum = MaleSum + FemaleSum;
    int i = 0;
    //QStringList EventList[400];//最多读取400行，放弃tempLine[0]不使用，也就是最多存储399个子项目数据
    //int CountryScore[300];

    //初始化国家分数
    for (i=0;i<300;i++){
        CountryScore[i] = 0;
        CountryScoreFemale[i] = 0;
        CountryScoreMale[i] = 0;
    }

    //根据国家总数添加对应行
    for (i = 1; i <= CountrySum; i++){
        int rowCount = ui->tableWidgetScoreBoard->rowCount();
        ui->tableWidgetScoreBoard->insertRow(rowCount);
    }

    //读取小项目奖牌数据到EventList以及header中
    for (i = 1; i <= EventSum ; i++){
        lineStr = output.readLine();
        EventList[i] = lineStr.split('\t');
        header << EventList[i][0];  //每一行开头第一个元素是项目名称，加入到header表头中
    }

    //调用calculateBacckground()来计算国家分数、男女团体分数
    CalculateBackground();

    //根据计算结果填充主页榜单表格,共CountrySum行，4列，4列分别为：国家编号，国家总分，男子总分，女子总分
    for (i = 1; i <= CountrySum; i++){
        //i+1即为对应行编号
        //将计算结果包裹到QStringList lineList中
        lineList={QString::number(i), QString::number(CountryScore[i]),
                  QString::number(CountryScoreMale[i]),QString::number(CountryScoreFemale[i])};

        qDebug() << lineList;
        //用lineList填充表格
        for (int col = 0; col < 4; col++){
            ui->tableWidgetScoreBoard->setItem(i-1, col, new TableWidgetItem(lineList[col]));
        }
    }

    curFile.close();//关闭文件
}

void OlympicMainWindow::CalculateBackground()
{
    int i = 0;
    for (i = 1; i <= MaleSum; i++){//统计男子团队数据
        //假定男子项目均为取前五积分，规则为7、5、3、2、1
        int num = EventList[i][1].toInt();//获取第1名名次所属国家的编号
        CountryScoreMale[num] += 7;
        num = EventList[i][2].toInt();//获取第2名名次所属国家的编号
        CountryScoreMale[num] += 5;
        num = EventList[i][3].toInt();//获取第3名名次所属国家的编号
        CountryScoreMale[num] += 3;
        num = EventList[i][4].toInt();//获取第3名名次所属国家的编号
        CountryScoreMale[num] += 2;
        num = EventList[i][5].toInt();//获取第5名名次所属国家的编号
        CountryScoreMale[num] += 1;
    }
    for (; i <= EventSum; i++){  //统计女子团队数据
        //假定女子项目均为取前三积分，规则为5、3、2
        int num = EventList[i][1].toInt();//获取第1名名次所属国家的编号
        CountryScoreFemale[num] += 5;
        num = EventList[i][2].toInt();//获取第2名名次所属国家的编号
        CountryScoreFemale[num] += 3;
        num = EventList[i][3].toInt();//获取第3名名次所属国家的编号
        CountryScoreFemale[num] += 2;
    }
    for (i = 1; i <= CountrySum; i++){//统计国家数据
        CountryScore[i] = CountryScoreFemale[i] + CountryScoreMale[i];
    }
    return;
}
