#ifndef SENDDATATHREAD_H
#define SENDDATATHREAD_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QHash>
#include <QVector>

class SendDataThread : public QObject
{
    Q_OBJECT
public:
    SendDataThread(QVector<double>*, double, int);
    ~SendDataThread();
    QThread *SerialOperator;

public slots:
    void openWorkerPort(QString, const QHash<QString, int>*);
    void startSend();
    void closeWorkerPort();
    void quitThread();

signals:
    void openPortFail(QString);
    void workerBytesWritten(double);

private:
    QVector<double> *series;
    QSerialPort *port;
    double frequency;
    int dataLength;
    int flag = 0;

    QString portName;
    QHash<QString, int> paramsMap;
};

#endif // SENDDATATHREAD_H
