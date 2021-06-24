#ifndef TRIANGLEWAVE_H
#define TRIANGLEWAVE_H
#include "abstractwave.h"
#include <QtMath>
#include "math.h"
#define _USE_MATH_DEFINES

class SineWave : public AbstractWave
{
public:
    SineWave();
    ~SineWave();
    void clear();
    void update(QHash<QString, double>*, QVector<double>*,
                QVector<double>*) override;
};

#endif // TRIANGLEWAVE_H
