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
    explicit ChildSearchWindow(QWidget *parent = nullptr);
    ~ChildSearchWindow();

private:
    Ui::ChildSearchWindow *ui;
};

#endif // CHILDSEARCHWINDOW_H
