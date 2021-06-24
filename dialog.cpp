#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>

//注意这里为了调用父对象的方法获取数据
//改变了默认的子窗口构造函数
Dialog::Dialog(Widget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    //为对话框添加最大/小化
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    ui->comboBoxBaud->setCurrentIndex(4);
    ui->comboBoxDatabit->setCurrentIndex(3);
    ui->comboBoxStopbit->setCurrentIndex(0);
    ui->comboBoxParity->setCurrentIndex(0);
    ui->comboBoxFlowControl->setCurrentIndex(0);

    //串口参数表初始化
    portParaMap.insert("baud", 9600);
    portParaMap.insert("dataBit", 8);
    portParaMap.insert("stopBit", 1);
    portParaMap.insert("parity", 0);
    portParaMap.insert("flowControl", 0);

    //准备数据，画图时画出的是1.5T，这里只需要一个周期的
    series = parent->getSeries();
    dataLength = floor(parent->getDataLength() * 2 / 3);
    frequency = parent->getFrequency();

    //注意线程对象、信号和connect写的位置
    worker = new SendDataThread(series, frequency, dataLength);
    connect(this, SIGNAL(openPort(QString, const QHash<QString, int>*)),
            worker, SLOT(openWorkerPort(QString, const QHash<QString, int>*)));

    connect(this, SIGNAL(closePort()), worker, SLOT(closeWorkerPort()));

    connect(worker, SIGNAL(openPortFail(QString)), this, SLOT(PortFail(QString)));

    connect(worker, SIGNAL(workerBytesWritten(double)), this,
            SLOT(updateText(double)));

    connect(this, SIGNAL(startWorkerSend()), worker, SLOT(startSend()));

    connect(this, SIGNAL(quitDialog()), worker, SLOT(quitThread()));
}

Dialog::~Dialog()
{
    delete ui;
    delete series;
    delete worker;
}

void Dialog::closeEvent(QCloseEvent *event)
{
    emit quitDialog();
    //worker->SerialOperator->deleteLater();
    worker->SerialOperator->wait();
    event->accept();
}

void Dialog::PortFail(QString error)
{
    QMessageBox::warning(this, tr("警告"),
                             tr("打开端口失败: ") + error);
}

void Dialog::updateText(double newData)
{
    byteLength++;
    ui->textEditSend->append(QString::number(newData));
    ui->label_9->setText("数据总数：" + QString::number(byteLength));
}

void Dialog::on_pushButtonStartStop_clicked()
{    
    if(!isOnCommunication)
    {
        portParaMap["baud"] = baudRate;
        portParaMap["dataBit"] = dataBit;
        portParaMap["stopBit"] = stopBit;
        portParaMap["parity"] = parity;
        portParaMap["flowControl"] = flowControl;

        emit openPort(portName, &portParaMap);
        isOnCommunication = true;
    }
    else
    {
        emit closePort();
        isOnCommunication = false;
    }

    ui->comboBoxPortName->setEnabled(!isOnCommunication);
    ui->comboBoxBaud->setEnabled(!isOnCommunication);
    ui->comboBoxDatabit->setEnabled(!isOnCommunication);
    ui->comboBoxStopbit->setEnabled(!isOnCommunication);
    ui->comboBoxParity->setEnabled(!isOnCommunication);
    ui->comboBoxFlowControl->setEnabled(!isOnCommunication);
}

void Dialog::on_pushButtonSend_clicked()
{
    if(!isOnCommunication)
    {
        QMessageBox::warning(this, tr("警告"),
                                 tr("尚未打开端口: "));
        return;
    }
    emit startWorkerSend();
}

void Dialog::on_comboBoxBaud_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0: baudRate = QSerialPort::Baud115200; break;
        case 1: baudRate = QSerialPort::Baud57600; break;
        case 2: baudRate = QSerialPort::Baud38400; break;
        case 3: baudRate = QSerialPort::Baud19200; break;
        case 4: baudRate = QSerialPort::Baud9600; break;
        case 5: baudRate = QSerialPort::Baud4800; break;
        case 6: baudRate = QSerialPort::Baud2400; break;
        case 7: baudRate = QSerialPort::Baud1200; break;
        default: break;
    }
}

void Dialog::on_comboBoxDatabit_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0: dataBit = QSerialPort::Data5; break;
        case 1: dataBit = QSerialPort::Data6; break;
        case 2: dataBit = QSerialPort::Data7; break;
        case 3: dataBit = QSerialPort::Data8; break;
        default: break;
    }
}

void Dialog::on_comboBoxStopbit_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0: stopBit = QSerialPort::OneStop; break;
        case 1: stopBit = QSerialPort::TwoStop; break;
        case 2: stopBit = QSerialPort::OneAndHalfStop; break;
        default: break;
    }
}

void Dialog::on_comboBoxParity_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0: parity = QSerialPort::NoParity; break;
        case 1: parity = QSerialPort::EvenParity; break;
        case 2: parity = QSerialPort::OddParity; break;
        case 3: parity = QSerialPort::MarkParity; break;
        case 4: parity = QSerialPort::SpaceParity; break;
        default: break;
    }
}

void Dialog::on_comboBoxFlowControl_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0: flowControl = QSerialPort::NoFlowControl; break;
        case 1: flowControl = QSerialPort::SoftwareControl; break;
        case 2: flowControl = QSerialPort::HardwareControl; break;
        default : break;
    }
}

void Dialog::on_pushButtonFindPort_clicked()
{
    ui->comboBoxPortName->clear();
    //通过QSerialPortInfo查找可用串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->comboBoxPortName->addItem(info.portName());
    }

    if(ui->comboBoxPortName->count() == 0)
    {
        QMessageBox::warning(this, tr("警告"),
                                 tr("未找到可用端口: "));
        return;
    }
}

void Dialog::on_comboBoxPortName_currentIndexChanged(const QString &arg1)
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if(info.portName() == arg1)
        {
            //portInfo = &info;
            portName = info.portName();
        }
    }
}

void Dialog::on_pushButtonClearSend_clicked()
{
    ui->textEditSend->clear();
}
