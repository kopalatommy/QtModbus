#include "modbusslave.h"

ModbusSlave::ModbusSlave(ModbusDatatable * dataTable, char address) : ModbusBase()
{
    this->dataTable = dataTable;
    this->address = address;
}

void ModbusSlave::Start(ushort port)
{
    if(!tcpServer.listen(QHostAddress::Any, port))
    {
        qDebug() << "Failed to start modbus slave tcp server. " << tcpServer.errorString();
    }
    connect(&tcpServer, &QTcpServer::newConnection, this, &ModbusSlave::NewConnection);
}

void ModbusSlave::Start(ushort port, char address)
{
    if(!tcpServer.listen(QHostAddress::Any, port))
    {
        qDebug() << "Failed to start modbus slave tcp server. " << tcpServer.errorString();
    }
    connect(&tcpServer, &QTcpServer::newConnection, this, &ModbusSlave::NewConnection);

    this->address = address;
}

void ModbusSlave::NewConnection()
{
    QTcpSocket * newSocket = tcpServer.nextPendingConnection();

    if(newSocket != Q_NULLPTR)
    {
        qDebug() << "Connected to " << newSocket->peerAddress();

        connect(newSocket, &QTcpSocket::readyRead, this, &ModbusSlave::ReadyRead);
    }
    else
    {
        qDebug() << "Modbus slave received null connection";
    }
}

void ModbusSlave::ReadyRead()
{
    QTcpSocket * socket = reinterpret_cast<QTcpSocket*>(sender());

    ParseData(socket->readAll(), socket);
}

void ModbusSlave::ParseData(QByteArray message, QTcpSocket * sender)
{
    qDebug() << "Received: " << Helpers::HexToDec(message);

    /*byteArray messageID;
    messageID.bytes[1] = message.at(0);
    messageID.bytes[0] = message.at(1);*/

    //Check address
    if(address == message.at(6))
    {
        //Check function code
        switch (message.at(7)) {
        case READ_COILS:
            qDebug() << "Wrote: " << sender->write(ReadCoils(message));
            break;

        case READ_INPUTS:
            sender->write(ReadDiscreteInputs(message));
            break;

        case READ_HOLDING_REGISTERS:
            sender->write(ReadHoldingRegisters(message));
            break;

        case READ_INPUT_REGISTERS:
            sender->write(ReadInputRegisters(message));
            break;

        case SET_SINGLE_COIL:
            sender->write(WriteSingleCoil(message));
            break;

        case SET_SINGLE_REGISTER:
            sender->write(WriteSingleRegister(message));
            break;

        case SET_MULTIPLE_COILS:
            qDebug() << "Wrote: " << sender->write(WriteMultipleCoils(message));
            break;

        case SET_MULTIPLE_REGISTERS:
            sender->write(WriteMultipleRegisters(message));
            break;

        default:
            qDebug() << "Bad funct code: " << QString::number(message.at(1));
        }
    }
    else
    {
        qDebug() << "Received bad address: " << QString::number(message.at(0)) << " Address: " << QString::number(address);
    }
}

QByteArray ModbusSlave::ReadCoils(QByteArray message)
{
    QByteArray response;

    byteArray messageID;
    messageID.bytes[1] = message[0];
    messageID.bytes[0] = message[1];

    byteArray protocol;
    protocol.bytes[1] = message[2];
    protocol.bytes[0] = message[3];

    byteArray messageLength;
    messageLength.bytes[1] = message[4];
    messageLength.bytes[0] = message[5];

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        char slaveAddress = message[6];
        char functCode = message[7];

        byteArray start;
        start.bytes[1] = message[8];
        start.bytes[0] = message[9];

        byteArray quantity;
        quantity.bytes[1] = message[10];
        quantity.bytes[0] = message[11];

        short numBytes = static_cast<short>(qCeil(static_cast<double>(quantity.word) / 8));

        byteArray length;
        length.word = numBytes + 3 /* Unit ID, funct code, num bytes, bytes */;

        //Set header
        //Transaction ID
        response.append(messageID.bytes[1]);
        response.append(messageID.bytes[0]);
        //Protocol
        response.append(protocol.bytes[1]);
        response.append(protocol.bytes[0]);
        //Message Length
        //Have to account for Unit ID and transaction ID
        response.append(length.bytes[1]);
        response.append(length.bytes[0]);

        //Append data
        response.append(slaveAddress);
        response.append(functCode);

        length.word = numBytes;
        //Num bytes
        response.append(length.bytes[0]);

        QList<char> values = ModbusDatatable::ConvertList(dataTable->GetCoils(address, quantity.word));
        //The actual bytes
        for(int i = 0; i < numBytes; i++)
        {
            response.append(values[i]);
        }
    }

    return response;
}

