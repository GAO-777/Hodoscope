#include "draw_widget.h"

Draw_Widget::Draw_Widget(DrawingView view, QWidget *parent)
     : QGraphicsView(parent)
{
    View = view;
    IsMeterSet = false;
    emit resizeEvent_signal(X_Meter);
    /* Немного поднастроим отображение виджета и его содержимого */
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключим скроллбар по горизонтали
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Отключим скроллбар по вертикали
    this->setAlignment(Qt::AlignCenter);                        // Делаем привязку содержимого к центру
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);    // Растягиваем содержимое по виджету
    this->setRenderHint(QPainter::Antialiasing);    // Устанавливаем сглаживание
    /* Также зададим минимальные размеры виджета */
    this->setMinimumHeight(200);
    this->setMinimumWidth(200);

    scene = new QGraphicsScene();   // Инициализируем сцену для отрисовки
    this->setScene(scene);          // Устанавливаем сцену в виджет

    Base_group              = new QGraphicsItemGroup();     // Инициализируем Базовую группу элементов
    Detectors_group         = new QGraphicsItemGroup();     // Инициализируем группу Cчетчиков
    ParticleTrail_group= new QGraphicsItemGroup();
    scene->addItem(Base_group);         // Добавляем Базовую группу в сцену
    scene->addItem(Detectors_group);         // Добавляем группу Cчетчиков в сцену
    scene->addItem(ParticleTrail_group);

    timer = new QTimer();               // Инициализируем Таймер
    timer->setSingleShot(true);
    // Подключаем СЛОТ для отрисовки к таймеру
    connect(timer, &QTimer::timeout, this,&Draw_Widget::Redraw);
    timer->start(50);                   // Стартуем таймер на 50 миллисекунд

    D_pos = QList<QList<QPointF>>();

}

void Draw_Widget::Redraw()
{
    /* Удаляем все элементы со сцены, если они есть перед новой отрисовкой */
    this->deleteItemsFromGroup(Base_group);
    this->deleteItemsFromGroup(Detectors_group);
    this->deleteItemsFromGroup(ParticleTrail_group);
    int width = this->width();      // определяем ширину нашего виджета
    int height = this->height();    // определяем высоту нашего виджета

    /* Устанавливаем размер сцены по размеру виджета
     * Первая координата - это левый верхний угол,
     * а Вторая - это правый нижний угол
     * */
    scene->setSceneRect(0,0,width,height);

    /* Приступаем к отрисовке произвольной картинки
     * */

        drawBasicFrame();

        //drawParticleTrail();

}

void Draw_Widget::setBasicMeter(double meter)
{
    X_Meter = meter;
    Y_Meter = meter;
}

