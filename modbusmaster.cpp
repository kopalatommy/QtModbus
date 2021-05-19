#include "modbusmaster.h"

ModbusMaster::ModbusMaster()
{
    connect(&socket, &QTcpSocket::connected, this, &ModbusMaster::OnConnected);
    connect(&socket, &QTcpSocket::disconnected, this, &ModbusMaster::OnDisconnected);
    connect(&socket, &QTcpSocket::stateChanged, this, &ModbusMaster::OnSocketStateChanged);
    connect(&socket, &QTcpSocket::errorOccurred, this, &ModbusMaster::OnSocketErrorOccured);
    connect(&socket, &QTcpSocket::readyRead, this, &ModbusMaster::ReadyRead);

    transactionID.word = 0;
}

bool ModbusMaster::Start(QString slaveIP, ushort port, char address, ModbusDatatable * datatable)
{
    //If already connected, then cannot connect to slave until disconnecting
    if(IsConnected())
    {
        qDebug() << "Trying to connect to slave when already connected. Connected to " << socket.peerName();
        return false;
    }

    //Connect to slave, slave will have a tcp server running
    socket.connectToHost(slaveIP, port, QIODevice::ReadWrite);

    //Wait for the socket to connect
    if(!socket.waitForConnected(30000))
    {
        return false;
    }

    //Store the slave address
    slaveAddress = address;

    this->dataTable = datatable;

    return true;
}

bool ModbusMaster::Close()
{
    //Cannot disconnect if not already connected
    if(!IsConnected())
    {
        return false;
    }

    if(socket.isOpen())
        socket.disconnectFromHost();

    return socket.waitForDisconnected(30000);
}

void ModbusMaster::OnConnected()
{
    qDebug() << "Master has successfully connected to slave";
}

void ModbusMaster::OnDisconnected()
{
    qDebug() << "Master has disconnected from slave";
}

void ModbusMaster::OnSocketStateChanged(QAbstractSocket::SocketState newState)
{
    switch (newState)
    {
        case QAbstractSocket::SocketState::UnconnectedState:
        qDebug() << "Socket has disconnected";
        break;

    case QAbstractSocket::SocketState::HostLookupState:
        qDebug() << "Socket is looking up the host";
        break;

    case QAbstractSocket::SocketState::ConnectingState:
        qDebug() << "Socket is attempting to connect";
        break;

    case QAbstractSocket::SocketState::ConnectedState:
        qDebug() << "Socket has successfully connected to slave";
        break;

    case QAbstractSocket::SocketState::BoundState:
        qDebug() << "Scoket is now bounded";
        break;

    case QAbstractSocket::SocketState::ListeningState:
        qDebug() << "Socket is listening";
        break;

    case QAbstractSocket::SocketState::ClosingState:
        qDebug() << "Socket is closing";
        break;
    }
}

void ModbusMaster::OnSocketErrorOccured(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket has encountered an error: " << error;

    if(!socket.isOpen())
    {
        errorString = "Socket has closed due to error: " + socket.errorString();
        qDebug() << errorString;
        state = ModbusState::NotConnected;
        emit NewState(state);
    }
}

void ModbusMaster::ReadyRead()
{
    ParseMessage(socket.readAll());
}

void ModbusMaster::ParseMessage(QByteArray message)
{
    qDebug() << "Received: " << Helpers::HexToDec(message);

    //Check address
    if(slaveAddress == message.at(6))
    {
        //Check function code
        switch (message.at(7))
        {
        case READ_COILS:
            HandleReadCoilsResponse(message);
            break;

        case READ_INPUTS:
            HandleReadDiscreteInputsResponse(message);
            break;

        case READ_HOLDING_REGISTERS:
            HandleReadHoldingRegistersResponse(message);
            break;

        case READ_INPUT_REGISTERS:
            HandleReadInputRegistersResponse(message);
            break;

        case SET_SINGLE_COIL:
            HandleWriteSingleCoilResponse(message);
            break;

        case SET_SINGLE_REGISTER:
            HandleWriteSingleRegisterResponse(message);
            break;

        case SET_MULTIPLE_COILS:
            HandleWriteMultipleCoilsResponse(message);
            break;

        case SET_MULTIPLE_REGISTERS:
            HandleWriteMultipleRegistersResponse(message);
            break;

        default:
            qDebug() << "Bad funct code: " << QString::number(message.at(7));
        }
    }
    else
    {
        qDebug() << "Received bad address: " << QString::number(message.at(0)) << " Address: " << QString::number(slaveAddress);
    }
}

