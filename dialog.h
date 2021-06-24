#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "widget.h"
#include "senddatathread.h"
#include <QCloseEvent>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(Widget *parent = nullptr);
    ~Dialog();

private slots:
    void on_pushButtonSend_clicked();

    void on_comboBoxBaud_currentIndexChanged(int index);

    void on_pushButtonStartStop_clicked();

    void on_comboBoxDatabit_currentIndexChanged(int index);

    void on_comboBoxStopbit_currentIndexChanged(int index);

    void on_comboBoxParity_currentIndexChanged(int index);

    void on_comboBoxFlowControl_currentIndexChanged(int index);

    void on_pushButtonFindPort_clicked();

    void on_comboBoxPortName_currentIndexChanged(const QString &arg1);

    void on_pushButtonClearSend_clicked();

signals:
    void openPort(QString, const QHash<QString, int>*);
    void startWorkerSend();
    void closePort();
    void quitDialog();

public slots:
    void PortFail(QString);
    void updateText(double);

private:
    Ui::Dialog *ui;
    QVector<double> *series;

    int dataLength, byteLength = 0;
    double frequency;
    bool isOnCommunication = false;

    QString portName = "";
    int baudRate = 9600;
    int dataBit = 8;
    int stopBit = 1;
    int parity = 0;
    int flowControl = 0;
    QHash<QString, int>portParaMap;

    SendDataThread *worker;
    void closeEvent(QCloseEvent *event);    // 重写closeEvent的申明
};

#endif // DIALOG_H