void Draw_Widget::drawBasicFrame()
{
    int width = this->width();      // определяем ширину нашего виджета
    int height = this->height();    // определяем высоту нашего виджета

    /* ======= Размеры синей арматуры =======*/
    if(View == front){
        BF_Xsize = X_Meter*BF_length;
    }else{
        BF_Xsize = X_Meter*BF_width;
    }
    BF_Ysize = Y_Meter*BF_height;

    /* ======= Размеры верт. держателей =======*/
    int GF_Xsize = X_Meter*GF_width;
    int GF_Ysize = Y_Meter*(StandInfo->StepBetweenLayers)-BF_Ysize;

    /* ======= Размеры счетчиков =======*/
    if(View == front){
        D_Xsize = X_Meter*D_length;
    }else{
        D_Xsize = X_Meter*D_width;
    }
    D_Ysize = Y_Meter*D_height;

    /* ======= Размеры триггерных счетчиков =======*/
    int TD_Xsize,TD_Ysize;
    if(View == front){
        if(StandInfo->alongXaxis)
            TD_Xsize = X_Meter*TD_length;
        else
            TD_Xsize = X_Meter*TD_width;
    }else{
        if(StandInfo->alongXaxis)
            TD_Xsize = X_Meter*TD_width;
        else
            TD_Xsize = X_Meter*TD_length;
    }
    TD_Ysize = Y_Meter*TD_height;


    QPen penBlack(Qt::black);
    QPen penRed(Qt::red);

    QBrush BF_brush;
    BF_brush.setColor(QColor(70,113,213));
    BF_brush.setStyle(Qt::SolidPattern);



    QBrush YF_brush;
    YF_brush.setColor(QColor(0xDA,0xD4,0x62));
    YF_brush.setStyle(Qt::SolidPattern);

    QBrush GF_brush;
    GF_brush.setColor(QColor(189,189,189));
    GF_brush.setStyle(Qt::SolidPattern);

    QBrush D_brush;
    D_brush.setColor(QColor(0x49,0xE4,0x39));
    D_brush.setStyle(Qt::SolidPattern);

    QBrush TD_brush;
    TD_brush.setColor(QColor(0xEB,0x82,0x6A));
    TD_brush.setStyle(Qt::SolidPattern);



    /*================== Отрисовка желтого стола арматуры ================================= */
    int YF_Xsize,YF_Ysize;
    if(View == front){
        YF_Xsize = X_Meter*YF_length;
    }else{
        YF_Xsize = X_Meter*YF_width;
    }
    YF_Ysize = Y_Meter*YF_height;
    YF_Y0pos = Y_Meter*0.5;


    QGraphicsItem* yellow_frame = scene->addRect(QRectF(-(YF_Xsize/2), -(YF_Ysize/2), YF_Xsize, YF_Ysize),penBlack,YF_brush);
    yellow_frame->setPos(width/2,YF_Y0pos);
    Base_group->addToGroup(yellow_frame);


    /* Позиция синей арматуры */
    BF_X0pos = width/2;
    BF_Y0pos = YF_Y0pos-BF_Ysize/2;
    int BF_Ylast = 0; // положение последних счетчиков, чтоб на них положить триггерный
    D_pos.clear();
/*================================================================================================================================*\
 * ----------------------- Отрисовка статичных элементов -------------------------------------------------------------------------*
\*================================================================================================================================*/
    for(int i=1;i<=StandInfo->NumLayers;i++){
        double StepBetweenLayers = i*StandInfo->StepBetweenLayers*Y_Meter;
        /* Отрисовка синей арматуры */
        QGraphicsItem* blue_frame = scene->addRect(QRectF(-(BF_Xsize/2), -(BF_Ysize/2), BF_Xsize, BF_Ysize),penBlack,BF_brush);
        blue_frame->setPos(BF_X0pos,BF_Y0pos-StepBetweenLayers);
        Base_group->addToGroup(blue_frame);

        /* Отрисовка верт. держателей */
        QGraphicsItem* gray_frameL = scene->addRect(QRectF(-(GF_Xsize/2), -(GF_Ysize/2), GF_Xsize, GF_Ysize),penBlack,GF_brush);
        gray_frameL->setPos((BF_X0pos-BF_Xsize/2)+X_Meter*0.05,YF_Y0pos-(StepBetweenLayers)+GF_Ysize/2);
        Base_group->addToGroup(gray_frameL);

        QGraphicsItem* gray_frameR = scene->addRect(QRectF(-(GF_Xsize/2), -(GF_Ysize/2), GF_Xsize, GF_Ysize),penBlack,GF_brush);
        gray_frameR->setPos((BF_X0pos+BF_Xsize/2)-X_Meter*0.05,YF_Y0pos-(StepBetweenLayers)+GF_Ysize/2);
        Base_group->addToGroup(gray_frameR);

        int Num_Detectors = StandInfo->LayerInfo->at(i-1).Num_Detectors;
        double StepBetween = (StandInfo->LayerInfo->at(i-1).StepBetween)*X_Meter;

        QList<QPointF> d_pos;
        if(View==front){
            if(Num_Detectors!=0){
                QGraphicsItem* detectors_r = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                detectors_r->setPos(blue_frame->pos().x(), blue_frame->pos().y()-BF_Ysize);
                Detectors_group->addToGroup(detectors_r);
                d_pos.append(detectors_r->pos());
            }
             D_pos.append(d_pos);
        }else if(View==side){
            if(Num_Detectors%2==0){
                for(int p = 1;p<=Num_Detectors;){

                    QGraphicsItem* detectors_l = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                    detectors_l->setPos(blue_frame->pos().x()-((StepBetween/2+D_Xsize/2)*p), blue_frame->pos().y()-BF_Ysize);
                    Detectors_group->addToGroup(detectors_l);
                    d_pos.append(detectors_l->pos());

                    QGraphicsItem* detectors_r = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                    detectors_r->setPos(blue_frame->pos().x()+((StepBetween/2+D_Xsize/2)*p), blue_frame->pos().y()-BF_Ysize);
                    Detectors_group->addToGroup(detectors_r);
                     d_pos.append(detectors_r->pos());

                    p=p+2;
                }

            }else{
                for(int p = 0;p<Num_Detectors;p++){
                    if(p%2 == 0){
                        if(p==0){
                            QGraphicsItem* detectors_r = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                            detectors_r->setPos(blue_frame->pos().x()+((StepBetween+D_Xsize)*p), blue_frame->pos().y()-BF_Ysize);
                            Detectors_group->addToGroup(detectors_r);
                            d_pos.append(detectors_r->pos());
                        }else{
                            QGraphicsItem* detectors_r = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                            detectors_r->setPos(blue_frame->pos().x()+((StepBetween+D_Xsize)*(p-1)), blue_frame->pos().y()-BF_Ysize);
                            Detectors_group->addToGroup(detectors_r);
                            d_pos.append(detectors_r->pos());
                        }
                    }else{

                        QGraphicsItem* detectors_l = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                        detectors_l->setPos(blue_frame->pos().x()-((StepBetween+D_Xsize)*p), blue_frame->pos().y()-BF_Ysize);
                        Detectors_group->addToGroup(detectors_l);
                        d_pos.append(detectors_l->pos());
                    }
                }
            }
            std::sort(d_pos.begin(), d_pos.end(), compareX);
            D_pos.append(d_pos);
        }
    }

    TD_Y0posBottom  = (YF_Y0pos-YF_Ysize/2-TD_Ysize/2)- Y_Meter*0.001;
    TD_Y0posTop     = (BF_Y0pos-(StandInfo->NumLayers*StandInfo->StepBetweenLayers*Y_Meter)-BF_Ysize/2-D_Ysize-TD_Ysize/2)- Y_Meter*0.001;
    if(View == front){
        if(StandInfo->alongXaxis){
//======== Нижний триггерный счетчик ================//
            TD_X0posBottom = BF_X0pos;
            QGraphicsItem* trig_detectorBottom = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorBottom->setPos(TD_X0posBottom,TD_Y0posBottom);
            Detectors_group->addToGroup(trig_detectorBottom);
//======== Верхний триггерный счетчик ================//
       /*     TD_X0posTop = BF_X0pos;
            QGraphicsItem* trig_detectorTop = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorTop->setPos(TD_X0posTop,TD_Y0posTop);
            Detectors_group->addToGroup(trig_detectorTop);*/
        }else{
//======== Нижний триггерный счетчик ================//
            TD_X0posBottom = BF_X0pos+(StandInfo->TD_X0posBottom)*X_Meter;
            QGraphicsItem* trig_detectorBottom = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorBottom->setPos(TD_X0posBottom,TD_Y0posBottom);
            Detectors_group->addToGroup(trig_detectorBottom);
//======== Верхний триггерный счетчик ================//
            TD_X0posTop = BF_X0pos+(StandInfo->TD_X0posTop)*X_Meter;;
            QGraphicsItem* trig_detectorTop = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorTop->setPos(TD_X0posTop,TD_Y0posTop);
            Detectors_group->addToGroup(trig_detectorTop);
        }
    }else if(View == side){
        if(StandInfo->alongXaxis){
//======== Нижний триггерный счетчик ================//
            TD_X0posBottom = BF_X0pos+(StandInfo->TD_X0posBottom)*X_Meter;
            QGraphicsItem* trig_detectorBottom = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorBottom->setPos(TD_X0posBottom,TD_Y0posBottom);
            Detectors_group->addToGroup(trig_detectorBottom);
//======== Верхний триггерный счетчик ================//
            TD_X0posTop = BF_X0pos+(StandInfo->TD_X0posTop)*X_Meter;
            QGraphicsItem* trig_detectorTop = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorTop->setPos(TD_X0posTop,TD_Y0posTop);
            Detectors_group->addToGroup(trig_detectorTop);

        }else{
//======== Нижний триггерный счетчик ================//
            TD_X0posBottom = BF_X0pos;
            QGraphicsItem* trig_detectorBottom = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorBottom->setPos(TD_X0posBottom,TD_Y0posBottom);
            Detectors_group->addToGroup(trig_detectorBottom);
//======== Верхний триггерный счетчик ================//
            TD_X0posTop = BF_X0pos;
            QGraphicsItem* trig_detectorTop = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorTop->setPos(TD_X0posTop,TD_Y0posTop);
            Detectors_group->addToGroup(trig_detectorTop);
        }
    }

}

