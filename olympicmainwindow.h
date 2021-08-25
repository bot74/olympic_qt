#ifndef OLYMPICMAINWINDOW_H
#define OLYMPICMAINWINDOW_H

#include <QMainWindow>
#include <QFile>

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
    void saveFile(const QString &name);

signals:
    void sectionClicked(int);
public slots:
    void sortTableByCol(int col);
private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
};
#endif // OLYMPICMAINWINDOW_H
