#ifndef TABLEWIDGETITEM_H
#define TABLEWIDGETITEM_H

#include <QTableWidgetItem>

class TableWidgetItem : public QTableWidgetItem
{
public:
    TableWidgetItem(const QString txt = "Error");
    //explicit TableWidgetItem(const QStringList list = QStringList("Error"));
    //using QTableWidgetItem::QTableWidgetItem;   //集成使用基类的构造函数
    ~TableWidgetItem();
    bool operator < (const QTableWidgetItem &other) const {
        QString str1 = text();
        QString str2 = other.text();
        return str1.toInt() < str2.toInt();
    }
};


#endif // TABLEWIDGETITEM_H
