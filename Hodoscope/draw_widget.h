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
#define D_height    0.01
// - - - Размеры триггерных счетчиков - - - //
#define TD_length   0.2
#define TD_width    0.04
#define TD_height   0.007



enum DrawingView {front,side};


class Detector:public QGraphicsItem
{
public:
    Detector(int size_x, int size_y);
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setColor(const QColor &color);
    void reflush();
private:
    int x_size;
    int y_size;
    QColor m_color;
};

class Draw_Widget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Draw_Widget(DrawingView view, QWidget *parent = 0);


signals:
    void resizeEvent_signal(double basic_meter);

private slots:
    void slotAlarmTimer();  /* Cлот для обработчика переполнения таймера в нём будет производиться перерисовка виджета */

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

    double TD_X0posTop;
    double TD_Y0posTop;
    double TD_X0posBottom;
    double TD_Y0posBottom;
    double Ysize_betweenBF;

    QGraphicsScene      *scene;     // Объявляем сцену для отрисовки
    QGraphicsItemGroup  *Base_group;    // Элементы основы
    QGraphicsItemGroup  *ParticleTrails_group;
    QList<QPointF>*      BasicFrame_pos;
    QGraphicsItemGroup  *Detectors_group;    // Элементы основы
    QList<Detector*>* Detector_list;

    void setBasicMeter(double meter);
    void drawBasicFrame();
    void drawDetectors();
    void drawParticleTrail();


    void resizeEvent(QResizeEvent *event); //Перегружаем событие изменения размера окна,чтобы перехватывать его
    void deleteItemsFromGroup(QGraphicsItemGroup *group); // Метод для удаления всех элементов из группы элементов

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
};



#endif // DRAW_WIDGET_H








