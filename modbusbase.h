#ifndef MODBUSBASE_H
#define MODBUSBASE_H

#include <QObject>

#include "modbusdatatable.h"

//Function codes
#define READ_COILS              0x01
#define READ_INPUTS             0x02
#define READ_HOLDING_REGISTERS  0x03
#define READ_INPUT_REGISTERS    0x04
#define SET_SINGLE_COIL         0x05
#define SET_SINGLE_REGISTER     0x06
#define SET_MULTIPLE_COILS      0x0F
#define SET_MULTIPLE_REGISTERS  0x10

enum ModbusState{
    NotConnected,
    Waiting,
    Connected
};

struct ModbusResponse{
public:
    ModbusResponse()
    {

    }

    ModbusResponse(QByteArray message)
    {
        //Store the message
        received = message;

        //Check if received header
        if(message.length() < 6)
        {
            received.append(message);
        }
        else
        {
            //Grab header bytes
            for(int i = 0; i < 6; i++)
                messageHeader.append(message[i]);
        }

        //Get data bytes
        for(int i = 6; i < message.length(); i++)
        {
            messageData.append(message[i]);
        }
    }

    ModbusResponse(short messageID)
    {
        byteArray temp;
        temp.word = messageID;
        //Append message ID
        messageHeader.append(temp.bytes[1]);
        messageHeader.append(temp.bytes[0]);

        temp.word = 0;
        //Append protocol
        messageHeader.append(temp.bytes[1]);
        messageHeader.append(temp.bytes[0]);
    }

    void ReadByte(char byte)
    {
        received.append(byte);

        //Distribute byte
        if(received.length() <= 6)
            messageHeader.append(byte);
        else
            messageData.append(byte);
    }

    bool ReceivedFull()
    {
        return received.length() > 6 && (received.length() - 6) == GetMessageLength();
    }

    QByteArray GetHeader()
    {
        return messageHeader;
    }

    QByteArray GetData()
    {
        return messageData;
    }

    short GetTransactionID()
    {
        if(messageHeader.length() > 1)
        {
            byteArray temp;
            temp.bytes[1] = received[0];
            temp.bytes[0] = received[1];
            return temp.word;
        }
        else
        {
            qDebug() << "Error: Have not received transaction id bytes";
            return 0;
        }
    }

    short GetProtocol()
    {
        if(messageHeader.length() > 3)
        {
            byteArray temp;
            temp.bytes[1] = received[2];
            temp.bytes[0] = received[3];
            return temp.word;
        }
        else
        {
            qDebug() << "Error: Have not received protocol bytes";
            return 0;
        }
    }

    short GetMessageLength()
    {
        if(messageHeader.length() > 1)
        {
            byteArray temp;
            temp.bytes[1] = received[4];
            temp.bytes[0] = received[5];
            return temp.word;
        }
        else
        {
            qDebug() << "Error: Have not received message length bytes";
            return 0;
        }
    }

    char GetUnitID()
    {
        if(messageData.length() > 0)
            return messageData.at(0);
        else
        {
            qDebug() << "Error: Has not received Unit ID byte";
            return 0;
        }
    }

    char GetFunctionCode()
    {
        if(messageData.length() > 1)
            return messageData.at(1);
        else
        {
            qDebug() << "Error: Has not received function code byte";
            return 0;
        }

    }

    char GetDataBytes()
    {
        if(messageData.length() > 2)
            return messageData[2];
        else
        {
            qDebug() << "Error: Have not received numDataBytes byte";
            return 0;
        }
    }

    short GetStartAddress()
    {
        if(messageData.length() > 3)
        {
            byteArray temp;
            temp.bytes[1] = messageData[2];
            temp.bytes[0] = messageData[3];
            return temp.word;
        }
        else
        {
            qDebug() << "Error: Have not received start address bytes";
            return 0;
        }
    }

    short GetQuantity()
    {
        if(messageData.length() > 5)
        {
            byteArray temp;
            temp.bytes[1] = messageData[4];
            temp.bytes[0] = messageData[5];
            return temp.word;
        }
        else
        {
            qDebug() << "Error: Have not received quantity bytes";
            return 0;
        }
    }

    QByteArray GetMessage()
    {
        return received;
    }

    void Clear()
    {
        received.clear();
        messageHeader.clear();
        messageData.clear();
    }

private:
    QByteArray received;

    QByteArray messageHeader;
    QByteArray messageData;
};

class ModbusBase : public QObject
{
    Q_OBJECT

public:
    ModbusBase();
    ModbusBase(ModbusDatatable * datatable);

    bool IsConnected();
    ModbusState GetState();
    QString GetErrorString();

signals:
    void NewState(ModbusState newState);

protected:
    bool isConnected = false;
    ModbusState state = ModbusState::NotConnected;
    QString errorString = "No Error";

    ModbusDatatable * dataTable = Q_NULLPTR;
};

#endif // MODBUSBASE_H
