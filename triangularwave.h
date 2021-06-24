#ifndef TRIANGULARWAVE_H
#define TRIANGULARWAVE_H
#include "abstractwave.h"

class TriangularWave : public AbstractWave
{
public:
    TriangularWave();
    ~TriangularWave();
    void clear();
    void update(QHash<QString, double>*,
                QVector<double>*, QVector<double>*) override;
};

#endif // TRIANGULARWAVE_H