void ModbusMaster::ReadCoils(short startAddress, short length)
{
    QByteArray message;

    valueStart = startAddress;
    valueCount = length;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);

    //Append protocol, wil always be 00 00 for Modbud TCP/IP
    message.append('\0');
    message.append('\0');

    //Append the length of the remaining bytes, should be 6
    message.append('\0');
    message.append('\6');

    //Append slave Unit ID
    message.append(slaveAddress);

    //Append the function code
    message.append(READ_COILS);

    //Append the starting address
    byteArray start;
    start.word = startAddress;
    message.append(start.bytes[1]);
    message.append(start.bytes[0]);

    //Append the length bytes
    byteArray len;
    len.word = length;
    message.append(len.bytes[1]);
    message.append(len.bytes[0]);

    qDebug() << "Writing: " << Helpers::HexToDec(message);

    messages[transactionID.word] = QPair<QTime, QByteArray>(QTime::currentTime(), message);

    socket.write(message);

    //Increment the id
    transactionID.word += 1;
}

void ModbusMaster::ReadDiscreteInputs(short address, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);

    //Append protocol, wil always be 00 00 for Modbud TCP/IP
    message.append('\0');
    message.append('\0');

    //Append the length of the remaining bytes, should be 6
    message.append('\0');
    message.append('\6');

    //Append slave Unit ID
    message.append(slaveAddress);

    //Append the function code
    message.append(READ_INPUTS);

    //Append the starting address
    byteArray start;
    start.word = address;
    message.append(start.bytes[1]);
    message.append(start.bytes[0]);

    //Append the length bytes
    byteArray len;
    len.word = length;
    message.append(len.bytes[1]);
    message.append(len.bytes[0]);

    qDebug() << "Writing: " << Helpers::HexToDec(message);

    socket.write(message);
    messages[transactionID.word] = QPair<QTime, QByteArray>(QTime::currentTime(), message);

    //Increment the id
    transactionID.word += 1;
}

//ToDo, verify this is correct for the code
void ModbusMaster::ReadHoldingRegisters(short address, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);

    //Append protocol, wil always be 00 00 for Modbud TCP/IP
    message.append('\0');
    message.append('\0');

    //Append the length of the remaining bytes, should be 6
    message.append('\0');
    message.append('\6');

    //Append slave Unit ID
    message.append(slaveAddress);

    //Append the function code
    message.append(READ_HOLDING_REGISTERS);

    //Append the starting address
    byteArray start;
    start.word = address;
    message.append(start.bytes[1]);
    message.append(start.bytes[0]);

    //Append the length bytes
    byteArray len;
    len.word = length;
    message.append(len.bytes[1]);
    message.append(len.bytes[0]);

    qDebug() << "Writing: " << Helpers::HexToDec(message);

    socket.write(message);
    messages[transactionID.word] = QPair<QTime, QByteArray>(QTime::currentTime(), message);

    //Increment the id
    transactionID.word += 1;
}

void ModbusMaster::ReadInputRegisters(short address, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);

    //Append protocol, wil always be 00 00 for Modbud TCP/IP
    message.append('\0');
    message.append('\0');

    //Append the length of the remaining bytes, should be 6
    message.append('\0');
    message.append('\6');

    //Append slave Unit ID
    message.append(slaveAddress);

    //Append the function code
    message.append(READ_INPUT_REGISTERS);

    //Append the starting address
    byteArray start;
    start.word = address;
    message.append(start.bytes[1]);
    message.append(start.bytes[0]);

    //Append the length bytes
    byteArray len;
    len.word = length;
    message.append(len.bytes[1]);
    message.append(len.bytes[0]);

    qDebug() << "Writing: " << Helpers::HexToDec(message);

    socket.write(message);

    messages[transactionID.word] = QPair<QTime, QByteArray>(QTime::currentTime(), message);

    //Increment the id
    transactionID.word += 1;
}

