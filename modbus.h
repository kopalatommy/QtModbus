#ifndef MODBUS_H
#define MODBUS_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

enum FunctCodes{
    READ_COILS = 0x01,
    READ_DISCRETE_INPUTS = 0x02,
    READ_MULTIPLE_REGISTERS = 0x03,
    READ_INPUT_REGISTERS = 0x04,
    WRITE_SINGLE_COIL = 0x05,
    WRITE_SINGLE_REGISTER = 0x06,
    WRITE_MULTIPLE_COILS = 0x0F,
    WRITE_MULTIPLE_REGISTERS = 0x10
};

enum ExceptionCodes{
    UNSUPPORTED_FUNCTION = 0x01,
    INVALID_INDEX = 0x02,
    INCORRECT_DATA = 0x03,
    UNRECOVERABLE = 0x04
};

class Modbus : public QObject
{
    Q_OBJECT
public:
    Modbus();

private slots:
    void OnReceiveNewConnection();

    void OnSlaveDisconnected();
    void OnSlaveReadyRead();

protected:
    union byteArray{
        ushort value;
        char bytes[2];
    };

    QTcpServer tcpServer;

    QString HexToDec(QByteArray arr);

    void ParseMessage(QByteArray data);
};

#endif // MODBUS_H
