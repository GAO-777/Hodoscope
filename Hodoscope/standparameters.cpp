#include "standparameters.h"
#include "ui_standparameters.h"

StandParameters::StandParameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StandParameters)
{
    ui->setupUi(this);

/*================================================================================================================================*\
 * ----------------------- Определение таблицы слоев -----------------------------------------------------------------------------*
\*================================================================================================================================*/
    StandItemModel = new QStandardItemModel();
    ui->LayersTable->setModel(StandItemModel);
    StandItemModel->setColumnCount(2);
    StandItemModel->setHeaderData(0,Qt::Horizontal,QStringLiteral("Кол-во счетчиков"));
    StandItemModel->setHeaderData(1,Qt::Horizontal,QStringLiteral("Шаг между счетчиками"));

    ui->LayersTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->LayersTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->LayersTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->LayersTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);

    ui->LayersTable->setModel(StandItemModel);


}

StandParameters::~StandParameters()
{
    delete ui;
}

void StandParameters::on_Apply_pb_clicked()
{
    this->close();
}

void StandParameters::SaveParameters()
{
    int NumRows = StandItemModel->rowCount();

    for(int i=0;i<NumRows;i++){
        QStandardItem * numDetectors = StandItemModel->item(i, 0);
        QStandardItem * step = StandItemModel->item(i, 1);

    }
}


void StandParameters::on_CreateLineLayers_pb_clicked()
{
    for(int i=0;i<ui->NumLayers_sb->value();i++){
        QStandardItem *standItem = new QStandardItem();
        StandItemModel->setItem(i,0,standItem);

    }
}

