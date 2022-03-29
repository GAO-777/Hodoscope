#include "draw_widget.h"

Draw_Widget::Draw_Widget(DrawingView view, QWidget *parent)
     : QGraphicsView(parent)
{
    View = view;
    IsMeterSet = false;
    emit resizeEvent_signal(X_Meter);
    Ysize_betweenBF = 0.05;
    /* Немного поднастроим отображение виджета и его содержимого */
    //this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключим скроллбар по горизонтали
    //this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Отключим скроллбар по вертикали
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
    ParticleTrails_group    = new QGraphicsItemGroup();

    scene->addItem(Base_group);         // Добавляем Базовую группу в сцену
    scene->addItem(Detectors_group);         // Добавляем группу Cчетчиков в сцену
    scene->addItem(ParticleTrails_group);

    timer = new QTimer();               // Инициализируем Таймер
    timer->setSingleShot(true);
    // Подключаем СЛОТ для отрисовки к таймеру
    connect(timer, &QTimer::timeout, this,&Draw_Widget::slotAlarmTimer);
    timer->start(50);                   // Стартуем таймер на 50 миллисекунд

    BasicFrame_pos = new QList<QPointF>();

}

void Draw_Widget::slotAlarmTimer()
{
    /* Удаляем все элементы со сцены, если они есть перед новой отрисовкой */
    this->deleteItemsFromGroup(Base_group);
    this->deleteItemsFromGroup(Detectors_group);

    int width = this->width();      // определяем ширину нашего виджета
    int height = this->height();    // определяем высоту нашего виджета

    /* Устанавливаем размер сцены по размеру виджета
     * Первая координата - это левый верхний угол,
     * а Вторая - это правый нижний угол
     * */
    scene->setSceneRect(0,0,width-10,height-10);

    /* Приступаем к отрисовке произвольной картинки
     * */

        drawBasicFrame();

        drawParticleTrail();

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
    int GF_Ysize = Y_Meter*Ysize_betweenBF-BF_Ysize;

    /* ======= Размеры счетчиков =======*/
    int D_Xsize,D_Ysize;
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
    BF_brush.setColor(Qt::blue);
    BF_brush.setStyle(Qt::SolidPattern);



    QBrush YF_brush;
    YF_brush.setColor(Qt::yellow);
    YF_brush.setStyle(Qt::SolidPattern);

    QBrush GF_brush;
    GF_brush.setColor(Qt::gray);
    GF_brush.setStyle(Qt::SolidPattern);

    QBrush D_brush;
    D_brush.setColor(Qt::darkGreen);
    D_brush.setStyle(Qt::SolidPattern);

    QBrush TD_brush;
    TD_brush.setColor(Qt::darkRed);
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



    BasicFrame_pos->clear();
/*================================================================================================================================*\
 * ----------------------- Отрисовка статичных элементов -------------------------------------------------------------------------*
\*================================================================================================================================*/
    for(int i=1;i<=StandInfo->NumLayers;i++){

        /* Отрисовка синей арматуры */
        QGraphicsItem* blue_frame = scene->addRect(QRectF(-(BF_Xsize/2), -(BF_Ysize/2), BF_Xsize, BF_Ysize),penBlack,BF_brush);
        blue_frame->setPos(BF_X0pos,BF_Y0pos-(i*Ysize_betweenBF*Y_Meter));
        BasicFrame_pos->append(blue_frame->pos());
        Base_group->addToGroup(blue_frame);

        /* Отрисовка верт. держателей */
        QGraphicsItem* gray_frameL = scene->addRect(QRectF(-(GF_Xsize/2), -(GF_Ysize/2), GF_Xsize, GF_Ysize),penBlack,GF_brush);
        gray_frameL->setPos((BF_X0pos-BF_Xsize/2)+X_Meter*0.05,YF_Y0pos-(i*Ysize_betweenBF*Y_Meter)+GF_Ysize/2);
        Base_group->addToGroup(gray_frameL);

        QGraphicsItem* gray_frameR = scene->addRect(QRectF(-(GF_Xsize/2), -(GF_Ysize/2), GF_Xsize, GF_Ysize),penBlack,GF_brush);
        gray_frameR->setPos((BF_X0pos+BF_Xsize/2)-X_Meter*0.05,YF_Y0pos-(i*Ysize_betweenBF*Y_Meter)+GF_Ysize/2);
        Base_group->addToGroup(gray_frameR);

        int Num_Detectors = StandInfo->LayerInfo->at(i-1).Num_Detectors;
        double StepBetween = (StandInfo->LayerInfo->at(i-1).StepBetween/100)*X_Meter;

        if(View==front){
            if(Num_Detectors!=0){
                QGraphicsItem* detectors_r = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                detectors_r->setPos(blue_frame->pos().x(), blue_frame->pos().y()-BF_Ysize);
                Detectors_group->addToGroup(detectors_r);
            }
        }else if(View==side){
            if(Num_Detectors%2==0){
                for(int p = 1;p<=Num_Detectors;){

                    QGraphicsItem* detectors_r = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                    detectors_r->setPos(blue_frame->pos().x()+((StepBetween/2+D_Xsize/2)*p), blue_frame->pos().y()-BF_Ysize);
                    Detectors_group->addToGroup(detectors_r);

                    QGraphicsItem* detectors_l = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                    detectors_l->setPos(blue_frame->pos().x()-((StepBetween/2+D_Xsize/2)*p), blue_frame->pos().y()-BF_Ysize);
                    Detectors_group->addToGroup(detectors_l);
                    p=p+2;
                }

            }else{
                for(int p = 0;p<Num_Detectors;p++){
                    if(p%2 == 0){
                        if(p==0){
                            QGraphicsItem* detectors_r = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                            detectors_r->setPos(blue_frame->pos().x()+((StepBetween+D_Xsize)*p), blue_frame->pos().y()-BF_Ysize);
                            Detectors_group->addToGroup(detectors_r);
                        }else{
                            QGraphicsItem* detectors_r = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                            detectors_r->setPos(blue_frame->pos().x()+((StepBetween+D_Xsize)*(p-1)), blue_frame->pos().y()-BF_Ysize);
                            Detectors_group->addToGroup(detectors_r);
                        }
                    }else{

                        QGraphicsItem* detectors_l = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
                        detectors_l->setPos(blue_frame->pos().x()-((StepBetween+D_Xsize)*p), blue_frame->pos().y()-BF_Ysize);
                        Detectors_group->addToGroup(detectors_l);
                    }
                }
            }
        }
    }


    TD_Y0posBottom  = (YF_Y0pos-YF_Ysize/2-TD_Ysize/2)- Y_Meter*0.001;
    TD_Y0posTop     = (BF_Y0pos-(StandInfo->NumLayers*Ysize_betweenBF*Y_Meter)-BF_Ysize/2-D_Ysize-TD_Ysize/2)- Y_Meter*0.001;
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
            TD_X0posBottom = BF_X0pos+(StandInfo->TD_X0posBottom/100)*X_Meter;
            QGraphicsItem* trig_detectorBottom = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorBottom->setPos(TD_X0posBottom,TD_Y0posBottom);
            Detectors_group->addToGroup(trig_detectorBottom);
//======== Верхний триггерный счетчик ================//
            TD_X0posTop = BF_X0pos+(StandInfo->TD_X0posTop/100)*X_Meter;;
            QGraphicsItem* trig_detectorTop = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorTop->setPos(TD_X0posTop,TD_Y0posTop);
            Detectors_group->addToGroup(trig_detectorTop);
        }
    }else if(View == side){
        if(StandInfo->alongXaxis){
//======== Нижний триггерный счетчик ================//
            TD_X0posBottom = BF_X0pos+(StandInfo->TD_X0posBottom/100)*X_Meter;
            QGraphicsItem* trig_detectorBottom = scene->addRect(QRectF(-(TD_Xsize/2), -(TD_Ysize/2), TD_Xsize, TD_Ysize),penBlack,TD_brush);
            trig_detectorBottom->setPos(TD_X0posBottom,TD_Y0posBottom);
            Detectors_group->addToGroup(trig_detectorBottom);
//======== Верхний триггерный счетчик ================//
            TD_X0posTop = BF_X0pos+(StandInfo->TD_X0posTop/100)*X_Meter;
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

void Draw_Widget::drawDetectors()
{
    int width = this->width();      // определяем ширину нашего виджета
    int height = this->height();    // определяем высоту нашего виджета

    double NUM_layer1 = 2;
    double Step_layer1 = X_Meter*0.1;

    int D_Xsize,D_Ysize;
    if(View == front){
        D_Xsize = X_Meter*D_length;
    }else{
        D_Xsize = X_Meter*D_width;
    }
    D_Ysize = Y_Meter*D_height;


    QPen penBlack(Qt::black);
    QBrush GrayBrush;
    GrayBrush.setColor(Qt::gray);
    GrayBrush.setStyle(Qt::SolidPattern);
    Detector_list = new QList<Detector*>();

/*========= Первый слой ========================== */
    //for(int i=0;i<NUM_layer1;i++){
    if(View == front){

        Detector* detectors = new Detector(D_Xsize,D_Ysize);
        int x_pos,y_pos;
        x_pos = BasicFrame_pos->at(0).x();
        y_pos = BasicFrame_pos->at(0).y()-BF_Ysize;
        detectors->setPos(x_pos,y_pos);
        scene->addItem(detectors);
        Detector_list->append(detectors);
        Detectors_group->addToGroup(detectors);



    }else{




        Detector* detector1 = new Detector(D_Xsize,D_Ysize);
        int x_pos,y_pos;
        x_pos = BasicFrame_pos->at(0).x()+Step_layer1/2;
        y_pos = BasicFrame_pos->at(0).y()-BF_Ysize;
        detector1->setPos(x_pos,y_pos);
        scene->addItem(detector1);
        Detector_list->append(detector1);
        Detectors_group->addToGroup(detector1);

        Detector* detector2 = new Detector(D_Xsize,D_Ysize);

        x_pos = BasicFrame_pos->at(0).x()-Step_layer1/2;
        y_pos = BasicFrame_pos->at(0).y()-BF_Ysize;
        detector2->setPos(x_pos,y_pos);
        scene->addItem(detector2);
        Detector_list->append(detector2);
        Detectors_group->addToGroup(detector2);
    }

    //}



}

void Draw_Widget::drawParticleTrail()
{
    QPen penRed(Qt::red);

    QGraphicsItem* particle_trail = scene->addLine(0,0,200,200,penRed);
    particle_trail->setPos(100,100);
    ParticleTrails_group->addToGroup(particle_trail);

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

void Draw_Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_K){
        for(int i=0;i<Detector_list->size();i++){



        }


        }


     QGraphicsView::keyPressEvent(event);

}

Detector::Detector(int size_x, int size_y)
{
    m_color = Qt::black;
    x_size = size_x;
    y_size = size_y;
}

QRectF Detector::boundingRect() const
{
    return QRectF(-(x_size/2), -(y_size/2), x_size, y_size);
}

void Detector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setBrush(m_color);
    painter->drawRect(-(x_size/2), -(y_size/2), x_size, y_size);
}

void Detector::setColor(const QColor &color){
    m_color=color;
}

void Detector::reflush()
{
    this->update();
}
