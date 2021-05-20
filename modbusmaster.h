#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QTime>

#include "modbusbase.h"
#include "helpers.h"

/*
    // for synchronous access
    virtual bool waitForConnected(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000) override;
    bool waitForBytesWritten(int msecs = 30000) override;
    virtual bool waitForDisconnected(int msecs = 30000);
 */

class ModbusMaster : public ModbusBase
{
public:
    ModbusMaster();

    //Returns true if the master connects to the slave
    bool Start(QString slaveIP, ushort port, char address, ModbusDatatable * datatable);

    bool Close();

    void ParseMessage(ModbusResponse message);

    void ReadCoils(short startAddress, short length);
    void ReadDiscreteInputs(short address, short length);
    void ReadHoldingRegisters(short address, short length);
    void ReadInputRegisters(short address, short length);
    void WriteSingleCoil(short address);
    void WriteSingleRegister(short address);
    void WriteMultipleCoils(short address, short length);
    void WriteMultipleRegisters(short address, short length);

private slots:
    void OnConnected();
    void OnDisconnected();
    void OnSocketStateChanged(QAbstractSocket::SocketState newState);
    void OnSocketErrorOccured(QAbstractSocket::SocketError error);

    void ReadyRead();

private:
    QTcpSocket socket;
    char slaveAddress = 0;

    byteArray transactionID;

    ModbusResponse current;

    QMap<int /* Message ID */, QPair<QTime /* Sent time */, ModbusResponse /* Message */>> messages;

    int responseID;
    int valueStart;
    int valueCount;

    void HandleReadCoilsResponse(ModbusResponse response);
    void HandleReadDiscreteInputsResponse(ModbusResponse response);
    void HandleReadHoldingRegistersResponse(ModbusResponse response);
    void HandleReadInputRegistersResponse(ModbusResponse response);
    void HandleWriteSingleCoilResponse(ModbusResponse response);
    void HandleWriteSingleRegisterResponse(ModbusResponse response);
    void HandleWriteMultipleCoilsResponse(ModbusResponse response);
    void HandleWriteMultipleRegistersResponse(ModbusResponse response);
};

#endif // MODBUSMASTER_H
