#ifndef TOOLS_H
#define TOOLS_H

#include <QList>
#include <QPointF>

struct Layer_Info
{
    int Num_Detectors;
    double StepBetween;
};

struct Stand_Info
{
    bool   alongXaxis;      // Расположить триггерные счетчики вдоль оси Y
    double TD_X0posTop;     // Положение X0 верхнего счетчика
    double TD_X0posBottom;  // Положение X0 нижнего счетчика
    double StepBetweenLayers;
    int NumLayers;
    QList<Layer_Info>* LayerInfo;
};

struct Event_Info
{

};

static bool compareX(const QPointF& first, const QPointF& second)
{
    if (first.x() < second.x())
        return true;
    else
        return false;
}


#endif // TOOLS_H
