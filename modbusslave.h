#ifndef MODBUSSLAVE_H
#define MODBUSSLAVE_H

#include <QDebug>
#include <QTcpSocket>
#include <QTcpServer>
#include <QString>
#include <QtMath>

#include "helpers.h"

#include "modbusbase.h"

class ModbusSlave : public ModbusBase
{
    Q_OBJECT

public:
    ModbusSlave(ModbusDatatable * datatable, char address);

    void Start(ushort port);
    void Start(ushort port, char address);

    QByteArray ReadCoils(QByteArray message);
    QByteArray ReadDiscreteInputs(QByteArray message);
    QByteArray ReadHoldingRegisters(QByteArray message);
    QByteArray ReadInputRegisters(QByteArray message);
    QByteArray WriteSingleCoil(QByteArray message);
    QByteArray WriteSingleRegister(QByteArray message);
    QByteArray WriteMultipleCoils(QByteArray message);
    QByteArray WriteMultipleRegisters(QByteArray message);

public slots:
    void ParseData(QByteArray data, QTcpSocket * sender);

signals:

private slots:
    void NewConnection();
    void ReadyRead();

private:
    char address = 0;

    QTcpServer tcpServer;
};

#endif // MODBUSSLAVE_H
