#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QHash>
#include <QMessageBox>
#include <QFileDialog>  //文件对话框
#include <QFile>        //文件类
#include <QDataStream>  //串行化数据流，用于读写 .ds 文件
#include <QTextStream>  //用于列表控件显示的文本和数据转换
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QVector<double> *getSeries();
    int getDataLength();
    double getFrequency();

private slots:
    void on_doubleSpinBoxFreq_editingFinished();

    void on_doubleSpinBoxAmp_editingFinished();

    void on_doubleSpinBoxEmpty_editingFinished();

    void on_doubleSpinBoxOffset_editingFinished();

    void on_doubleSpinBoxPhase_editingFinished();

    void on_spinBoxNum_editingFinished();

    void on_pushButtonDraw_clicked();

    void on_radioButtonRect_clicked();

    void on_radioButtonTriang_clicked();

    void on_radioButtonSin_clicked();

    void on_doubleSpinBoxMinX_editingFinished();

    void on_doubleSpinBoxMaxX_editingFinished();

    void on_doubleSpinBoxMinY_editingFinished();

    void on_doubleSpinBoxMaxY_editingFinished();

    void on_pushButtonSave_clicked();

    void on_pushButtonOpen_clicked();

    void on_pushButtonSerial_clicked();

private:
    Ui::Widget *ui;
    QVector<double> *x1, *y1;
    double minX, maxX, maxY, minY;
    QString waveSelected;
    QHash<QString, double> paramsMap;

    void waveDataGenerate();
    void updateAxis();  //从类参数重设坐标轴
    void updateSpinBox();  //从类参数重设SpinBox
    void initializeChart();
};
#endif // WIDGET_H
