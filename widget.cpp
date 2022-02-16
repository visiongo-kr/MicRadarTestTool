#include "widget.h"
#include "ui_widget.h"

#include <QSerialPortInfo>
#include <QDebug>
#include <QChart>
#include <QLineSeries>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    /* init serial port */
    serialPort0 = new QSerialPort();
    serialPort1 = new QSerialPort();
    /* init serial port thread */
    serialPortThread = new SerialPortReadThread(serialPort0);
    /* update serial port info */
    updateSerialPortInfo();
    /* init ui */
    initUi();
    /* init chart view */
    initLineChartView();
    /* init circular queue */
    r77abh1Tlv1HeartQueue = new CircularQueue<char>(1024);
    r77abh1Tlv1BreatheQueue = new CircularQueue<char>(1024);
    r77abh1Tlv2HeartQueue = new CircularQueue<char>(1024);
    r77abh1Tlv2BreatheQueue = new CircularQueue<char>(1024);
    /* timer */
    lineChartViewTimer = new QTimer;
    lineChartViewTimer->start(40);
    /* connect */
    connect(this->ui->pushButtonSerialPort0Connect, &QPushButton::released, this, &Widget::pushButtonSerialPort0Connect);
    connect(this->ui->pushButtonSerialPort1Connect, &QPushButton::released, this, &Widget::pushButtonSerialPort1Connect);

    connect(this->ui->tabWidget, &QTabWidget::currentChanged, this, &Widget::onTabWidgetCurrentChanged);

    qRegisterMetaType<SerialPortReadThread::dataFrameB60abh1>("dataFrameB60abh1");
    connect(serialPortThread, &SerialPortReadThread::b60abh1ReceiveFrame, this, &Widget::onSerialPortThreadRecvB60abh1Frame);

    qRegisterMetaType<SerialPortReadThread::dataFrameR77abh1>("dataFrameR77abh1");
    connect(serialPortThread, &SerialPortReadThread::r77abh1ReceiveFrame, this, &Widget::onSerialPortThreadRecvR77abh1Frame);

    connect(lineChartViewTimer, &QTimer::timeout, this, &Widget::onQtimeOutUpdateLineChartView);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initUi()
{
    /* set visible */
    this->ui->widgetSerial1->setVisible(false);
    this->ui->groupBoxR77abh1Tlv2->hide();
}

