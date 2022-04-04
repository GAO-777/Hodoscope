#ifndef DRAW_WIDGET_H
#define DRAW_WIDGET_H

#include <QGraphicsView>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QTimer>
#include <QList>
#include <QDebug>
#include <QKeyEvent>
#include "Tools.h"


// - - - Размеры желтого стола - - - //
#define YF_length   1.2
#define YF_width    0.4
#define YF_height   0.02
// - - - Размеры синих полочек - - - //
#define BF_length   1
#define BF_width    0.4
#define BF_height   0.01
// - - - Размеры верт. держателей - - - //
#define GF_width    0.02
// - - - Размеры счетчиков - - - //
#define D_length    0.95
#define D_width     0.04
#define D_height    0.007
// - - - Размеры триггерных счетчиков - - - //
#define TD_length   0.2
#define TD_width    0.04
#define TD_height   0.007



enum DrawingView {front,side};

class Draw_Widget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Draw_Widget(DrawingView view, QWidget *parent = 0);


signals:
    void resizeEvent_signal(double basic_meter);

public slots:
    void Redraw();  /* Cлот для обработчика переполнения таймера в нём будет производиться перерисовка виджета */

public:
    QTimer              *timer; /* Таймер для задержки отрисовки. Дело в том, что при создании окна и виджета
                                    необходимо некоторое время, чтобы родительский слой
                                    развернулся, чтобы принимать от него адекватные параметры ширины и высоты */
    /* - - -  Основные переменные - - - */
    DrawingView View;
    Stand_Info* StandInfo;
    bool IsMeterSet;    // true->берем уже посчитанный, false-> считаем
    double X_Meter;
    double Y_Meter;
    double BF_Ysize;
    double BF_Xsize;
    double BF_X0pos;
    double BF_Y0pos;
    double YF_Y0pos;
    double D_Xsize;
    double D_Ysize;
    // - - - Координаты триггерных счетчиков - - - //
    double TD_X0posTop;
    double TD_Y0posTop;
    double TD_X0posBottom;
    double TD_Y0posBottom;
    // - - - Координаты счетчиков всех слоёв - - - //
    QList<QList<QPointF>>* D_pos;
    // - - - Информация о событии - - - - - - - - -//
    QList<QPointF>* Triggered_Detectors;
    double k,b; // кооэффициенты прямой y=kx+b


    bool TrackRendered;


    QGraphicsScene      *scene;     // Объявляем сцену для отрисовки
    QGraphicsItemGroup  *Base_group;    // Элементы основы
    QGraphicsItemGroup  *Detectors_group;    // Элементы основы
    QGraphicsItemGroup  *ParticleTrail_group;

    void setBasicMeter(double meter);
    void drawBasicFrame();
    void setEventData(QList<QList<int>>* Trg_D);
    void drawParticleTrail();
    void DrawEvent();
    void TrajectoryCalculation();

    void resizeEvent(QResizeEvent *event); //Перегружаем событие изменения размера окна,чтобы перехватывать его
    void deleteItemsFromGroup(QGraphicsItemGroup *group); // Метод для удаления всех элементов из группы элементов

};



#endif // DRAW_WIDGET_H








