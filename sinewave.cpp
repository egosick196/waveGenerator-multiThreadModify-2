#include "sinewave.h"

SineWave::SineWave(): AbstractWave()
{
}


SineWave::~SineWave() {}

void SineWave::update(QHash<QString, double> *paraMap,
                      QVector<double> *x1, QVector<double>*y1)
{
    //首先更新参数
    AbstractWave::update(paraMap);

    //计算中间参数
    double k = 2 * M_PI * frequency;
    double T = 1 / frequency;
    double rho = 2 * M_PI * phase / 360;
    double intval = 1.5 * T / (dataLength - 1);
    double t = 0;  x1->clear();  y1->clear();
    double currAmplitude = 0, currentX = 0;
    for(int i = 0; i < dataLength; i++)
    {
        currentX = k * t + rho;
        currAmplitude = amplitude * qSin(currentX) + offset;
        x1->append(t * 1000),  y1->append(currAmplitude);
        t += intval;
    }
}