void Draw_Widget::drawParticleTrail()
{


    QBrush D_brush;
    D_brush.setColor(Qt::red);
    D_brush.setStyle(Qt::SolidPattern);
    QPen penBlack(Qt::black);
    if(View==side){

        QList<QPointF> Trg_D;
        Trg_D<<D_pos.at(0).at(0)<<D_pos.at(4).at(3)<<D_pos.at(3).at(2)<<D_pos.at(1).at(1)<<D_pos.at(2).at(1)<<D_pos.at(2).at(2);

        for (int i=0;i<Trg_D.size();i++){

            QGraphicsItem* triggered_detector = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
            triggered_detector->setPos(Trg_D.at(i));
            ParticleTrail_group->addToGroup(triggered_detector);

        }


        double a;
        double b;
        double sumx = 0;
        double sumy = 0;
        double sumx2 = 0;
        double sumxy = 0;
        int n = Trg_D.size();
        for (int i = 0; i<n; i++) {
            sumx += Trg_D.at(i).x();
            sumy += Trg_D.at(i).y();
            sumx2 += Trg_D.at(i).x() * Trg_D.at(i).x();
            sumxy += Trg_D.at(i).x() * Trg_D.at(i).y();
        }
        a = (n*sumxy - (sumx*sumy)) / (n*sumx2 - sumx*sumx);
        b = (sumy - a*sumx) / n;

        qDebug()<<a,b;

        QPen penRed(Qt::red);
        QGraphicsItem* particle_trail = scene->addLine(0,b,500,500*a+b,penRed);
        ParticleTrail_group->addToGroup(particle_trail);
    }
    if(View==front){
        QPen penRed(Qt::red);
        QGraphicsItem* particle_trail = scene->addLine(TD_X0posTop,TD_Y0posTop,TD_X0posBottom,TD_Y0posBottom,penRed);
        ParticleTrail_group->addToGroup(particle_trail);

         QList<QPointF> Trg_D;
        Trg_D<<QPointF(TD_X0posTop,TD_Y0posTop)<<QPointF(TD_X0posBottom,TD_Y0posBottom);
        double a;
        double b;
        double sumx = 0;
        double sumy = 0;
        double sumx2 = 0;
        double sumxy = 0;
        int n = Trg_D.size();


        for (int i = 0; i<n; i++) {
            sumx += Trg_D.at(i).x();
            sumy += Trg_D.at(i).y();
            sumx2 += Trg_D.at(i).x() * Trg_D.at(i).x();
            sumxy += Trg_D.at(i).x() * Trg_D.at(i).y();
        }
        if((n*sumx2 - sumx*sumx) == 0)
            a=0;
        else
            a = (n*sumxy - (sumx*sumy)) / (n*sumx2 - sumx*sumx);
        b = (sumy - a*sumx) / n;

    for(int i=0;i<D_pos.size();i++){
        double X,Y;
        Y=D_pos.at(i).at(0).y();

        if(a==0)
            X=D_pos.at(i).at(0).x();
        else
            X=(Y-b)/a;

        qDebug()<<a;
        double DD_Xsize = X_Meter*D_width;

   double  DD_Ysize = Y_Meter*D_height;
        QGraphicsItem* triggered_detector = scene->addRect(QRectF(-(DD_Xsize/2), -(DD_Ysize/2),DD_Xsize, DD_Ysize),penBlack,D_brush);
        triggered_detector->setPos(X,Y);
        ParticleTrail_group->addToGroup(triggered_detector);
    }
    }
}

