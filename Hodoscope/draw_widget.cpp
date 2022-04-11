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
    //QSizeGrip * sizeGrip = new QSizeGrip(this);

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

    D_pos = new QList<QList<QPointF>>();
    Triggered_Detectors = new QList<QPointF>();
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
    YF_Y0pos = height/2 + Y_Meter*0.10;


    QGraphicsItem* yellow_frame = scene->addRect(QRectF(-(YF_Xsize/2), -(YF_Ysize/2), YF_Xsize, YF_Ysize),penBlack,YF_brush);
    yellow_frame->setPos(width/2,YF_Y0pos);
    Base_group->addToGroup(yellow_frame);


    /* Позиция синей арматуры */
    BF_X0pos = width/2;
    BF_Y0pos = YF_Y0pos-BF_Ysize/2;
    int BF_Ylast = 0; // положение последних счетчиков, чтоб на них положить триггерный
    D_pos->clear();
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
             D_pos->append(d_pos);
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
            D_pos->append(d_pos);
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

void Draw_Widget::setEventData(QList<QList<int>>* Trg_D)
{
    // - - - Передаем данные события в отрисовку - - - //
    Triggered_Detectors->clear();
    if(View == side){
        for(int i=0;i<Trg_D->size();i++) // прокручиваем все слои
            // Берем конкретный слой и в нем номер сработавшего счетчика и вытаскиваем по номеру его координату
            for(int p=0;p<Trg_D->at(i).size();p++)
                Triggered_Detectors->append(D_pos->at(i).at(Trg_D->at(i).at(p)));
    }if(View == front){
        Triggered_Detectors->append(QPointF(TD_X0posTop,TD_Y0posTop));
        Triggered_Detectors->append(QPointF(TD_X0posBottom,TD_Y0posBottom));
    }
}

void Draw_Widget::drawParticleTrail()
{


    QBrush D_brush;
    D_brush.setColor(Qt::red);
    D_brush.setStyle(Qt::SolidPattern);
    QPen penBlack(Qt::black);
    if(View==side){

        // - - - Закрашиваем все счетчики, которые сработали - - - //
        for (int i=0;i<Triggered_Detectors->size();i++){
            QGraphicsItem* triggered_detector = scene->addRect(QRectF(-(D_Xsize/2), -(D_Ysize/2), D_Xsize, D_Ysize),penBlack,D_brush);
            triggered_detector->setPos(Triggered_Detectors->at(i));
            ParticleTrail_group->addToGroup(triggered_detector);
        }

        TrajectoryCalculation(&k,&b); // находим наклон и смещение прямой

        // - - - Начало и конец траектории - - - //
        double X1_line,Y1_line;
        double X2_line,Y2_line;

        if(k!=0){
            //Triggered_Detectors->append(QPointF(TD_X0posTop,TD_Y0posTop));
            //Triggered_Detectors->append(QPointF(TD_X0posBottom,TD_Y0posBottom));
            //TrajectoryCalculation(&k,&b);
            Y1_line = TD_Y0posTop - (0.2*Y_Meter);
            X1_line = (Y1_line-b)/k;
            Y2_line = TD_Y0posBottom + (0.2*Y_Meter);
            X2_line = (Y2_line-b)/k;


        }else{
            Y1_line = TD_Y0posTop - (0.2*Y_Meter);
            X1_line = b;
            Y2_line = TD_Y0posBottom + (0.2*Y_Meter);
            X2_line = b;
        }



        QPen penRed(Qt::red);
        QGraphicsItem* particle_trail = scene->addLine(X1_line,Y1_line,X2_line,Y2_line,penRed);
        ParticleTrail_group->addToGroup(particle_trail);
    }
    if(View==front){

        QPen penRed(Qt::red);
        QGraphicsItem* particle_trail = scene->addLine(TD_X0posTop,TD_Y0posTop,TD_X0posBottom,TD_Y0posBottom,penRed);
        ParticleTrail_group->addToGroup(particle_trail);

        TrajectoryCalculation(&k,&b); // находим наклон и смещение прямой

    for(int i=0;i<D_pos->size();i++){
        double X,Y;
        Y=D_pos->at(i).at(0).y();

        if(k==0)
            X=D_pos->at(i).at(0).x();
        else
            X=(Y-b)/k;

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

void Draw_Widget::deleteEventView()
{
    this->deleteItemsFromGroup(ParticleTrail_group);
}

void Draw_Widget::DrawEvent()
{
    this->deleteItemsFromGroup(ParticleTrail_group);
    drawParticleTrail();
}

void Draw_Widget::TrajectoryCalculation(double *k, double *b)
{
    double sumx = 0;
    double sumy = 0;
    double sumx2 = 0;
    double sumxy = 0;
    int n = Triggered_Detectors->size();

    for (int i = 0; i<n; i++) {
        sumx += Triggered_Detectors->at(i).x();
        sumy += Triggered_Detectors->at(i).y();
        sumx2 += Triggered_Detectors->at(i).x() * Triggered_Detectors->at(i).x();
        sumxy += Triggered_Detectors->at(i).x() * Triggered_Detectors->at(i).y();
    }

    double a = (n*sumx2 - sumx*sumx);
    double aa = round((n*sumx2 - sumx*sumx)*100)/100;

    if(aa == 0){
        *k = 0;
        if(Triggered_Detectors->size() !=0)
            *b = Triggered_Detectors->at(0).x();
        else *b = 0;
    }
    else{
        *k = (n*sumxy - (sumx*sumy)) / (n*sumx2 - sumx*sumx);
        *b = (sumy - *k*sumx) / n;
    }

}

QList<QList<int>>* Draw_Widget::createRandomData()
{
    double X1_lineRand,Y1_lineRand;
    double X2_lineRand,Y2_lineRand;


    Y1_lineRand = TD_Y0posTop;
    Y2_lineRand = TD_Y0posBottom;
    double xx = getRandomFloat(-TD_Xsize/2,TD_Xsize/2,2);
   // X1_lineRand = TD_X0posTop + xx;
    //X2_lineRand = TD_X0posBottom + xx;
    X1_lineRand = TD_X0posTop + getRandomFloat(-TD_Xsize/2,TD_Xsize/2,2);
    X2_lineRand = TD_X0posBottom + getRandomFloat(-TD_Xsize/2,TD_Xsize/2,2);
    qDebug()<<X1_lineRand<<X2_lineRand;

    QPen penRed(Qt::red);
    QGraphicsItem* particle_trail = scene->addLine(X1_lineRand,Y1_lineRand,X2_lineRand,Y2_lineRand,penRed);
    ParticleTrail_group->addToGroup(particle_trail);

    QList<QGraphicsItem *> colliding = scene->collidingItems(particle_trail);
    qDebug()<<colliding;


    QList<QList<int>>* Triggered_Detectors = new QList<QList<int>>();


    for(int n=0;n<D_pos->size();n++){
        QList<int> newTrgD = QList<int>();
        for(int m=0;m<D_pos->at(n).size();m++){
            for(int i=0;i<colliding.size();i++)
                if(colliding[i]->pos() == D_pos->at(n).at(m))
                    newTrgD.append(m);
        }
        Triggered_Detectors->append(newTrgD);
    }

    return Triggered_Detectors;
}
















