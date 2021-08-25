#include "tablewidgetitem.h"

TableWidgetItem::TableWidgetItem(const QString txt):
    QTableWidgetItem(txt)
{
    qDebug() << "TableWidgetItem constructing with value" << txt;
    this->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}
TableWidgetItem::~TableWidgetItem()
{
    qDebug() << "TableWidgetItem deconstruted";
}
