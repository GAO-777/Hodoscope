#ifndef TOOLS_H
#define TOOLS_H

#include <QList>
#include <QPointF>

#include <stdlib.h> // нужен для вызова функции rand(), srand()
#include <time.h> // нужен для вызова функции time()

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

static double getRandomFloat(double min, double max, int precision)
{
    // - min - нижний предел;
    // - max - верхний предел;
    // - precision - точность, количество знаков после комы.
    srand(time(0)*rand());
    double value;
    // получить случайное число как целое число с порядком precision
    value = rand() % (int)pow(10, precision);
    // получить вещественное число
    value = min + (value / pow(10, precision)) * (max - min);
    return value;
}

static bool compareX(const QPointF& first, const QPointF& second)
{
    if (first.x() < second.x())
        return true;
    else
        return false;
}


#endif // TOOLS_H