QByteArray ModbusSlave::ReadDiscreteInputs(QByteArray message)
{
    QByteArray response;

    byteArray messageID;
    messageID.bytes[1] = message[0];
    messageID.bytes[0] = message[1];

    byteArray protocol;
    protocol.bytes[1] = message[2];
    protocol.bytes[0] = message[3];

    byteArray messageLength;
    messageLength.bytes[1] = message[4];
    messageLength.bytes[0] = message[5];

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        char slaveAddress = message[6];
        char functCode = message[7];

        byteArray start;
        start.bytes[1] = message[8];
        start.bytes[0] = message[9];

        byteArray quantity;
        quantity.bytes[1] = message[10];
        quantity.bytes[0] = message[11];

        short numBytes = static_cast<short>(qCeil(static_cast<double>(quantity.word) / 8));

        byteArray length;
        length.word = numBytes + 3 /* Unit ID, funct code, num bytes, bytes */;

        //Set header
        //Transaction ID
        response.append(messageID.bytes[1]);
        response.append(messageID.bytes[0]);
        //Protocol
        response.append(protocol.bytes[1]);
        response.append(protocol.bytes[0]);
        //Message Length
        //Have to account for Unit ID and transaction ID
        response.append(length.bytes[1]);
        response.append(length.bytes[0]);

        //Append data
        response.append(slaveAddress);
        response.append(functCode);

        length.word = numBytes;
        //Num bytes
        response.append(length.bytes[0]);

        QList<char> bytes = ModbusDatatable::ConvertList(dataTable->GetDiscreteInputs(address, quantity.word));
        //The actual bytes
        for(int i = 0; i < numBytes; i++)
        {
            response.append(bytes[i]);
        }
    }

    return response;
}

QByteArray ModbusSlave::ReadHoldingRegisters(QByteArray message)
{
    QByteArray response;

    byteArray messageID;
    messageID.bytes[1] = message[0];
    messageID.bytes[0] = message[1];

    byteArray protocol;
    protocol.bytes[1] = message[2];
    protocol.bytes[0] = message[3];

    byteArray messageLength;
    messageLength.bytes[1] = message[4];
    messageLength.bytes[0] = message[5];

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        char slaveAddress = message[6];
        char functCode = message[7];

        byteArray start;
        start.bytes[1] = message[8];
        start.bytes[0] = message[9];

        byteArray quantity;
        quantity.bytes[1] = message[10];
        quantity.bytes[0] = message[11];

        short numBytes = quantity.word * 2;

        byteArray length;
        length.word = numBytes + 3 /* Unit ID, funct code, num bytes, bytes */;

        //Set header
        //Transaction ID
        response.append(messageID.bytes[1]);
        response.append(messageID.bytes[0]);
        //Protocol
        response.append(protocol.bytes[1]);
        response.append(protocol.bytes[0]);
        //Message Length
        //Have to account for Unit ID and transaction ID
        response.append(length.bytes[1]);
        response.append(length.bytes[0]);

        //Append data
        response.append(slaveAddress);
        response.append(functCode);

        length.word = numBytes;
        //Num bytes
        response.append(length.bytes[0]);
        //The actual bytes
        QList<short> regs = dataTable->GetInputRegisters(address, quantity.word);
        byteArray reg;
        for(short i = 0; i < numBytes; i++)
        {
            reg.word = regs[i];
            response.append(reg.bytes[1]);
            response.append(reg.bytes[0]);
        }
    }

    return response;
}

