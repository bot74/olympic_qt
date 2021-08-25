#include "childsearchwindow.h"
#include "ui_childsearchwindow.h"

ChildSearchWindow::ChildSearchWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChildSearchWindow)
{
    ui->setupUi(this);
}

ChildSearchWindow::~ChildSearchWindow()
{
    delete ui;
}
