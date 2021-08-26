#ifndef MODIFIEDATA_H
#define MODIFIEDATA_H

#include <QWidget>

namespace Ui {
class ModifieData;
}

class ModifieData : public QWidget
{
    Q_OBJECT

public:
    explicit ModifieData(QWidget *parent = nullptr,
                         int *CountryNum = nullptr,
                         int *EventNum = nullptr);
    ~ModifieData();
    void hideLableAndSpineShort();

signals:
    void sendModifiedData(int* cnt);

private slots:
    void on_pushButton_clicked();

private:
    Ui::ModifieData *ui;
};

#endif // MODIFIEDATA_H
