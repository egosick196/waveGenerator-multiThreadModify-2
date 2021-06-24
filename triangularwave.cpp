#include "triangularwave.h"

TriangularWave::TriangularWave(): AbstractWave()
{
}

TriangularWave::~TriangularWave() {}

void TriangularWave::update(QHash<QString, double> *paraMap,
                            QVector<double> *x1, QVector<double> *y1)
{
    //首先更新参数
    AbstractWave::update(paraMap);

    //为了计算曲线数据方便，计算中间量
    double rho = phase/360;
    double T = 1 / frequency;
    double k = dutyRatio / 100;
    double intval = 1.5 * T / (dataLength - 1);

    //分段函数与横轴交点
    double startPhasePoint = -rho * T;
    double halfPhasePoint = 0.5 * T + startPhasePoint;
    double endPhasePoint = T + startPhasePoint;

    //分段函数分段点
    double peak1 = startPhasePoint + k * T / 2;
    double peak2 = startPhasePoint + T * (2 - k) / 2;
    double peak3 = peak1 + T;
    double peak4 = peak2 + T;

    //各段斜率
    double k_rise = (dataMax - dataMin) / (peak3 - peak2);
    double k_drop = (dataMin - dataMax) / (peak2 - peak1);

    double t = 0;  x1->clear();  y1->clear();
    double currAmplitude;
    for(int i = 0; i < dataLength; i++)
    {
        if(t < peak1)
        {
            currAmplitude = k_rise * (t - startPhasePoint) + offset;
        }
        else if(t < peak2)
        {
            currAmplitude = k_drop * (t - halfPhasePoint) + offset;
        }
        else if(t < peak3)
        {
            currAmplitude = k_rise * (t - endPhasePoint) + offset;
        }
        else if(t < peak4)
        {
            currAmplitude = k_drop * (t - halfPhasePoint - T) + offset;
        }
        else
        {
            currAmplitude = k_rise * (t - endPhasePoint - T) + offset;
        }
        x1->append(t * 1000),  y1->append(currAmplitude);
        t += intval;
    }
}
