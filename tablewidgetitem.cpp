#include "tablewidgetitem.h"

TableWidgetItem::TableWidgetItem(const QString txt):
    QTableWidgetItem(txt)
{
#ifdef QT_DEBUG
    qDebug() << "TableWidgetItem constructing with value" << txt;
#endif
    this->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}
TableWidgetItem::~TableWidgetItem()
{
#ifdef QT_DEBUG
    qDebug() << "TableWidgetItem deconstruted";
#endif
}
