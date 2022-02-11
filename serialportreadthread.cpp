#include "serialportreadthread.h"

#include <QDebug>

SerialPortReadThread::SerialPortReadThread(QSerialPort *serialPort)
{
    this->serialPort = serialPort;
    recvStatus = READHEAD;
    isRunning = true;
}

void SerialPortReadThread::setDevice(volatile int value)
{
    device = value;
}

void SerialPortReadThread::setIsRunning(bool value)
{
    isRunning = value;
}

void SerialPortReadThread::run()
{
    static int readLength = 0;
    static uint8_t frameBuffer[4096];

    isRunning = true;
    while (isRunning) {
        if (!serialPort->isOpen()) {
            if (recvStatus != READHEAD) {
                /* init */
                readLength = 0;
                recvStatus = READHEAD;
            }
            continue;
        }
        if (!serialPort->waitForReadyRead(1)) {
            continue;
        }
        char readByte;
        if (serialPort->read(&readByte, 1) <= 0) {
            continue;
        }
        switch (device) {
        case 0:
            /* receive head */
            if (recvStatus == READHEAD) {
                if (readLength == 0 && readByte == 0x53) {
                    frameR60abh1.head[readLength] = readByte;
                    readLength++;
                }
                if (readLength == 1 && readByte == 0x59) {
                    frameR60abh1.head[readLength] = readByte;
                    recvStatus = READHEADDATA;
                    readLength = 0;
                }
            } else if (recvStatus == READHEADDATA) {
                frameBuffer[readLength] = readByte;
                readLength++;
                if (readLength == 4) {
                    frameR60abh1.control = frameBuffer[0];
                    frameR60abh1.command = frameBuffer[1];
                    frameR60abh1.dataLength = frameBuffer[2] * 0xff + frameBuffer[3];
//                    qDebug() << frameBuffer[2];
//                    qDebug() << frameBuffer[3];
                    recvStatus = READDATA;
                    readLength = 0;
                }
            } else if (recvStatus == READDATA) {
                frameBuffer[readLength] = readByte;
                readLength++;
                if (readLength == frameR60abh1.dataLength + 1) {
                    memcpy(frameR60abh1.data, frameBuffer, frameR60abh1.dataLength + 1);
                    frameR60abh1.check = frameBuffer[frameR60abh1.dataLength];
                    recvStatus = READTAIL;
                    readLength = 0;
                }
            } else if (recvStatus == READTAIL) {
                frameR60abh1.tail[readLength] = readByte;
                readLength++;
                if (readLength == 2) {
                    if (frameR60abh1.tail[0] == 0x54 && frameR60abh1.tail[1] == 0x43) {
                        /* emit signal */
//                        qDebug() << "signal";
                        emit r60abh1ReceiveFrame(frameR60abh1);
                    }
                    /* init */
                    recvStatus = READHEAD;
                    readLength = 0;
                }
            }
            break;
        }
    }
    qDebug() << "thread end";
    /* init */
    readLength = 0;
    recvStatus = READHEAD;
}
