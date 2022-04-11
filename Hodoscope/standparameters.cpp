#include "standparameters.h"
#include "ui_standparameters.h"
#include "mainwindow.h"

StandParameters::StandParameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StandParameters)
{
    ui->setupUi(this);
    Parent = parent;
    MainWindow* mainWindow = static_cast<MainWindow*>(Parent);
    StandInfo = &mainWindow->StandInfo;
/*================================================================================================================================*\
 * ----------------------- Определение таблицы слоев -----------------------------------------------------------------------------*
\*================================================================================================================================*/
    StandItemModel = new QStandardItemModel();
    ui->LayersTable->setModel(StandItemModel);
    StandItemModel->setColumnCount(2);
    StandItemModel->setHeaderData(0,Qt::Horizontal,QStringLiteral("Кол-во счетчиков"));
    StandItemModel->setHeaderData(1,Qt::Horizontal,QStringLiteral("Шаг между счетчиками [см]"));

    ui->LayersTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->LayersTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->LayersTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->LayersTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);

    ui->LayersTable->setModel(StandItemModel);

    LoadParameters();
}

StandParameters::~StandParameters()
{
    delete ui;
}

void StandParameters::on_Apply_pb_clicked()
{
    SaveParameters();
    MainWindow* mainWindow = static_cast<MainWindow*>(Parent);
    mainWindow->FrontView->Redraw();
    mainWindow->SideView->Redraw();
    this->close();
}

void StandParameters::SaveParameters()
{
    StandInfo->alongXaxis       = ui->TD_AlongY_cb->isChecked();
    StandInfo->TD_X0posTop      = ui->TD_X0posTop_sb->value()/100;
    StandInfo->TD_X0posBottom   = ui->TD_X0posBottom_sb->value()/100;
    StandInfo->StepBetweenLayers =  ui->StepBetweenLayers_sb->value()/100;

    int NumRows = StandItemModel->rowCount();
    Layer_Info LI;
    StandInfo->NumLayers = NumRows;
    StandInfo->LayerInfo->clear();
    for(int i=0;i<NumRows;i++){
        QStandardItem * numDetectors = StandItemModel->item(i, 0);
        QStandardItem * step = StandItemModel->item(i, 1);
        LI.Num_Detectors = numDetectors->text().toInt();
        LI.StepBetween   = step->text().toDouble()/100;
        StandInfo->LayerInfo->append(LI);
    }
}

void StandParameters::LoadParameters()
{
    if(StandInfo->alongXaxis)
        ui->TD_AlongY_cb->setCheckState(Qt::Checked);
    else
        ui->TD_AlongY_cb->setCheckState(Qt::Unchecked);

    ui->TD_X0posTop_sb->setValue(StandInfo->TD_X0posTop*100);
    ui->TD_X0posBottom_sb->setValue(StandInfo->TD_X0posBottom*100);
    ui->StepBetweenLayers_sb->setValue(StandInfo->StepBetweenLayers*100);

    int NumRows = StandInfo->NumLayers;
    ui->NumLayers_sb->setValue(NumRows);
    for(int i=0;i<NumRows;i++){
        QStandardItem *numDetectors = new QStandardItem();
        numDetectors->setText(QString::number(StandInfo->LayerInfo->at(i).Num_Detectors));
        StandItemModel->setItem(i,0,numDetectors);

        QStandardItem *step = new QStandardItem();
        step->setText(QString::number(StandInfo->LayerInfo->at(i).StepBetween));
        StandItemModel->setItem(i,1,step);
    }

}


void StandParameters::on_CreateLineLayers_pb_clicked()
{
    for(int i=0;i<ui->NumLayers_sb->value();i++){
        QStandardItem *numDetectors = new QStandardItem();
        numDetectors->setText("1");
        StandItemModel->setItem(i,0,numDetectors);

        QStandardItem *step = new QStandardItem();
        step->setText("0");
        StandItemModel->setItem(i,1,step);

    }
}