/* Этим методом перехватываем событие изменения размера виджет
 * */
void Draw_Widget::resizeEvent(QResizeEvent *event)
{
    timer->start(50);   // Как только событие произошло стартуем таймер для отрисовки
    QGraphicsView::resizeEvent(event);  // Запускаем событие родителького класса

    int width = this->width();      // определяем ширину нашего виджета
    int height = this->height();    // определяем высоту нашего виджета
    if(!IsMeterSet){
        X_Meter = (width/10)*8;
        Y_Meter = X_Meter;
    }
    emit resizeEvent_signal(X_Meter);
}


/* Метод для удаления всех элементов из группы
 * */
void Draw_Widget::deleteItemsFromGroup(QGraphicsItemGroup *group)
{
    /* Перебираем все элементы сцены, и если они принадлежат группе,
     * переданной в метод, то удаляем их
     * */
    foreach( QGraphicsItem *item, scene->items(group->boundingRect())) {
       if(item->group() == group ) {
          delete item;
       }
    }
}

void Draw_Widget::DrawEvent()
{

        if(TrackRendered){
         TrackRendered = !TrackRendered;
             this->deleteItemsFromGroup(ParticleTrail_group);
        }else{
         drawParticleTrail();
         TrackRendered = !TrackRendered;
        }


}
