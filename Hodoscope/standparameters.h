#ifndef STANDPARAMETERS_H
#define STANDPARAMETERS_H

#include <QWidget>
#include <QStandardItemModel>
#include "Tools.h"
namespace Ui {
class StandParameters;
}

class StandParameters : public QWidget
{
    Q_OBJECT

public:
    explicit StandParameters(QWidget *parent = nullptr);
    ~StandParameters();
    QWidget* Parent;
    Stand_Info* StandInfo;
    QStandardItemModel* StandItemModel;

private slots:
    void on_Apply_pb_clicked();
    void SaveParameters();
    void on_CreateLineLayers_pb_clicked();

private:
    Ui::StandParameters *ui;
};

#endif // STANDPARAMETERS_H
