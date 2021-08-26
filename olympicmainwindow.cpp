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
#ifdef QT_DEBUG
    qDebug() << "表头设置完毕";
    qDebug() << "表头为：" << header;
#endif
#ifdef QT_DEBUG
    qDebug() << "defined QT_DEBUG";
#endif

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

#ifdef QT_DEBUG
    qDebug() << "表格样式设置完毕";
#endif
    openDefault();
    connect(ui->tableWidgetScoreBoard->horizontalHeader(),SIGNAL(sectionClicked(int)),
            this, SLOT(sortTableByCol(int)));//用于排序的信号连接

}

void OlympicMainWindow::sortTableByCol(int col)
{
    static bool m_bUpOrDown = false;//自定义的一个bool变量，表示升序还是降序,初值为false
    if(m_bUpOrDown)	{
#ifdef QT_DEBUG
        qDebug() << "正在尝试升序排列数据";
#endif
        ui->tableWidgetScoreBoard->sortItems(col, Qt::AscendingOrder);
        m_bUpOrDown=false;
    }
    else {
#ifdef QT_DEBUG
        qDebug() << "正在尝试降序排列数据";
#endif
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
#ifdef QT_DEBUG
        qDebug() << "读取文件失败";
        qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
        qDebug()<<"current currentPath: "<<QDir::currentPath();
#endif

        QString prompt = "读取文件失败\n";
        QMessageBox msgBox(QMessageBox::Icon::Information,     //图标
                           "文件丢失",                        //标题
                           prompt,                      //内容
                           QMessageBox::Ok,                 //按钮
                           this);                           //停靠父窗口
        //显示消息窗口
        msgBox.exec();

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

    /*
    //从文件中读取数据，第一步，初始化
    //QFile f("DefaultInput.txt");
    curFile.setFileName("DefaultInput.txt");
    if (! curFile.open(QIODevice::ReadOnly | QIODevice::Text)){
#ifdef QT_DEBUG
        qDebug() << "读取文件失败";
        qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
        qDebug()<<"current currentPath: "<<QDir::currentPath();
#endif
    }
    */

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
#ifdef QT_DEBUG
        qDebug() << lineList;
#endif
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
    //猜测验证成功，就是覆盖保存，不是追加保存
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

    int i = 0;
    QStringList lineList;   //整行字符串分割处理为单个字符串存入到表格中
    //根据计算结果填充主页榜单表格,共CountrySum行，4列，4列分别为：国家编号，国家总分，男子总分，女子总分
    //更新方法为即使用户只修改了一个项目的数据（甚至没有修改），也强制对整个表格进行更新
    for (i = 1; i <= CountrySum; i++){
        //i+1即为对应行编号
        //将计算结果包裹到QStringList lineList中
        lineList={QString::number(i), QString::number(CountryScore[i]),
                  QString::number(CountryScoreMale[i]),QString::number(CountryScoreFemale[i])};
#ifdef QT_DEBUG
        qDebug() << lineList;
#endif
        //用lineList填充表格
        for (int col = 0; col < 4; col++){
            ui->tableWidgetScoreBoard->setItem(i-1, col, new TableWidgetItem(lineList[col]));
        }
    }

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
#ifdef QT_DEBUG
        qDebug() << "读取文件失败";
        qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
        qDebug()<<"current currentPath: "<<QDir::currentPath();
#endif
        QString prompt = "没有在程序所在目录下找到DefaultInput.txt\n";
        prompt += "您可以在打开程序后手动选择输入文件\n";
        QMessageBox msgBox(QMessageBox::Icon::Information,     //图标
                           "文件丢失",                        //标题
                           prompt,                      //内容
                           QMessageBox::Ok,                 //按钮
                           this);                           //停靠父窗口
        //显示消息窗口
        msgBox.exec();
        return;//退出openDefault函数
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
#ifdef QT_DEBUG
        qDebug() << lineList;
#endif
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

    //初始化国家分数
    for (i=0;i<300;i++){
        CountryScore[i] = 0;
        CountryScoreFemale[i] = 0;
        CountryScoreMale[i] = 0;
    }

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

void OlympicMainWindow::on_actionByCountryNum_triggered()
{
    //todo:弹出一个新窗口，用于读入用户输入的国家编号和想要查询的奥运项目
    //读取编号后，访问对应国家的对应项目相信获奖信息
    //本函数未完成，有如下bug/todo
    //1. msgBox太小，不能完整输出结果（已解决，是if逻辑判断没进去，我以为size会是6和4，但计算出来是7和5）
    //2. 还没有界面用于读入用户输入
    //3. 只做了取前5项目的文本逻辑（已解决，已补充前3逻辑）

    int *CountryNum = new int;
    int *EventNum = new int;
    *CountryNum = CountrySum;
    *EventNum = EventSum;
    CSW = new ChildSearchWindow(nullptr, CountryNum, EventNum);//动态申请
    CSW->show();
    connect(CSW, SIGNAL(sendData(int,int)),
            this, SLOT(receiveData(int,int)));//获取用户输入
    delete CountryNum;
    delete EventNum;
}


void OlympicMainWindow::receiveData(int CountryNum, int EventNum)
{
    int SingleScore = 0;//单项项目所获总分

    //警告，不要轻易取消下面修改CountryNum和EventNum的注释，仅用于临时debug：
    //CountryNum = 5;
    //EventNum = 2;

    //判断项目类型，初始化提示语
    int size = EventList[EventNum].size();
#ifdef QT_DEBUG
    qDebug() << size;
#endif
#ifdef QT_DEBUG
    //if (1 == EventNum)  //不明白是不是Qt的Bug，如果EventNum为1，那么size会是7而不是6，但是如果直接访问[1][7]则非法内存
        //size--;         //作为折衷方案，做一个特殊判断
    //在release之后发现存在非法内存访问，是这里又出错了吗？
#endif
    //qDebug() << EventList[1].size();
    //qDebug() << EventList[2].size();
    //qDebug() <<EventList[1][7];

    QString prompt = "国家 " + QString::number(CountryNum) + " 在项目 "
                    + EventList[EventNum][0] + "(编号 " + QString::number(EventNum)
            + " )中获得了：";

    //统计成绩，完成提示语
    if (6 == size){//取前5的项目

        if (CountryNum == EventList[EventNum][1].toInt()){
            //输出名次
            prompt += "金牌(7pts)；";
            //累计算分
            SingleScore += 7;
        }
        if (CountryNum == EventList[EventNum][2].toInt()){
            //输出名次
            prompt += "银牌(5pts)；";
            //累计算分
            SingleScore += 5;
        }
        if (CountryNum == EventList[EventNum][3].toInt()){
            //输出名次
            prompt += "铜牌(3pts)；";
            //累计算分
            SingleScore += 3;
        }
        if (CountryNum == EventList[EventNum][4].toInt()){
            //输出名次
            prompt += "第4名(2pts)；";
            //累计算分
            SingleScore += 2;
        }
        if (CountryNum == EventList[EventNum][5].toInt()){
            //输出名次
            prompt += "第5名(1pts)；";
            //累计算分
            SingleScore += 1;
        }
    }
    else if (4 == size){//取前3的项目
        if (CountryNum == EventList[EventNum][1].toInt()){
            //输出名次
            prompt += "金牌(5pts)；";
            //累计算分
            SingleScore += 5;
        }
        if (CountryNum == EventList[EventNum][2].toInt()){
            //输出名次
            prompt += "银牌(3pts)；";
            //累计算分
            SingleScore += 3;
        }
        if (CountryNum == EventList[EventNum][3].toInt()){
            //输出名次
            prompt += "铜牌(2pts)；";
            //累计算分
            SingleScore += 2;
        }

    }

    prompt += "的成绩，总共在本项目中斩获 ";
    prompt += QString::number(SingleScore);
    prompt += " 分";
#ifdef QT_DEBUG
    qDebug() << "SingleScore for Country" << CountryNum
             << "is " << SingleScore;
    qDebug() << prompt;
#endif
    QMessageBox msgBox(QMessageBox::Icon::Information,     //图标
                       "查询成功",                        //标题
                       prompt,                      //内容
                       QMessageBox::Ok,                 //按钮
                       this);                           //停靠父窗口
    //显示消息窗口
    msgBox.exec();

}

void OlympicMainWindow::receiveDataEventNumOnly(int CountryNum, int EventNum)
{
    QString prompt = "";
    QString EventName = EventList[EventNum][0];
    int size = EventList[EventNum].size();
#ifdef  QT_DEBUG
    //不知为何release版本有未知的非法内存访问，考虑怀疑这里
    //if (1 == EventNum)  //不明白是不是Qt的Bug，如果EventNum为1，那么size会是7而不是6，但是如果直接访问[1][7]则非法内存
        //size--;         //作为折衷方案，做一个特殊判断
#endif
    //qDebug() << EventList[1].size();
    //qDebug() << EventList[2].size();
    //qDebug() <<EventList[1][7];
    prompt += EventName;
    prompt += " 项目(编号 ";
    prompt += QString::number(EventNum);
    prompt += " )中各个国家的获奖情况(按名次从高到低，以编号形式)为：\n";
    prompt += EventList[EventNum][1] + "\n";
    prompt += EventList[EventNum][2] + "\n";
    prompt += EventList[EventNum][3] + "\n";
    if (6 == size){//if(1 == EventNum) size-- 就是为了解决这里的潜在bug
        prompt += EventList[EventNum][4] + "\n";
        prompt += EventList[EventNum][5] + "\n";
    }

    QMessageBox msgBox(QMessageBox::Icon::Information,     //图标
                       "查询成功",                        //标题
                       prompt,                      //内容
                       QMessageBox::Ok,                 //按钮
                       this);                           //停靠父窗口
    //显示消息窗口
    msgBox.exec();
}

void OlympicMainWindow::receiveEventToModified(int CountryNum, int EventNum)
{

    //只能使用EventNum
    MD = new ModifieData();
    if (EventNum > MaleSum){//是女子项目，只需要修改三个值
        MD->hideLableAndSpineShort();
    }
    MD->show();

}

void OlympicMainWindow::receiveDataToModified(int* cnt)
{
    //故技重施做一次窗口间的信息传递即可

    //仅供调试
    //cnt[6]; //[0]不使用
    //for (int i = 0; i< 6; i++){
    //    cnt[i] = 0;
    //}
    int EventNum = cnt[0];
    //cnt[1]=1;cnt[2]=2;cnt[3]=3;cnt[4]=4;cnt[5]=5;//for debug purpose
    QString EventName = EventList[EventNum][0];
    QStringList updated;
#ifdef QT_DEBUG
    for (int i =0; i < 6; i++){
        qDebug() << "cnt[" << i << "]=" << cnt[i];
    }
#endif
    if (EventNum <= MaleSum){
        updated = {EventName,
                   QString::number(cnt[1]),
                   QString::number(cnt[2]),
                   QString::number(cnt[3]),
                   QString::number(cnt[4]),
                   QString::number(cnt[5])
                  };
    }
    else{
        updated = {EventName,
                   QString::number(cnt[1]),
                   QString::number(cnt[2]),
                   QString::number(cnt[3])
                  };
    }
#ifdef QT_DEBUG
    qDebug() << "updated = " << updated;
#endif
    EventList[EventNum] = updated;
    on_actionCalculate_triggered();

    QMessageBox msgBox;
    msgBox.setText(tr("数据已被修改"));
    msgBox.setInformativeText("是否保存数据到文件中?如果选择不保存，数据会临时保存在程序中，稍后您可以通过文件选项卡中的保存按钮来保存数据");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Save:
        // Save was clicked
        on_actionSave_triggered();
        break;
    case QMessageBox::Discard:
        // Don't Save was clicked
        break;
    default:
        // should never be reached
        break;
    }
}

void OlympicMainWindow::on_actionAbout_triggered()
{
    QString prompt = "输入输出数据格式定义:\n";
    prompt += "第 1 行 3个整数国家编号 n,男子项目总数 m,女子项目总数 w，用/t分隔\n";
    prompt += "第 2 行到第 m + 1 行，每行开头一个对应的项目名称，之后是 5 个整数 Cij，表示对应的项目 mi 取前五的国家编号 Cij，用/t分隔\n";
    prompt += "第 m + 2 行到第 m + w + 1行，每行开头一个对应的项目名称，之后是 3 个整数 Cij, 表示对应的项目 wi 取前三的国家编号 Cij，用/t分隔\n";
    prompt += "规定 n >= 5 && n <= 300, m >= 0, w >= 0;\n";
    prompt += "规定男子项目全部取前五名积分，女子项目全部取前三名积分；\n";
    prompt += "使用备注：\n";
    prompt += "点击主页表头，可以实现按照国家编号、国家总分、男子总分或女子总分的升降序排列\n";
    prompt += "使用数据修改工具时，对于只取前三名积分的项目，输入的第四第五名数据不会被收录\n";

    QMessageBox msgBox(QMessageBox::Icon::Information,     //图标
                       "关于",                        //标题
                       prompt,                      //内容
                       QMessageBox::Ok,                 //按钮
                       this);                           //停靠父窗口
    //显示消息窗口
    msgBox.exec();
}


void OlympicMainWindow::on_actionByEventNum_triggered()
{
    int *CountryNum = new int;
    int *EventNum = new int;
    *CountryNum = CountrySum;
    *EventNum = EventSum;

    CSW = new ChildSearchWindow(nullptr, CountryNum, EventNum);
    CSW->hideSpinCountry();
    CSW->hideLableCountry();
    CSW->show();
    connect(CSW, SIGNAL(sendData(int,int)),
            this, SLOT(receiveDataEventNumOnly(int,int)));//获取用户输入
    delete CountryNum;
    delete EventNum;
}


void OlympicMainWindow::on_actionInput_Modifie_Data_triggered()
{
    //1. 打开一个含有EventNum选项的窗口，首先让用户选择要修改成绩的项目（完成）
    //2. 给出一个对话框，要求用户修改名次（进展中）
    //3. 将用户数据更新修改到EventList中
    //4. 调用on_action_Calculate()来更新table数据
    int *CountryNum = new int;
    int *EventNum = new int;
    *CountryNum = CountrySum;
    *EventNum = EventSum;

    /*
    CSW = new ChildSearchWindow(nullptr, CountryNum, EventNum);
    CSW->hideSpinCountry();
    CSW->hideLableCountry();
    CSW->show();
    connect(CSW, SIGNAL(sendData(int,int)),
            this, SLOT(receiveEventToModified(int,int)));//获取用户输入
    */

    MD = new ModifieData(nullptr, CountryNum, EventNum);
    MD->show();
    connect(MD, SIGNAL(sendModifiedData(int*)),
            this, SLOT(receiveDataToModified(int*)));//获取用户输入


}

