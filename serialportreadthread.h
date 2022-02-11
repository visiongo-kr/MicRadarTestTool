#ifndef SERIALPORTREADTHREAD_H
#define SERIALPORTREADTHREAD_H

#include <QObject>
#include <QThread>
#include <QSerialPort>

class SerialPortReadThread : public QThread
{
    Q_OBJECT
public:
    /* Read status */
    enum serialPortReadStatus {
        READHEAD        = 0,    // read frame head
        READHEADDATA    = 1,    // read frame head data
        READDATA        = 2,    // read frame data (data, check and tail)
        READTAIL        = 3,    // read frame tail
    };

    /* R77ABH1 Frame data */
    struct tlvData {
        uint8_t tlvFlag;
        uint8_t objDistance;
        uint8_t objDirection;
        uint8_t status;
        uint8_t breatheValue;
        uint8_t heartRate;
        uint8_t breatheLine[20];
        uint8_t heartLine[20];
    };
    struct dataFramR77abh1 {
        uint8_t head[4];
        uint8_t length;
        uint8_t workMode;
        uint16_t workTime;
        uint8_t tlvNum;
        uint8_t runParm[3];
        uint16_t reserve;
        struct tlvData tlv[2];
        uint16_t crc;
        uint16_t frameTail;
    };

    /* R60ABH1 Frame data */
    struct dataFrameR60abh1 {
        uint8_t head[2];
        uint8_t control;
        uint8_t command;
        uint16_t dataLength;
        uint8_t data[2048];
        uint8_t check;
        uint8_t tail[2];
    };

    SerialPortReadThread(QSerialPort *serialPort);
    void setDevice(volatile int value);

    void setIsRunning(bool value);

private:
    QSerialPort *serialPort;
    bool isRunning;
    volatile int device;
    enum serialPortReadStatus recvStatus;
    struct dataFramR77abh1 frameR77abh1;
    struct dataFrameR60abh1 frameR60abh1;
protected:
    virtual void run();
signals:
    void r60abh1ReceiveFrame(struct dataFrameR60abh1 frame);
};

#endif // SERIALPORTREADTHREAD_H
