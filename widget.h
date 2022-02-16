#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <windows.h>
#include <dbt.h>
#include <stdint.h>
#include <QLineSeries>
#include <QTimer>

#include "linechartview.h"
#include "serialportreadthread.h"
#include "circularqueue.h"

#define B60ABH1HEART_Y_MAX          256
#define B60ABH1HEART_Y_MIN          0
#define B60ABH1HEART_X_MAX          500
#define B60ABH1HEART_X_MIN          0
#define B60ABH1BREATHE_Y_MAX        256
#define B60ABH1BREATHE_Y_MIN        0
#define B60ABH1BREATHE_X_MAX        500
#define B60ABH1BREATHE_X_MIN        0
#define R77ABH1TLV1HEART_Y_MAX      130
#define R77ABH1TLV1HEART_Y_MIN      -128
#define R77ABH1TLV1HEART_X_MAX      500
#define R77ABH1TLV1HEART_X_MIN      0
#define R77ABH1TLV1BREATHE_Y_MAX    130
#define R77ABH1TLV1BREATHE_Y_MIN    -128
#define R77ABH1TLV1BREATHE_X_MAX    500
#define R77ABH1TLV1BREATHE_X_MIN    0
#define R77ABH1TLV2HEART_Y_MAX      130
#define R77ABH1TLV2HEART_Y_MIN      -128
#define R77ABH1TLV2HEART_X_MAX      500
#define R77ABH1TLV2HEART_X_MIN      0
#define R77ABH1TLV2BREATHE_Y_MAX    130
#define R77ABH1TLV2BREATHE_Y_MIN    -128
#define R77ABH1TLV2BREATHE_X_MAX    500
#define R77ABH1TLV2BREATHE_X_MIN    0

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

    QChart *b60abh1HeartChart;
    QLineSeries *b60abh1HeartSeries;
    QChart *b60abh1BreatheChart;
    QLineSeries *b60abh1BreatheSeries;

    QChart *r77abh1Tlv1HeartChart;
    QLineSeries *r77abh1Tlv1HeartSeries;
    QChart *r77abh1Tlv1BreatheChart;
    QLineSeries *r77abh1Tlv1BreatheSeries;

    QChart *r77abh1Tlv2HeartChart;
    QLineSeries *r77abh1Tlv2HeartSeries;
    QChart *r77abh1Tlv2BreatheChart;
    QLineSeries *r77abh1Tlv2BreatheSeries;

    QTimer *lineChartViewTimer;
    CircularQueue<char> *r77abh1Tlv1HeartQueue;
    CircularQueue<char> *r77abh1Tlv1BreatheQueue;
    CircularQueue<char> *r77abh1Tlv2HeartQueue;
    CircularQueue<char> *r77abh1Tlv2BreatheQueue;

    void initUi();
    void initLineChartView();
    void updateChartView(QChart *chart, QLineSeries *series,size_t size);
    void updateSerialPortInfo();
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
private slots:
    void handelSerialPort0();
    void pushButtonSerialPort0Connect();
    void pushButtonSerialPort1Connect();
    void onTabWidgetCurrentChanged(int index);
    void onSerialPortThreadRecvB60abh1Frame(SerialPortReadThread::dataFrameB60abh1 frame);
    void onSerialPortThreadRecvR77abh1Frame(SerialPortReadThread::dataFrameR77abh1 frame);
    void onQtimeOutUpdateLineChartView();
};
#endif // WIDGET_H
