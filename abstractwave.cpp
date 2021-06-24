#include "abstractwave.h"

AbstractWave::AbstractWave(QObject *parent) : QObject(parent)
{
}

AbstractWave::~AbstractWave() {}

void AbstractWave::update(QHash<QString, double> *paraMap,
                          QVector<double> *x, QVector<double> *y)
{
    frequency = (*paraMap)["frequency"];  //单位为Hz
    amplitude = (*paraMap)["amplitude"];  //单位为Vpp
    offset = (*paraMap)["offset"];
    phase = (*paraMap)["phase"];
    dataMin = (*paraMap)["dataMin"];
    dataMax = (*paraMap)["dataMax"];;  //数据值的范围
    dutyRatio = (*paraMap)["dutyRatio"];  //占空比
    dataLength = (*paraMap)["dataLength"];  //数据点数量
}