void Widget::initLineChartView()
{
    /* init data */
    // b60abh1
    b60abh1HeartChart = new QChart;
    b60abh1HeartSeries = new QLineSeries;

    b60abh1BreatheChart = new QChart;
    b60abh1BreatheSeries = new QLineSeries;
    // r77abh1 tlv1
    r77abh1Tlv1HeartChart = new QChart;
    r77abh1Tlv1HeartSeries = new QLineSeries;

    r77abh1Tlv1BreatheChart = new QChart;
    r77abh1Tlv1BreatheSeries = new QLineSeries;
    // r77abh1 tlv2
    r77abh1Tlv2HeartChart = new QChart;
    r77abh1Tlv2HeartSeries = new QLineSeries;

    r77abh1Tlv2BreatheChart = new QChart;
    r77abh1Tlv2BreatheSeries = new QLineSeries;
    /* init */
    // b60abh1
    b60abh1HeartSeries->setUseOpenGL(true);
    b60abh1HeartChart->addSeries(b60abh1HeartSeries);
    b60abh1HeartChart->createDefaultAxes();
    b60abh1HeartChart->axes(Qt::Vertical).back()->setRange(B60ABH1HEART_Y_MIN, B60ABH1HEART_Y_MAX);
    b60abh1HeartChart->axes(Qt::Horizontal).back()->setRange(B60ABH1HEART_X_MIN, B60ABH1HEART_X_MAX);
    b60abh1HeartChart->legend()->hide();
    b60abh1HeartChart->axes(Qt::Horizontal).back()->hide();
    b60abh1HeartChart->axes(Qt::Vertical).back()->hide();
    this->ui->graphicsViewB60abh1HeartRate->setLineChart(b60abh1HeartChart);

    b60abh1BreatheSeries->setUseOpenGL(true);
    b60abh1BreatheChart->addSeries(b60abh1BreatheSeries);
    b60abh1BreatheChart->createDefaultAxes();
    b60abh1BreatheChart->axes(Qt::Vertical).back()->setRange(B60ABH1BREATHE_Y_MIN, B60ABH1BREATHE_Y_MAX);
    b60abh1BreatheChart->axes(Qt::Horizontal).back()->setRange(B60ABH1BREATHE_X_MIN, B60ABH1BREATHE_X_MAX);
    b60abh1BreatheChart->legend()->hide();
    b60abh1BreatheChart->axes(Qt::Horizontal).back()->hide();
    b60abh1BreatheChart->axes(Qt::Vertical).back()->hide();
    this->ui->graphicsViewB60abh1Breathe->setLineChart(b60abh1BreatheChart);
    // r77abh1 tlv1
    r77abh1Tlv1HeartSeries->setUseOpenGL(true);
    r77abh1Tlv1HeartChart->addSeries(r77abh1Tlv1HeartSeries);
    r77abh1Tlv1HeartChart->createDefaultAxes();
    r77abh1Tlv1HeartChart->axes(Qt::Vertical).back()->setRange(R77ABH1TLV1HEART_Y_MIN, R77ABH1TLV1HEART_Y_MAX);
    r77abh1Tlv1HeartChart->axes(Qt::Horizontal).back()->setRange(R77ABH1TLV1HEART_X_MIN, R77ABH1TLV1HEART_X_MAX);
    r77abh1Tlv1HeartChart->legend()->hide();
    r77abh1Tlv1HeartChart->axes(Qt::Horizontal).back()->hide();
    r77abh1Tlv1HeartChart->axes(Qt::Vertical).back()->hide();
    this->ui->graphicsViewR77abh1Tlv1HeartRate->setLineChart(r77abh1Tlv1HeartChart);

    r77abh1Tlv1BreatheSeries->setUseOpenGL(true);
    r77abh1Tlv1BreatheChart->addSeries(r77abh1Tlv1BreatheSeries);
    r77abh1Tlv1BreatheChart->createDefaultAxes();
    r77abh1Tlv1BreatheChart->axes(Qt::Vertical).back()->setRange(R77ABH1TLV1BREATHE_Y_MIN, R77ABH1TLV1BREATHE_Y_MAX);
    r77abh1Tlv1BreatheChart->axes(Qt::Horizontal).back()->setRange(R77ABH1TLV1BREATHE_X_MIN, R77ABH1TLV1BREATHE_X_MAX);
    r77abh1Tlv1BreatheChart->legend()->hide();
    r77abh1Tlv1BreatheChart->axes(Qt::Horizontal).back()->hide();
    r77abh1Tlv1BreatheChart->axes(Qt::Vertical).back()->hide();
    this->ui->graphicsViewR77abh1Tlv1Breathe->setLineChart(r77abh1Tlv1BreatheChart);
    // r77abh1 Tlv2
    r77abh1Tlv2HeartSeries->setUseOpenGL(true);
    r77abh1Tlv2HeartChart->addSeries(r77abh1Tlv2HeartSeries);
    r77abh1Tlv2HeartChart->createDefaultAxes();
    r77abh1Tlv2HeartChart->axes(Qt::Vertical).back()->setRange(R77ABH1TLV2HEART_Y_MIN, R77ABH1TLV2HEART_Y_MAX);
    r77abh1Tlv2HeartChart->axes(Qt::Horizontal).back()->setRange(R77ABH1TLV2HEART_X_MIN, R77ABH1TLV2HEART_X_MAX);
    r77abh1Tlv2HeartChart->legend()->hide();
    r77abh1Tlv2HeartChart->axes(Qt::Horizontal).back()->hide();
    r77abh1Tlv2HeartChart->axes(Qt::Vertical).back()->hide();
    this->ui->graphicsViewR77abh1Tlv2HeartRate->setLineChart(r77abh1Tlv2HeartChart);

    r77abh1Tlv2BreatheSeries->setUseOpenGL(true);
    r77abh1Tlv2BreatheChart->addSeries(r77abh1Tlv2BreatheSeries);
    r77abh1Tlv2BreatheChart->createDefaultAxes();
    r77abh1Tlv2BreatheChart->axes(Qt::Vertical).back()->setRange(R77ABH1TLV2BREATHE_Y_MIN, R77ABH1TLV2BREATHE_Y_MAX);
    r77abh1Tlv2BreatheChart->axes(Qt::Horizontal).back()->setRange(R77ABH1TLV2BREATHE_X_MIN, R77ABH1TLV2BREATHE_X_MAX);
    r77abh1Tlv2BreatheChart->legend()->hide();
    r77abh1Tlv2BreatheChart->axes(Qt::Horizontal).back()->hide();
    r77abh1Tlv2BreatheChart->axes(Qt::Vertical).back()->hide();
    this->ui->graphicsViewR77abh1Tlv2Breathe->setLineChart(r77abh1Tlv2BreatheChart);
}

void Widget::updateChartView(QChart *chart, QLineSeries *series,size_t size)
{
    if ((size_t)series->pointsVector().length() > size) {
        chart->axes(Qt::Horizontal).back()->setMin(series->pointsVector().length() - size - 1);
        chart->axes(Qt::Horizontal).back()->setMax(series->pointsVector().length() - 1);
    }
}

