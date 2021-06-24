#include "squarewave.h"

SquareWave::SquareWave(): AbstractWave()
{
}

SquareWave::~SquareWave() {}

void SquareWave::update(QHash<QString, double> *paraMap,
                      QVector<double> *x1, QVector<double>*y1)
{
    //首先更新参数
    AbstractWave::update(paraMap);

    //为了计算曲线数据方便，计算中间量
    double rho = phase/360;
    double T = 1 / frequency;
    double k = dutyRatio / 100;
    double intval = 1.5 * T / (dataLength - 1);

    //分段函数的分段点
    double startPhasePoint = -rho * T;
    double halfPhasePoint = (k - rho) * T;
    double endPhasePoint = (1 - rho) * T;

    double t = 0;  x1->clear();  y1->clear();
    double currAmplitude;
    for(int i = 0; i < dataLength; i++)
    {
        if(startPhasePoint <= 0)
        {
            if(t < halfPhasePoint)
            {
                currAmplitude = dataMax;
            }
            else if(t < endPhasePoint)
            {
                currAmplitude = dataMin;
            }
            else if(t < (halfPhasePoint + T))
            {
                currAmplitude = dataMax;
            }
            else
            {
                currAmplitude = dataMin;
            }
        }
        else
        {
            if(t < startPhasePoint)
            {
                currAmplitude = dataMin;
            }
            else if(t < halfPhasePoint)
            {
                currAmplitude = dataMax;
            }
            else if(t < endPhasePoint)
            {
                currAmplitude = dataMin;
            }
            else
            {
                currAmplitude = dataMax;
            }
        }
        x1->append(t * 1000),  y1->append(currAmplitude);
        t += intval;
    }
}
