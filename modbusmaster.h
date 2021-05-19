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

struct ModbusResponse{
public:
    ModbusResponse()
    {

    }

    void ReadByte(char byte)
    {
        received.append(byte);

        //If have not parsed MBAP header and have read enough bytes, parse the header
        //2-Transaction ID
        //2-Protocol
        //2-Length
        if(!parsedHeader && received.length() == 6)
        {
            transactionID.bytes[1] = received.at(0);
            transactionID.bytes[0] = received.at(1);

            protocol.bytes[1] = received.at(2);
            protocol.bytes[0] = received.at(3);

            length.bytes[1] = received.at(4);
            length.bytes[0] = received.at(5);

            received.clear();
            parsedHeader = true;
        }
        //Wait until the rest of the message has been received
        else if(received.length() == length.word)
        {
            unitID = received.at(0);
            functCode = received.at(1);

            //Remove non-data bytes
            received.remove(0, 2);

            receivedAll = true;
        }
    }

    bool ReceivedFull()
    {
        return receivedAll;
    }

    QByteArray GetData()
    {
        return received;
    }

    void Clear()
    {
        receivedAll = false;
        parsedHeader = false;

        received.clear();
    }

private:
    QByteArray received;

    //MBAP header
    byteArray transactionID;
    byteArray protocol;
    byteArray length;
    bool parsedHeader = false;

    //PDU data
    char unitID;
    char functCode;
    bool receivedAll = false;
};

class ModbusMaster : public ModbusBase
{
public:
    ModbusMaster();

    //Returns true if the master connects to the slave
    bool Start(QString slaveIP, ushort port, char address, ModbusDatatable * datatable);

    bool Close();

    void ParseMessage(QByteArray message);

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

    QMap<int /* Message ID */, QPair<QTime /* Sent time */, QByteArray /* Message */>> messages;

    int responseID;
    int valueStart;
    int valueCount;

    void HandleReadCoilsResponse(QByteArray response);
    void HandleReadDiscreteInputsResponse(QByteArray response);
    void HandleReadHoldingRegistersResponse(QByteArray response);
    void HandleReadInputRegistersResponse(QByteArray response);
    void HandleWriteSingleCoilResponse(QByteArray response);
    void HandleWriteSingleRegisterResponse(QByteArray response);
    void HandleWriteMultipleCoilsResponse(QByteArray response);
    void HandleWriteMultipleRegistersResponse(QByteArray response);
};

#endif // MODBUSMASTER_H