void Widget::updateSerialPortInfo()
{
    static QStringList preSerialPortNames;
    QStringList serialPortNames;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()) {
        serialPortNames << info.portName();
    }
    if (preSerialPortNames == serialPortNames) {
        return;
    }
    preSerialPortNames = serialPortNames;
    this->ui->comboBoxSerialPorts0Name->clear();
    this->ui->comboBoxSerialPorts1Name->clear();
    this->ui->comboBoxSerialPorts0Name->addItems(serialPortNames);
    this->ui->comboBoxSerialPorts1Name->addItems(serialPortNames);
}

bool Widget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if(eventType == QByteArray("windows_generic_MSG")) {
        MSG *pMsg = reinterpret_cast<MSG*>(message);

        if(pMsg->message == WM_DEVICECHANGE) {
            switch(pMsg->wParam) {
            /* device is connected */
            case DBT_DEVICEARRIVAL:
                qDebug() << "device is connected";
                updateSerialPortInfo();
                break;
            /* devices disconnected */
            case DBT_DEVICEREMOVECOMPLETE:
                qDebug() << "devices disconnected";
                updateSerialPortInfo();
                if (serialPort0->isOpen()) {
                    foreach(const QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
                        qDebug() << info.portName();
                        if(info.portName() == serialPort0->portName()) {
                            return false;
                        } else {
                            pushButtonSerialPort0Connect();
                        }
                    }
                }
                break;
            }
        }
    }

    return false;
}

void Widget::handelSerialPort0()
{
//    static struct dataFrameB60abh1 frameB60abh1;
//    static struct dataFramR77abh1 frameR77abh1;
//    static enum serialPortReadStatus status = READHEAD;
//    static int needReadLength = 0;

//    if (needReadLength != 0) {

//    }
    serialPortThread->start();
}

void Widget::pushButtonSerialPort0Connect()
{
    if (this->ui->pushButtonSerialPort0Connect->text() == "Connect") {
        /* port name */
        this->serialPort0->setPortName(this->ui->comboBoxSerialPorts0Name->currentText());
        /* connect to serial port */
        if (this->ui->comboBoxSerialPorts0BauRate->currentText() == "115200") {
            this->serialPort0->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections);
        } else if (this->ui->comboBoxSerialPorts0BauRate->currentText() == "9600") {
            this->serialPort0->setBaudRate(QSerialPort::Baud9600, QSerialPort::AllDirections);
        }
        this->serialPort0->setDataBits(QSerialPort::Data8);
        this->serialPort0->setFlowControl(QSerialPort::NoFlowControl);
        this->serialPort0->setParity(QSerialPort::NoParity);
        this->serialPort0->setStopBits(QSerialPort::OneStop);
        if (this->serialPort0->open(QIODevice::ReadWrite)) {
            this->ui->pushButtonSerialPort0Connect->setText("DisConnect");
            serialPortThread->start();
        }
    } else {
        if (serialPortThread->isRunning()) {
            serialPortThread->setThreadStatus(false);
            serialPortThread->wait();
            this->serialPort0->close();
        }
        this->ui->pushButtonSerialPort0Connect->setText("Connect");
    }
}

void Widget::pushButtonSerialPort1Connect()
{
    if (this->ui->pushButtonSerialPort1Connect->text() == "Connect") {
        /* port name */
        this->serialPort1->setPortName(this->ui->comboBoxSerialPorts1Name->currentText());
        /* connect to serial port */
        if (this->ui->comboBoxSerialPorts1BauRate->currentText() == "115200") {
            this->serialPort1->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections);
        } else if (this->ui->comboBoxSerialPorts1BauRate->currentText() == "9600") {
            this->serialPort1->setBaudRate(QSerialPort::Baud9600, QSerialPort::AllDirections);
        }
        this->serialPort1->setDataBits(QSerialPort::Data8);
        this->serialPort1->setFlowControl(QSerialPort::NoFlowControl);
        this->serialPort1->setParity(QSerialPort::NoParity);
        this->serialPort1->setStopBits(QSerialPort::OneStop);
        if (this->serialPort1->open(QIODevice::ReadWrite)) {
            this->ui->pushButtonSerialPort1Connect->setText("DisConnect");
        }
    } else {
        this->serialPort1->close();
        this->ui->pushButtonSerialPort1Connect->setText("Connect");
    }
}


void Widget::onTabWidgetCurrentChanged(int index)
{
    if (this->serialPort0->isOpen()) {
        pushButtonSerialPort0Connect();
    }
    if (index == 2) {
        this->ui->widgetSerial1->setVisible(true);
    } else {
        if (this->serialPort1->isOpen()) {
            pushButtonSerialPort1Connect();
        }
        this->ui->widgetSerial1->setVisible(false);
    }
    serialPortThread->setDevice(index);
}