void ModbusMaster::WriteSingleCoil(short address)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);

    //Append protocol, wil always be 00 00 for Modbud TCP/IP
    message.append('\0');
    message.append('\0');

    //Append the length of the remaining bytes, should be 6
    message.append('\0');
    message.append('\6');

    //Append slave Unit ID
    message.append(slaveAddress);

    //Append the function code
    message.append(SET_SINGLE_COIL);

    //Append the starting address
    byteArray start;
    start.word = address;
    message.append(start.bytes[1]);
    message.append(start.bytes[0]);

    //Append the value of the coil
    //ToDo, integrate with table
    message.append(CHAR_MAX);
    message.append('\0');

    qDebug() << "Writing: " << Helpers::HexToDec(message);

    socket.write(message);

    messages[transactionID.word] = QPair<QTime, QByteArray>(QTime::currentTime(), message);

    //Increment the id
    transactionID.word += 1;
}

void ModbusMaster::WriteSingleRegister(short address)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);

    //Append protocol, wil always be 00 00 for Modbud TCP/IP
    message.append('\0');
    message.append('\0');

    //Append the length of the remaining bytes, should be 6
    message.append('\0');
    message.append('\6');

    //Append slave Unit ID
    message.append(slaveAddress);

    //Append the function code
    message.append(SET_SINGLE_REGISTER);

    //Append the starting address
    byteArray start;
    start.word = address;
    message.append(start.bytes[1]);
    message.append(start.bytes[0]);

    //Append the value of the coil
    //ToDo, integrate with table
    byteArray reg;
    reg.word = 0xFF;
    message.append(reg.bytes[1]);
    message.append(reg.bytes[0]);

    qDebug() << "Writing: " << Helpers::HexToDec(message);

    socket.write(message);

    messages[transactionID.word] = QPair<QTime, QByteArray>(QTime::currentTime(), message);

    //Increment the id
    transactionID.word += 1;
}

void ModbusMaster::WriteMultipleCoils(short address, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);

    //Append protocol, wil always be 00 00 for Modbud TCP/IP
    message.append('\0');
    message.append('\0');

    //Append the length of the remaining bytes, should be 6
    message.append('\0');
    message.append('\6');

    //Append slave Unit ID
    message.append(slaveAddress);

    //Append the function code
    message.append(SET_MULTIPLE_COILS);

    //Append the starting address
    byteArray start;
    start.word = address;
    message.append(start.bytes[1]);
    message.append(start.bytes[0]);

    //Append the number of coils
    byteArray len;
    len.word = length;
    message.append(len.bytes[1]);
    message.append(len.bytes[0]);

    //ToDo, integrate table

    qDebug() << "Writing: " << Helpers::HexToDec(message);

    socket.write(message);

    messages[transactionID.word] = QPair<QTime, QByteArray>(QTime::currentTime(), message);

    //Increment the id
    transactionID.word += 1;
}

void ModbusMaster::WriteMultipleRegisters(short address, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);

    //Append protocol, wil always be 00 00 for Modbud TCP/IP
    message.append('\0');
    message.append('\0');

    //Append the length of the remaining bytes, should be 6
    message.append('\0');
    message.append('\6');

    //Append slave Unit ID
    message.append(slaveAddress);

    //Append the function code
    message.append(SET_MULTIPLE_COILS);

    //Append the starting address
    byteArray start;
    start.word = address;
    message.append(start.bytes[1]);
    message.append(start.bytes[0]);

    //Append the number of coils
    byteArray len;
    len.word = length;
    message.append(len.bytes[1]);
    message.append(len.bytes[0]);

    //ToDo, integrate table

    qDebug() << "Writing: " << Helpers::HexToDec(message);

    socket.write(message);

    messages[transactionID.word] = QPair<QTime, QByteArray>(QTime::currentTime(), message);

    //Increment the id
    transactionID.word += 1;
}

