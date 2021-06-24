#ifndef SUAREWAVE_H
#define SUAREWAVE_H

#include "abstractwave.h"

class SquareWave : public AbstractWave
{
public:
    SquareWave();
    ~SquareWave();
    void clear();
    void update(QHash<QString, double>*, QVector<double>*,
                QVector<double>*) override;
};

#endif // SUAREWAVE_H
