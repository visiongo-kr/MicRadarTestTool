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

uint16_t SerialPortReadThread::calculateCrc16(uint8_t *lpuc_Frame, uint16_t lus_Len)
{
    const unsigned char cuc_CRCHi[256] = {
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40
    };
    const unsigned char cuc_CRCLo[256] = {
        0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
        0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
        0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
        0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
        0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
        0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
        0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
        0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
        0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
        0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
        0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
        0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
        0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
        0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
        0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
        0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
        0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
        0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
        0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
        0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
        0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
        0x41, 0x81, 0x80, 0x40
    };

    uint8_t luc_CRCHi = 0xFF;
    uint8_t luc_CRCLo = 0xFF;
    int li_Index=0;

    while(lus_Len--)
    {
        li_Index = luc_CRCLo ^ *( lpuc_Frame++);
        luc_CRCLo = (uint8_t)( luc_CRCHi ^ cuc_CRCHi[li_Index]);
        luc_CRCHi = cuc_CRCLo[li_Index];
    }

    return (unsigned short int )(luc_CRCLo<< 8 | luc_CRCHi);
}

void SerialPortReadThread::run()
{
    static int readLength = 0;
    static uint8_t frameBuffer[4096];
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
            uint8_t readByte = readBuffer[readIndex];
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
            /* For R24AFD1 */
            case 1:
                /* receive head */
                if (recvStatus == READHEAD) {
                    if (readByte == 0x55) {
                        frameR2afd1.head = 0x55;
                        frameBuffer[0] = readByte;
                        readLength++;
                        recvStatus = READHEADDATA;
                        qDebug() << "recv head";
                    }
                } else if (recvStatus == READHEADDATA) {
                    if (readLength == 1) {
                        qDebug("%u", readByte);
                        frameBuffer[readLength] = readByte;
                        readLength++;
                    } else {
                        frameR2afd1.length = readByte;
                        frameR2afd1.length = frameR2afd1.length << 8;
                        frameR2afd1.length = frameR2afd1.length | frameBuffer[readLength - 1];
                        frameBuffer[readLength] = readByte;
                        readLength++;
                        recvStatus = READDATA;
                        qDebug() << "recv length" << frameR2afd1.length;
                    }
                } else if (recvStatus == READDATA) {
                    frameBuffer[readLength] = readByte;
                    readLength++;
                    if (readLength == 3 + frameR2afd1.length) {
                        frameR2afd1.command = frameBuffer[3];
                        frameR2afd1.address1 = frameBuffer[4];
                        frameR2afd1.address2 = frameBuffer[5];
                        memcpy(frameR2afd1.data, &frameBuffer[6], frameR2afd1.length - 7);
                        qDebug() << calculateCrc16(&frameBuffer[1], frameR2afd1.length) << "--" << frameR2afd1.crc;
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
