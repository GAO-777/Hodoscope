#ifndef TOOLS_H
#define TOOLS_H

#include <QList>

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
    int NumLayers;
    QList<Layer_Info>* LayerInfo;
};





#endif // TOOLS_H