QByteArray ModbusSlave::ReadInputRegisters(QByteArray message)
{
    QByteArray response;

    byteArray messageID;
    messageID.bytes[1] = message[0];
    messageID.bytes[0] = message[1];

    byteArray protocol;
    protocol.bytes[1] = message[2];
    protocol.bytes[0] = message[3];

    byteArray messageLength;
    messageLength.bytes[1] = message[4];
    messageLength.bytes[0] = message[5];

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        char slaveAddress = message[6];
        char functCode = message[7];

        byteArray start;
        start.bytes[1] = message[8];
        start.bytes[0] = message[9];

        byteArray quantity;
        quantity.bytes[1] = message[10];
        quantity.bytes[0] = message[11];

        short numBytes = quantity.word * 2;

        byteArray length;
        length.word = numBytes + 3 /* Unit ID, funct code, num bytes, bytes */;

        //Set header
        //Transaction ID
        response.append(messageID.bytes[1]);
        response.append(messageID.bytes[0]);
        //Protocol
        response.append(protocol.bytes[1]);
        response.append(protocol.bytes[0]);
        //Message Length
        //Have to account for Unit ID and transaction ID
        response.append(length.bytes[1]);
        response.append(length.bytes[0]);

        //Append data
        response.append(slaveAddress);
        response.append(functCode);

        length.word = numBytes;
        //Num bytes
        response.append(length.bytes[0]);
        //The actual bytes
        QList<short> regs = dataTable->GetHoldingRegisters(address, quantity.word);
        byteArray reg;
        for(short i = 0; i < numBytes; i++)
        {
            reg.word = regs[i];
            response.append(reg.bytes[1]);
            response.append(reg.bytes[0]);
        }
    }

    return response;
}

QByteArray ModbusSlave::WriteSingleCoil(QByteArray message)
{
    byteArray messageID;
    messageID.bytes[1] = message[0];
    messageID.bytes[0] = message[1];

    byteArray protocol;
    protocol.bytes[1] = message[2];
    protocol.bytes[0] = message[3];

    byteArray messageLength;
    messageLength.bytes[1] = message[4];
    messageLength.bytes[0] = message[5];

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        QByteArray response;

        char slaveAddress = message[6];
        char functCode = message[7];

        byteArray address;
        address.bytes[1] = message[8];
        address.bytes[0] = message[9];

        byteArray value;
        value.bytes[1] = message[10];
        value.bytes[0] = message[11];

        //Build response
        //Transaction ID
        response.append(messageID.bytes[1]);
        response.append(messageID.bytes[0]);
        //Protocol
        response.append(protocol.bytes[1]);
        response.append(protocol.bytes[0]);
        //Message length
        response.append(messageLength.bytes[1]);
        response.append(messageLength.bytes[0]);
        //Device address
        response.append(slaveAddress);
        //Function code
        response.append(functCode);
        //Address
        response.append(address.bytes[1]);
        response.append(address.bytes[0]);
        //Value
        response.append(value.bytes[1]);
        response.append(value.bytes[0]);

        dataTable->SetCoil(address.word, value.word == 0xFF);

        return response;
    }

    return QByteArray();
}

QByteArray ModbusSlave::WriteSingleRegister(QByteArray message)
{
    byteArray messageID;
    messageID.bytes[1] = message[0];
    messageID.bytes[0] = message[1];

    byteArray protocol;
    protocol.bytes[1] = message[2];
    protocol.bytes[0] = message[3];

    byteArray messageLength;
    messageLength.bytes[1] = message[4];
    messageLength.bytes[0] = message[5];

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        QByteArray response;

        char slaveAddress = message[6];
        char functCode = message[7];

        byteArray address;
        address.bytes[1] = message[8];
        address.bytes[0] = message[9];

        byteArray value;
        value.bytes[1] = message[10];
        value.bytes[0] = message[11];

        //Build response
        //Transaction ID
        response.append(messageID.bytes[1]);
        response.append(messageID.bytes[0]);
        //Protocol
        response.append(protocol.bytes[1]);
        response.append(protocol.bytes[0]);
        //Message length
        response.append(messageLength.bytes[1]);
        response.append(messageLength.bytes[0]);
        //Device address
        response.append(slaveAddress);
        //Function code
        response.append(functCode);
        //Address
        response.append(address.bytes[1]);
        response.append(address.bytes[0]);
        //Value
        response.append(value.bytes[1]);
        response.append(value.bytes[0]);

        dataTable->SetHoldingRegister(address.word, value.word);

        return response;
    }

    return QByteArray();
}

