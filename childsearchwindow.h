#ifndef CHILDSEARCHWINDOW_H
#define CHILDSEARCHWINDOW_H

#include <QWidget>

namespace Ui {
class ChildSearchWindow;
}

class ChildSearchWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChildSearchWindow(QWidget *parent = nullptr,
                               int *CountryNum = nullptr,
                               int *EventNum = nullptr);
    ~ChildSearchWindow();
    void hideSpinCountry();
    void hideLableCountry();

signals:
    void sendData(int CountryNum, int EventNum);

private slots:
    void on_pushButton_clicked();

private:
    Ui::ChildSearchWindow *ui;

};

#endif // CHILDSEARCHWINDOW_H
