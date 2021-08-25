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
    //int index = 1;

    //建立初始表头
    QStringList header;
    header<<tr("CountryNum")<<tr("CountryScore")<<tr("MaleScore")<<tr("FemaleScore");
    ui->tableWidgetScoreBoard->setHorizontalHeaderLabels(header);

    qDebug() << "表头设置完毕";
    qDebug() << "表头为：" << header;

    //设置表格格式
    ui->tableWidgetScoreBoard->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetScoreBoard->verticalHeader()->hide();
    ui->tableWidgetScoreBoard->horizontalHeader()->setSortIndicatorShown(true);//This property holds whether the sort indicator is shown. By default, this property is false.
    ui->tableWidgetScoreBoard->horizontalHeader()->setSectionsClickable(true);//If clickable is true, the header will respond to single clicks.
    ui->tableWidgetScoreBoard->setEditTriggers(QAbstractItemView::NoEditTriggers);//初始主页表格禁用编辑
    //ui->tableWidgetScoreBoard.cell


    qDebug() << "表格样式设置完毕";

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
    //第 2 行到第 m + 1 行，每行 5 个整数 Cij，表示对应的项目 mi 取前五的国家编号 Cij，用/t分隔
    //第 m + 2 行到第 m + w + 1行， 每行 3 个整数 Cij, 表示对应的项目 wi 取前三的国家编号 Cij，用/t分隔

    //读取n,m,w并存储到对应变量中
    //lineStr = output.readLine();
    //lineList = lineStr.split('\t');
    //int CountrySum = lineList[0].toInt();
    //int MaleSum = lineList[1].toInt();
    //int FemaleSum = lineList[2].toInt();

    //读取各个小项目的具体奖牌分配情况
    while(!output.atEnd()){
        qDebug() << "循环中";
        lineStr = output.readLine();  //读取文件的一行
        qDebug() << lineStr;
        lineList = lineStr.split('\t');   //去除lineStr中的换行符作为表格数据
        //向表里面动态添加参数
        int rowCount = ui->tableWidgetScoreBoard->rowCount();
        ui->tableWidgetScoreBoard->insertRow(rowCount);
        for (int i = 0; i < 4; i++){
            //ui->tableWidgetScoreBoard->setItem(rowCount,i,new QTableWidgetItem(lineList[i]));   //使用QTable可以zheng'chang正常输出，使用我自己的则不行
            ui->tableWidgetScoreBoard->setItem(rowCount,i,new TableWidgetItem(lineList[i]));//自己的继承重载版本
            qDebug() << lineList[i];
        }
    }
    curFile.close();//关闭文件
    connect(ui->tableWidgetScoreBoard->horizontalHeader(),SIGNAL(sectionClicked(int)),
            this, SLOT(sortTableByCol(int)));

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

    //从文件中读取数据，第二步，遍历文件
    while(!output.atEnd()){
        qDebug() << "循环中";
        lineStr=output.readLine();  //读取文件的一行
        qDebug() << lineStr;
        lineList=lineStr.split('\t');   //去除lineStr中的换行符作为表格数据
        //向表里面动态添加参数
        int rowCount = ui->tableWidgetScoreBoard->rowCount();
        ui->tableWidgetScoreBoard->insertRow(rowCount);
        for (int i = 0; i < 4; i++){
            //ui->tableWidgetScoreBoard->setItem(rowCount,i,new QTableWidgetItem(lineList[i]));   //使用QTable可以zheng'chang正常输出，使用我自己的则不行
            ui->tableWidgetScoreBoard->setItem(rowCount,i,new TableWidgetItem(lineList[i]));//自己的继承重载版本
            qDebug() << lineList[i];
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
        //从tableWidgets中提取当前的排行榜数据并且输出
        QTextStream data(&fileToSave);
        QStringList strList;

        //如果取消注释，这一段代码可用于打印表头，可以在未来想要修改程序输出格式时候用到
        /*
        for( int c = 0; c < ui->tableWidgetScoreBoard->columnCount(); ++c )
        {
            strList <<ui->tableWidgetScoreBoard->horizontalHeaderItem(c)->data(Qt::DisplayRole).toString();
        }
        data << strList.join("\t") << "\n";*/

        for( int r = 0; r < ui->tableWidgetScoreBoard->rowCount(); ++r )
        {
            strList.clear();
            for( int c = 0; c < ui->tableWidgetScoreBoard->columnCount(); ++c )
            {   QTableWidgetItem* item = ui->tableWidgetScoreBoard->item(r,c);        //Load items
                if (!item || item->text().isEmpty())                        //Test if there is something at item(r,c)
                {
                    ui->tableWidgetScoreBoard->setItem(r,c,new QTableWidgetItem("0"));//IF there is nothing write 0
                }
                strList << ui->tableWidgetScoreBoard->item( r, c )->text();

            }
            data << strList.join( "\t" )+"\n";
        }
        statusBar()->showMessage(tr("File saved successfully."), 3000);
    }
    fileToSave.close();
}

