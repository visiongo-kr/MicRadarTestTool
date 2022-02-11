#include "widget.h"
#include "ui_widget.h"

#include <QSerialPortInfo>
#include <QDebug>
#include <QChart>
#include <QLineSeries>

QT_CHARTS_USE_NAMESPACE

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
    QStringList serialPortNames;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()) {
        serialPortNames << info.portName();
    }
    this->ui->comboBoxSerialPorts0Name->addItems(serialPortNames);
    this->ui->comboBoxSerialPorts1Name->addItems(serialPortNames);
    /* init ui */
    initUi();
    /* init chart view */
    initLineChartView();
    /* connect */
    connect(this->ui->pushButtonSerialPort0Connect, &QPushButton::released, this, &Widget::pushButtonSerialPort0Connect);
    connect(this->ui->pushButtonSerialPort1Connect, &QPushButton::released, this, &Widget::pushButtonSerialPort1Connect);
    connect(this->ui->tabWidget, &QTabWidget::currentChanged, this, &Widget::onTabWidgetCurrentChanged);
    qRegisterMetaType<SerialPortReadThread::dataFrameR60abh1>("dataFrameR60abh1");
    connect(serialPortThread, &SerialPortReadThread::r60abh1ReceiveFrame, this, &Widget::onSerialPortThreadRecvR60abh1Frame);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initUi()
{
    /* set visible */
    this->ui->widgetSerial1->setVisible(false);
    QLineSeries *series = new QLineSeries();

    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("折线图");

    ui->graphicsViewB60abh1HeartRate->setChart(chart);
    ui->graphicsViewB60abh1HeartRate->setRenderHint(QPainter::Antialiasing);
}

void Widget::initLineChartView()
{
    /* init data */
    r60abh1HeartChart = new QChart;
    r60abh1HeartSeries = new QLineSeries;
    /* init */
    r60abh1HeartSeries->setUseOpenGL(true);
    r60abh1HeartChart->addSeries(r60abh1HeartSeries);
    r60abh1HeartChart->createDefaultAxes();
    r60abh1HeartChart->axisY()->setRange(0, 256);
    r60abh1HeartChart->axisX()->setRange(0, 50);
    r60abh1HeartChart->legend()->hide();
    this->ui->graphicsViewB60abh1HeartRate->setLineChart(r60abh1HeartChart);
}

bool Widget::winEvent(MSG *msg, long *result)
{
    int msgType = msg->message;
    if(msgType == WM_DEVICECHANGE) {
        qDebug() << "receive event";
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
        switch(msg->wParam) {
        case DBT_DEVICETYPESPECIFIC:
            qDebug() << "DBT_DEVICETYPESPECIFIC " ;
            break;
        case DBT_DEVICEARRIVAL:
            if (lpdb -> dbch_devicetype == DBT_DEVTYP_VOLUME) {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                if (lpdbv -> dbcv_flags == 0) {
                    // do some thing
                    /* update serial port info */
                    QStringList serialPortNames;
                    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()) {
                        serialPortNames << info.portName();
                    }
                    this->ui->comboBoxSerialPorts0Name->clear();
                    this->ui->comboBoxSerialPorts0Name->addItems(serialPortNames);
                    this->ui->comboBoxSerialPorts1Name->addItems(serialPortNames);
                }
            }
            qDebug() << "DBT_DEVICEARRIVAL" ;
            break;
        case DBT_DEVICEREMOVECOMPLETE:
            if (lpdb -> dbch_devicetype == DBT_DEVTYP_VOLUME) {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                if (lpdbv -> dbcv_flags == 0) {
                    qDebug() << "USB_DEVICE Remove";
                }
            }
                qDebug() << "DBT_DEVICEREMOVECOMPLETE" ;
            break;
        }
    }

    return false;
}

void Widget::handelSerialPort0()
{
//    static struct dataFrameR60abh1 frameR60abh1;
//    static struct dataFramR77abh1 frameR77abh1;
//    static enum serialPortReadStatus status = READHEAD;
//    static int needReadLength = 0;

//    if (needReadLength != 0) {

//    }
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
        serialPortThread->setIsRunning(false);
        serialPortThread->wait();
        this->serialPort0->close();
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
}

void Widget::onSerialPortThreadRecvR60abh1Frame(SerialPortReadThread::dataFrameR60abh1 frame)
{
//    qDebug() << frame.control << "-" << frame.command;
    switch (frame.control) {
    case 0x81:
        switch (frame.command) {
        case 0x01:
            break;
        case 0x02:
            this->ui->lcdNumberHeartRateValue->display(frame.data[0]);
            break;
        case 0x03:
            qreal x = r60abh1HeartSeries->pointsVector().length();
            qreal y = frame.data[0];
            this->r60abh1HeartSeries->append(x, y);
            break;
        }
        break;
    }
}

