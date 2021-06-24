#include "senddatathread.h"
#include <QSerialPortInfo>
#include <QDebug>

SendDataThread::SendDataThread(QVector<double> *controllerSeries,
                               double controllerFrequency,
                               int dataCount)
{
    series = controllerSeries;
    frequency = controllerFrequency;
    dataLength = dataCount;

    SerialOperator = new QThread();
    port = new QSerialPort();
    this->moveToThread(SerialOperator);
    port->moveToThread(SerialOperator);
    SerialOperator->start();
}

SendDataThread::~SendDataThread()
{
    port->close();
    delete port;
    delete SerialOperator;
}

void SendDataThread::quitThread()
{
    port->close();
    SerialOperator->quit();
}

void SendDataThread::openWorkerPort(QString portN,
                                    const QHash<QString, int> *Map)
{
    portName = portN;
    paramsMap = (*Map);

    port->setPortName(portName);
    //serial.setPort(*portInfo);
    port->setBaudRate(paramsMap["baud"]);
    port->setDataBits((QSerialPort::DataBits)paramsMap["dataBit"]);
    port->setStopBits((QSerialPort::StopBits)paramsMap["stopBit"]);
    port->setParity((QSerialPort::Parity)paramsMap["parity"]);
    port->setFlowControl((QSerialPort::FlowControl)paramsMap["flowControl"]);

    if(!port->open(QIODevice::ReadWrite))
    {
        emit openPortFail(port->errorString());
        return;
    }

    port->clearError();  //成功打开后清空原来的错误标志和缓存区
    port->clear();
}

void SendDataThread::closeWorkerPort()
{
    port->close();
}

void SendDataThread::startSend()
{
    //写数据时的中转量，右值不能直接取地址
    double *dataPointer = &frequency;
    uchar cutted;  double rowData;
    QByteArray data;  int frameEnd = 0x13;

    //写频率数据
    data.append(0x12);  //帧头，使用两字节避免与数据冲突
    for(int i = 0; i < 8; i ++)
    {
        cutted = ((uchar*)dataPointer)[i];        
        if(cutted == data[0] || cutted == frameEnd || cutted == 0x7D)
        {
            cutted = cutted ^ 0x20;
            data.append(0x7D);  //协议内容，避免帧头/尾与数据冲突
            qDebug()<<++flag;
        }
        data.append(cutted);
    }
    data.append(0x13);  //帧尾
    port->write(data);
    port->waitForBytesWritten();  //确保至少有一个字节写完
    emit workerBytesWritten(frequency);

    //写数据总量
    data.clear();  data.append(0x12);  //帧头
    double length = dataLength;
    dataPointer = &length;
    for(int i = 0; i < 8; i ++)
    {
        cutted = ((uchar*)dataPointer)[i];
        if(cutted == data[0] || cutted == frameEnd || cutted == 0x7D)
        {
            cutted = cutted ^ 0x20;
            data.append(0x7D);  //协议内容，避免帧头/尾与数据冲突
            qDebug()<<++flag;
        }
        data.append(cutted);
    }
    data.append(0x13);  //帧尾
    port->write(data);
    port->waitForBytesWritten();  //确保至少有一个字节写完
    emit workerBytesWritten(dataLength);

    //double型波形数据转到字节型
    for(int i = 0; i < dataLength; i++)
    {
        data.clear();  data.append(0x12);  //帧头
        rowData = series->at(i);
        dataPointer = &rowData;
        for(int j = 0; j < 8; j++)
        {
            cutted = ((uchar*)dataPointer)[j];
            if(cutted == data[0] || cutted == frameEnd || cutted == 0x7D)
            {
                cutted = cutted ^ 0x20;
                data.append(0x7D);  //协议内容，避免帧头/尾与数据冲突
                qDebug()<<++flag;
            }
            data.append(cutted);
        }
        data.append(0x13);  //帧尾
        port->write(data);
        port->waitForBytesWritten();  //确保至少有一个字节写完
        emit workerBytesWritten(rowData);
    }
}
