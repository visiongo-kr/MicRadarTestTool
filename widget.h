#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <windows.h>
#include <dbt.h>
#include <stdint.h>
#include <QLineSeries>

#include "linechartview.h"
#include "serialportreadthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QSerialPort *serialPort0;
    QSerialPort *serialPort1;
    SerialPortReadThread *serialPortThread;
    LineChartView *r60abh1HeartChartView;
    QChart *r60abh1HeartChart;
    QLineSeries *r60abh1HeartSeries;

    void initUi();
    void initLineChartView();
protected:
    bool winEvent(MSG *msg, long *result);

private slots:
    void handelSerialPort0();
    void pushButtonSerialPort0Connect();
    void pushButtonSerialPort1Connect();
    void onTabWidgetCurrentChanged(int index);
    void onSerialPortThreadRecvR60abh1Frame(SerialPortReadThread::dataFrameR60abh1 frame);
};
#endif // WIDGET_H