void Widget::onSerialPortThreadRecvB60abh1Frame(SerialPortReadThread::dataFrameB60abh1 frame)
{
    switch (frame.control) {
    case 0x81:
        switch (frame.command) {
        case 0x01:
            break;
        case 0x02:
            this->ui->lcdNumberB60abh1HeartRateValue->display(frame.data[0]);
            break;
        case 0x03:
            this->b60abh1HeartSeries->append(b60abh1HeartSeries->pointsVector().length(), frame.data[0]);
            updateChartView(this->b60abh1HeartChart, this->b60abh1HeartSeries, B60ABH1HEART_X_MAX);
            break;
        case 0x04:
            break;
        case 0x05:
            this->ui->lcdNumberB60abh1BreatheValue->display(frame.data[0]);
            break;
        case 0x06:
            this->b60abh1BreatheSeries->append(b60abh1BreatheSeries->pointsVector().length(), frame.data[0]);
            updateChartView(this->b60abh1BreatheChart, this->b60abh1BreatheSeries, B60ABH1BREATHE_X_MAX);
        default:
            break;
        }
        break;
    }
}

void Widget::onSerialPortThreadRecvR77abh1Frame(SerialPortReadThread::dataFrameR77abh1 frame)
{
    if (frame.tlvNum == 1) {
        this->ui->groupBoxR77abh1Tlv2->hide();
    } else {
        this->ui->groupBoxR77abh1Tlv2->show();
    }
    for (int tlvi = 0; tlvi < frame.tlvNum; tlvi++) {
        switch (tlvi) {
        case 0:
            this->ui->lcdNumberR77abh1Tlv1HeartRateValue->display(frame.tlv[0].heartRate);
            this->ui->lcdNumberR77abh1Tlv1BreatheValue->display(frame.tlv[0].breatheValue);
            for (int i = 0; i < 20; i++) {
                r77abh1Tlv1HeartQueue->push(frame.tlv[0].heartRateLine[i]);
                r77abh1Tlv1BreatheQueue->push(frame.tlv[0].breatheLine[i]);
            }
            break;
        case 1:
            this->ui->lcdNumberR77abh1Tlv2HeartRateValue->display(frame.tlv[1].heartRate);
            this->ui->lcdNumberR77abh1Tlv2BreatheValue->display(frame.tlv[1].breatheValue);
            for (int i = 0; i < 20; i++) {
                r77abh1Tlv2HeartQueue->push(frame.tlv[1].heartRateLine[i]);
                r77abh1Tlv2BreatheQueue->push(frame.tlv[1].breatheLine[i]);
            }
            break;
        }
    }
}

void Widget::onQtimeOutUpdateLineChartView()
{
    if (r77abh1Tlv1HeartQueue->length()) {
        this->r77abh1Tlv1HeartSeries->append(r77abh1Tlv1HeartSeries->pointsVector().length(), r77abh1Tlv1HeartQueue->front());
        updateChartView(this->r77abh1Tlv1HeartChart, this->r77abh1Tlv1HeartSeries, R77ABH1TLV1HEART_X_MAX);
        r77abh1Tlv1HeartQueue->pop();
    }
    if (r77abh1Tlv1BreatheQueue->length()) {
        this->r77abh1Tlv1BreatheSeries->append(r77abh1Tlv1BreatheSeries->pointsVector().length(), r77abh1Tlv1BreatheQueue->front());
        updateChartView(this->r77abh1Tlv1BreatheChart, this->r77abh1Tlv1BreatheSeries, R77ABH1TLV1BREATHE_X_MAX);
        r77abh1Tlv1BreatheQueue->pop();
    }
    if (r77abh1Tlv2HeartQueue->length()) {
        this->r77abh1Tlv2HeartSeries->append(r77abh1Tlv2HeartSeries->pointsVector().length(), r77abh1Tlv2HeartQueue->front());
        updateChartView(this->r77abh1Tlv2HeartChart, this->r77abh1Tlv2HeartSeries, R77ABH1TLV2HEART_X_MAX);
        r77abh1Tlv2HeartQueue->pop();
    }
    if (r77abh1Tlv2BreatheQueue->length()) {
        this->r77abh1Tlv2BreatheSeries->append(r77abh1Tlv2BreatheSeries->pointsVector().length(), r77abh1Tlv2BreatheQueue->front());
        updateChartView(this->r77abh1Tlv2BreatheChart, this->r77abh1Tlv2BreatheSeries, R77ABH1TLV2BREATHE_X_MAX);
        r77abh1Tlv2BreatheQueue->pop();
    }
}

