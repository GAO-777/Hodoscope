#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "standparameters.h"
#include<QSizePolicy>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    StandInfo.TD_X0posTop      = 0;
    StandInfo.TD_X0posBottom   = 0;
    StandInfo.StepBetweenLayers= 0.05;
    StandInfo.alongXaxis       = false;
    StandInfo.NumLayers        = 5;

    Layer_Info LI;
    LI.Num_Detectors = 4;
    LI.StepBetween   = 0;
    StandInfo.LayerInfo = new QList<Layer_Info>;
    StandInfo.LayerInfo->append(LI);
    StandInfo.LayerInfo->append(LI);
    StandInfo.LayerInfo->append(LI);
    StandInfo.LayerInfo->append(LI);
    StandInfo.LayerInfo->append(LI);




    /* Инициализируем виджет с графикой */
    FrontView   = new Draw_Widget(DrawingView::front);
    SideView   = new Draw_Widget(DrawingView::side);
    FrontView->StandInfo = &StandInfo;
    SideView->StandInfo  = &StandInfo;


    SideView->IsMeterSet = true;
    SideView->setMinimumSize(400,500);
    SideView->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    QObject::connect(FrontView,&Draw_Widget::resizeEvent_signal,SideView,&Draw_Widget::setBasicMeter);

    /* и добавляем его на слой */
    ui->DrawWidgets_hl->addWidget(SideView);
    ui->DrawWidgets_hl->addWidget(FrontView);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Menu_StandParameters_triggered()
{
    StandParameters* newWindowSP = new StandParameters(this);
    newWindowSP->setWindowFlags(Qt::Window);
    newWindowSP->show();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
   /* QList<QList<int>>* Trg_D = new QList<QList<int>>();
    for(int i =0;i<StandInfo.NumLayers;i++){
        QList<int> Trg_D_layers = QList<int>();
        for(int p =0;p<StandInfo.LayerInfo->at(i).Num_Detectors;p++)
            Trg_D_layers.append(0);
        Trg_D->append(Trg_D_layers);
    }*/

    QList<QList<int>>* Triggered_Detectors = new QList<QList<int>>();




     QRandomGenerator random;
     random.seed(QDateTime::currentMSecsSinceEpoch());

     for (int i=0;i<5;i++){
         QList<int> Trg_D_layers = QList<int>();
         for (int p=0;p<1;p++)
             Trg_D_layers.append(rand()%4);
         Triggered_Detectors->append(Trg_D_layers);
     }






    SideView->setEventData(Triggered_Detectors);

    if(event->key() == Qt::Key_Enter){
       FrontView->DrawEvent();
       SideView->DrawEvent();
    }
    QMainWindow::keyPressEvent(event);
}

