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
    int NumTrgDetectors_top;
    int NumTrgDetectors_bottom;
    QList<double> TrgDetectors_X0pos_top;
    QList<double> TrgDetectors_X0pos_bottom;
    int NumLayers;
    QList<Layer_Info> LayerInfo;

};



#endif // TOOLS_H
