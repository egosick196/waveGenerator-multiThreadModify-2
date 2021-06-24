#include "widget.h"
#include "ui_widget.h"
#include "squarewave.h"
#include "sinewave.h"
#include "triangularwave.h"
#include "dialog.h"
#include <QtConcurrent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->radioButtonRect->setChecked(true);
    waveSelected = "SquareWave";
    paramsMap.insert("frequency", 1000.00);
    paramsMap.insert("amplitude", 5.0);
    paramsMap.insert("offset", 0.0);
    paramsMap.insert("phase", 0.0);
    paramsMap.insert("dutyRatio", 50.0);
    paramsMap.insert("dataLength", 1024.00 * 1.5);
    x1 = new QVector<double>();
    y1 = new QVector<double>();

    updateSpinBox();

    ui->chartView->addGraph();
    ui->chartView->xAxis->setLabel("t/ms");
    ui->chartView->yAxis->setLabel("A/V");
    ui->chartView->xAxis->setRange(0, 1);
    ui->chartView->yAxis->setRange(-5, 5);
    ui->chartView->setInteraction(QCP::iRangeDrag, true);
    ui->chartView->replot();

    initializeChart();  //从画出的图表获得属性值，刷新界面显示
    waveDataGenerate();  //生成数据
}

Widget::~Widget()
{
    delete ui;
    delete x1;
    delete y1;
}

void Widget::updateAxis()  //重设坐标轴
{
    ui->chartView->xAxis->setRange(minX, maxX);
    ui->chartView->yAxis->setRange(minY, maxY);
}

void Widget::updateSpinBox()
{
    ui->doubleSpinBoxFreq->setValue(paramsMap["frequency"] / 1000);
    ui->doubleSpinBoxAmp->setValue(paramsMap["amplitude"]);
    ui->doubleSpinBoxOffset->setValue(paramsMap["offset"]);
    ui->doubleSpinBoxPhase->setValue(paramsMap["phase"]);
    ui->doubleSpinBoxEmpty->setValue(paramsMap["dutyRatio"]);
    ui->spinBoxNum->setValue(paramsMap["dataLength"]);
}

void Widget::on_doubleSpinBoxFreq_editingFinished()
{
    paramsMap["frequency"] = (ui->doubleSpinBoxFreq->value()) * 1000;
}

void Widget::on_doubleSpinBoxAmp_editingFinished()
{
    paramsMap["amplitude"] = ui->doubleSpinBoxAmp->value();
}

void Widget::on_doubleSpinBoxEmpty_editingFinished()
{
    paramsMap["dutyRatio"] = ui->doubleSpinBoxEmpty->value();
}

void Widget::on_doubleSpinBoxOffset_editingFinished()
{
    paramsMap["offset"] = ui->doubleSpinBoxOffset->value();
}

void Widget::on_doubleSpinBoxPhase_editingFinished()
{
    paramsMap["phase"] = ui->doubleSpinBoxPhase->value();
}

void Widget::on_spinBoxNum_editingFinished()
{
    paramsMap["dataLength"] = ui->spinBoxNum->value();
}

void Widget::waveDataGenerate()
{
    AbstractWave *dataGenerator = new AbstractWave();

    if(waveSelected == "SquareWave")
    {
        dataGenerator = new SquareWave();
    }
    else if(waveSelected == "TriangleWave")
    {
        dataGenerator = new TriangularWave();
    }
    else if(waveSelected == "SineWave")
    {
        dataGenerator = new SineWave();
    }

    dataGenerator->update(&paramsMap, x1, y1);
    ui->chartView->graph(0)->setData(*x1, *y1);
}

void Widget::initializeChart()  //
{
    double dataMin = -paramsMap["amplitude"] + paramsMap["offset"];
    double dataMax = paramsMap["amplitude"] + paramsMap["offset"];  //数据值的范围
    paramsMap.insert("dataMin", dataMin);
    paramsMap.insert("dataMax", dataMax);

    minX = 0;
    maxX = 1.6 * 1000 / paramsMap["frequency"];
    maxY = dataMax + 0.1 * paramsMap["amplitude"];
    minY = dataMin - 0.1 * paramsMap["amplitude"];

    updateAxis();

    ui->doubleSpinBoxMinX->setValue(0);
    ui->doubleSpinBoxMaxX->setValue(maxX);
    ui->doubleSpinBoxMinY->setValue(minY);
    ui->doubleSpinBoxMaxY->setValue(maxY);
}