void ModbusMaster::HandleReadCoilsResponse(QByteArray message)
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

    if(messageID.word != responseID)
    {
        qDebug() << "Error: Received different message";
        return;
    }

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        char slaveAddress = message[6];
        char functCode = message[7];

        char dataBytes = message[8];

        QList<char> values;
        for(int i = 0; i < dataBytes; i++)
        {
            values.append(message[9 + i]);
        }
        //Convert to bool list
        QList<bool> lst = ModbusDatatable::ConvertList(values);
        for(int i = 0; i < valueCount; i++)
        {
            qDebug() << "Setting coil " << i << " to " << lst[i];
            dataTable->SetCoil(valueStart + i, lst[i]);
        }
    }
    else
    {
        qDebug() << "Received bad response to ReadCoils";
        qDebug() << "Received " << (message.length() - 6) << " Expected: " << messageLength.word;
    }
}

void ModbusMaster::HandleReadDiscreteInputsResponse(QByteArray message)
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

    if(messageID.word != responseID)
    {
        qDebug() << "Error: Received different message";
        return;
    }

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        char slaveAddress = message[6];
        char functCode = message[7];

        char dataBytes = message[8];

        QList<char> values;
        for(int i = 0; i < dataBytes; i++)
        {
            values.append(message[9 + i]);
        }
        //Convert to bool list
        QList<bool> lst = ModbusDatatable::ConvertList(values);
        for(int i = 0; i < valueCount; i++)
        {
            dataTable->SetDiscreteInput(valueStart + i, lst[i]);
        }
    }
    else
    {
        qDebug() << "Received bad response to ReadCoils";
        qDebug() << "Received " << (message.length() - 6) << " Expected: " << messageLength.word;
    }
}

void ModbusMaster::HandleReadHoldingRegistersResponse(QByteArray message)
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

    if(messageID.word != responseID)
    {
        qDebug() << "Error: Received different message";
        return;
    }

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        char slaveAddress = message[6];
        char functCode = message[7];

        char dataBytes = message[8];

        QList<char> values;
        for(int i = 0; i < dataBytes; i++)
        {
            values.append(message[9 + i]);
        }
        //Convert to bool list
        QList<bool> lst = ModbusDatatable::ConvertList(values);
        for(int i = 0; i < valueCount; i++)
        {
            dataTable->SetDiscreteInput(valueStart + i, lst[i]);
        }
    }
    else
    {
        qDebug() << "Received bad response to ReadCoils";
        qDebug() << "Received " << (message.length() - 6) << " Expected: " << messageLength.word;
    }
}

void ModbusMaster::HandleReadInputRegistersResponse(QByteArray message)
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

    if(messageID.word != responseID)
    {
        qDebug() << "Error: Received different message";
        return;
    }

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {
        char slaveAddress = message[6];
        char functCode = message[7];

        char dataBytes = message[8];

        QList<short> values;
        byteArray arr;
        for(int i = 0; i < dataBytes / 2; i++)
        {
            arr.bytes[1] = message[9 + (i * 2)];
            arr.bytes[0] = message[10 + (i * 2)];

            values.append(arr.word);
        }
        dataTable->SetInputRegisters(valueStart, values);
    }
    else
    {
        qDebug() << "Received bad response to ReadCoils";
        qDebug() << "Received " << (message.length() - 6) << " Expected: " << messageLength.word;
    }
}

//This does nothing, the message is echoed back by the slave
void ModbusMaster::HandleWriteSingleCoilResponse(QByteArray message)
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

    if(messageID.word != responseID)
    {
        qDebug() << "Error: Received different message";
        return;
    }

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {

    }
}

void ModbusMaster::HandleWriteSingleRegisterResponse(QByteArray message)
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

    if(messageID.word != responseID)
    {
        qDebug() << "Error: Received different message";
        return;
    }

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {

    }
}

void ModbusMaster::HandleWriteMultipleCoilsResponse(QByteArray message)
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

    if(messageID.word != responseID)
    {
        qDebug() << "Error: Received different message";
        return;
    }

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {

    }
}

void ModbusMaster::HandleWriteMultipleRegistersResponse(QByteArray message)
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

    if(messageID.word != responseID)
    {
        qDebug() << "Error: Received different message";
        return;
    }

    //The header bytes are ignored
    if(message.length() - 6 == messageLength.word)
    {

    }
}