QByteArray ModbusSlave::WriteMultipleCoils(QByteArray message)
{
    byteArray messageID;
    messageID.bytes[1] = message[0];
    messageID.bytes[0] = message[1];

    byteArray protocol;
    protocol.bytes[1] = message[2];
    protocol.bytes[0] = message[3];

    byteArray messageLength;
    messageLength.bytes[1] = message[4];
    messageLength.bytes[0] = message[5];

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        QByteArray response;

        char slaveAddress = message[6];
        char functCode = message[7];

        byteArray address;
        address.bytes[1] = message[8];
        address.bytes[0] = message[9];

        byteArray length;
        length.bytes[1] = message[10];
        length.bytes[0] = message[11];

        char bytes = message.at(12);

        //Get the byte values
        QList<char> values;
        for(int i = 0; i < bytes; i++)
        {
            values.append(message.at(12 + i));
        }
        dataTable->SetCoils(address.word, ModbusDatatable::ConvertList(values));

        //Build response
        //Transaction ID
        response.append(messageID.bytes[1]);
        response.append(messageID.bytes[0]);
        //Protocol
        response.append(protocol.bytes[1]);
        response.append(protocol.bytes[0]);
        //Message length
        response.append(messageLength.bytes[1]);
        response.append(messageLength.bytes[0]);
        //Device address
        response.append(slaveAddress);
        //Function code
        response.append(functCode);
        //Address
        response.append(address.bytes[1]);
        response.append(address.bytes[0]);
        //Value
        response.append(length.bytes[1]);
        response.append(length.bytes[0]);

        return response;
    }

    return QByteArray();
}

QByteArray ModbusSlave::WriteMultipleRegisters(QByteArray message)
{
    byteArray messageID;
    messageID.bytes[1] = message[0];
    messageID.bytes[0] = message[1];

    byteArray protocol;
    protocol.bytes[1] = message[2];
    protocol.bytes[0] = message[3];

    byteArray messageLength;
    messageLength.bytes[1] = message[4];
    messageLength.bytes[0] = message[5];

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        QByteArray response;

        char slaveAddress = message[6];
        char functCode = message[7];

        byteArray address;
        address.bytes[1] = message[8];
        address.bytes[0] = message[9];

        byteArray length;
        length.bytes[1] = message[10];
        length.bytes[0] = message[11];

        char bytes = message.at(12);

        //Get the byte values
        QList<short> values;
        byteArray reg;
        for(int i = 0; i < bytes; i++)
        {
            reg.bytes[1] = message.at(12 + (i * 2 + 1));
            reg.bytes[0] = message.at(12 + (i * 2));
            values.append(reg.word);
        }
        dataTable->SetHoldingRegisters(address.word, values);

        //Build response
        //Transaction ID
        response.append(messageID.bytes[1]);
        response.append(messageID.bytes[0]);
        //Protocol
        response.append(protocol.bytes[1]);
        response.append(protocol.bytes[0]);
        //Message length
        response.append(messageLength.bytes[1]);
        response.append(messageLength.bytes[0]);
        //Device address
        response.append(slaveAddress);
        //Function code
        response.append(functCode);
        //Address
        response.append(address.bytes[1]);
        response.append(address.bytes[0]);
        //Value
        response.append(length.bytes[1]);
        response.append(length.bytes[0]);

        return response;
    }

    return QByteArray();
}
