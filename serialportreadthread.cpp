#include "serialportreadthread.h"

#include <QDebug>
#include <cstring>

SerialPortReadThread::SerialPortReadThread(QSerialPort *serialPort)
{
    this->serialPort = serialPort;
    recvStatus = READHEAD;
    threadStatus = true;
}

void SerialPortReadThread::setDevice(volatile int value)
{
    device = value;
}

void SerialPortReadThread::setThreadStatus(bool value)
{
    threadStatus = value;
}

void SerialPortReadThread::run()
{
    static int readLength = 0;
    static char frameBuffer[4096];
    QByteArray readBuffer;

    threadStatus = true;
    while (threadStatus) {
        if (!serialPort->isOpen()) {
            if (recvStatus != READHEAD) {
                /* init */
                readLength = 0;
                recvStatus = READHEAD;
            }
            continue;
        }
//        char readByte;
//        if (serialPort->read(&readByte, 1) <= 0) {
//            continue;
//        }
        readBuffer.clear();
        if (!serialPort->waitForReadyRead(1)) {
            continue;
        }
        readBuffer = serialPort->readAll();
        for (int readIndex = 0; readIndex < readBuffer.length(); readIndex++) {
            char readByte = readBuffer[readIndex];
            switch (device) {
            /* For R60ABH1 Device */
            case 0:
                /* receive head */
                if (recvStatus == READHEAD) {
                    if (readLength == 0 && readByte == 0x53) {
                        frameB60abh1.head[readLength] = readByte;
                        readLength++;
                    } else if (readLength == 1 && readByte == 0x59) {
                        frameB60abh1.head[readLength] = readByte;
                        recvStatus = READHEADDATA;
                        readLength = 0;
                    }
                } else if (recvStatus == READHEADDATA) {
                    frameBuffer[readLength] = readByte;
                    readLength++;
                    if (readLength == 4) {
                        frameB60abh1.control = frameBuffer[0];
                        frameB60abh1.command = frameBuffer[1];
                        frameB60abh1.dataLength = frameBuffer[2] * 0xff + frameBuffer[3];
                        recvStatus = READDATA;
                        readLength = 0;
                    }
                } else if (recvStatus == READDATA) {
                    frameBuffer[readLength] = readByte;
                    readLength++;
                    if (readLength == frameB60abh1.dataLength + 1) {
                        memcpy(frameB60abh1.data, frameBuffer, frameB60abh1.dataLength + 1);
                        frameB60abh1.check = frameBuffer[frameB60abh1.dataLength];
                        recvStatus = READTAIL;
                        readLength = 0;
                    }
                } else if (recvStatus == READTAIL) {
                    frameB60abh1.tail[readLength] = readByte;
                    readLength++;
                    if (readLength == 2) {
                        if (frameB60abh1.tail[0] == 0x54 && frameB60abh1.tail[1] == 0x43) {
                            /* emit signal */
                            emit b60abh1ReceiveFrame(frameB60abh1);
                        }
                        /* init */
                        recvStatus = READHEAD;
                        readLength = 0;
                    }
                }
                break;
            /* For R77ABH1 Device */
            case 2:
                /* receive head */
                if (recvStatus == READHEAD) {
                    if (readLength == 0 && readByte == 0x53) {
                        frameR77abh1.head[readLength] = readByte;
                        readLength++;
                    } else if (readLength > 0 &&  readLength < 4) {
                        frameR77abh1.head[readLength] = readByte;
                        readLength++;
                        if (readLength == 4) {
                            if (frameR77abh1.head[0] == 'S' &&
                                    frameR77abh1.head[1] == 'Y' &&
                                    frameR77abh1.head[2] == 'T' &&
                                    frameR77abh1.head[3] == 'C') {
                                readLength = 0;
                                recvStatus = READHEADDATA;
                            }
                        }
                    }
                } else if (recvStatus == READHEADDATA) {
                    frameBuffer[readLength] = readByte;
                    readLength++;
                    if (readLength == 11) {
                        frameR77abh1.length = frameBuffer[0];
                        frameR77abh1.workMode = frameBuffer[1];
                        frameR77abh1.workTime = frameBuffer[2] * 0xff + frameBuffer[3];
                        frameR77abh1.workStatus = frameBuffer[4];
                        frameR77abh1.tlvNum = frameBuffer[5];
                        readLength = 0;
                        recvStatus = READDATA;
                    }
                } else if (recvStatus == READDATA) {
                    frameBuffer[readLength] = readByte;
                    readLength++;
                    if (readLength == (46 * frameR77abh1.tlvNum + 4)) {
                        for (int i = 0; i < frameR77abh1.tlvNum; i++) {
                            frameR77abh1.tlv[i].tlvFlag = frameBuffer[i * 46];
                            frameR77abh1.tlv[i].objDistance = frameBuffer[i * 46 + 1];
                            frameR77abh1.tlv[i].objDirection = frameBuffer[i * 46 + 2];
                            frameR77abh1.tlv[i].status = frameBuffer[i * 46 + 3];
                            frameR77abh1.tlv[i].breatheValue = frameBuffer[i * 46 + 4];
                            frameR77abh1.tlv[i].heartRate = frameBuffer[i * 46 + 5];
                            memcpy(frameR77abh1.tlv[i].heartRateLine, &frameBuffer[i * 46 + 6], 20);
                            memcpy(frameR77abh1.tlv[i].breatheLine, &frameBuffer[i * 46 + 26], 20);
                        }
                        memcpy(&frameR77abh1.crc, &frameBuffer[15 + frameR77abh1.tlvNum * 46], 2);
                        memcpy(frameR77abh1.frameTail, &frameBuffer[frameR77abh1.tlvNum * 46 + 2], 2);
                        if (frameR77abh1.frameTail[0] == 0xee && frameR77abh1.frameTail[1] == 0xee) {
                            /* emit signal */
                            emit r77abh1ReceiveFrame(frameR77abh1);
                        }
                        readLength = 0;
                        recvStatus = READHEAD;
                    }
                }
                break;
            }
        }
    }
    qDebug() << "thread end";
    /* init */
    readLength = 0;
    recvStatus = READHEAD;
}