void Widget::on_pushButtonDraw_clicked()
{
    initializeChart();
    //注意传的参是函数句柄，没有括号
    QtConcurrent::run(this, &Widget::waveDataGenerate);
    ui->chartView->replot();
}

void Widget::on_radioButtonRect_clicked()
{
    waveSelected = "SquareWave";
}

void Widget::on_radioButtonTriang_clicked()
{
    waveSelected = "TriangleWave";
}

void Widget::on_radioButtonSin_clicked()
{
    waveSelected = "SineWave";
}

void Widget::on_doubleSpinBoxMinX_editingFinished()
{
    minX = ui->doubleSpinBoxMinX->value();
    updateAxis();
}

void Widget::on_doubleSpinBoxMaxX_editingFinished()
{
    maxX = ui->doubleSpinBoxMaxX->value();
    updateAxis();
}

void Widget::on_doubleSpinBoxMinY_editingFinished()
{
    minY = ui->doubleSpinBoxMinY->value();
    updateAxis();
}

void Widget::on_doubleSpinBoxMaxY_editingFinished()
{
    maxY = ui->doubleSpinBoxMaxY->value();
    updateAxis();
}

void Widget::on_pushButtonSave_clicked()
{
    QString newWaveFile = QFileDialog::getSaveFileName(
                this,
                tr("保存波形为.dat通用格式文件"),
                tr("."),
                tr("data files(*.dat);;All files(*)"));
    if(newWaveFile.isEmpty())
        return;

    QFile file(newWaveFile);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("无法打开文件"),
                            tr("无法打开要写入的文件：") + file.errorString());
        return;
    }

    QDataStream ds(&file);
    ds.setByteOrder(QDataStream::LittleEndian);  //设置字节存放顺序
    ds.writeBytes(waveSelected.toUtf8(), waveSelected.length());  //波形种类

    //遍历参数表，写文件
    auto it = paramsMap.constBegin(), itEnd = paramsMap.constEnd();
    for(; it != itEnd; it++)
    {
        QByteArray key = it.key().toUtf8();
        qreal value = it.value();
        ds.writeBytes(key, key.length());
        ds.writeRawData((char*)&value, sizeof(value));
    }
    file.close();
}

void Widget::on_pushButtonOpen_clicked()
{
    QString newWaveFile = QFileDialog::getOpenFileName(
                this,
                tr("打开波形文件"),
                tr("."),
                tr("data files(*.dat);;All files(*)"));
    if(newWaveFile.isEmpty())
        return;

    QFile file(newWaveFile);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("无法打开文件"),
                            tr("无法打开要读取的文件：") + file.errorString());
        return;
    }

    QDataStream ds(&file);
    //设置字节读取顺序
    ds.setByteOrder(QDataStream::LittleEndian);    
    QString key;  qreal value;  uint strLength;  char *buf;
    ds.readBytes(buf, strLength);
    waveSelected = QString::fromLocal8Bit(buf, strLength);  //波形种类

    //更新波形按钮
    if("SquareWave" == waveSelected)
        ui->radioButtonRect->setChecked(true);
    if("TriangleWave" == waveSelected)
        ui->radioButtonTriang->setChecked(true);
    if("SineWave" == waveSelected)
        ui->radioButtonSin->setChecked(true);

    for(int i = 0; i < 8; i++)
    {
        ds.readBytes(buf, strLength);
        key = QString::fromLocal8Bit(buf, strLength);
        ds.readRawData((char*)&value, sizeof (value));
        paramsMap[key] = value;
    }
    file.close();
    updateSpinBox();
    initializeChart();
    waveDataGenerate();
}

QVector<double> *Widget::getSeries()
{
    return y1;
}

int Widget::getDataLength()
{
    return paramsMap["dataLength"];
}

double Widget::getFrequency()
{
    return paramsMap["frequency"];
}

void Widget::on_pushButtonSerial_clicked()
{
    Dialog *newSerialDialog = new Dialog (this);
    newSerialDialog->exec();
    delete newSerialDialog;
}
