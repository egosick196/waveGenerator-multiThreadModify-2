#ifndef ABSTRACTWAVE_H
#define ABSTRACTWAVE_H

#include <QObject>
#include <QHash>
#include <QVector>

class AbstractWave: public QObject
{
    Q_OBJECT
public:
    explicit AbstractWave(QObject *parent = nullptr);
    ~AbstractWave();
    virtual void update(QHash<QString, double> *,
                        QVector<double>* = new QVector<double>(),
                        QVector<double>* = new QVector<double>());

protected:
    double frequency = 1000.00;  //单位为Hz
    double amplitude = 5.0;  //单位为Vpp
    double offset = 0.0;
    double phase = 0.0;
    double dataMin = -amplitude/2 + offset;
    double dataMax = amplitude/2 + offset;  //数据值的范围
    double dutyRatio = 50.0;  //占空比
    int dataLength = 100;  //数据点数量
};

#endif // ABSTRACTWAVE_H
