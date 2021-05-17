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

bool ModbusMaster::Start(QString slaveIP, ushort port, char address)
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
    /*char cur;
    while(socket.bytesAvailable() > 0)
    {
        socket.read(&cur, 1);

        current.ReadByte(cur);

        if(current.ReceivedFull())
        {
            qDebug() << "Received full message: " << ;
        }
    }*/
    qDebug() << "Received: " << Helpers::HexToDec(socket.readAll());
}

void ModbusMaster::ReadCoils(short startAddress, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);
    //Increment the id
    transactionID.word += 1;

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

    socket.write(message);
}

void ModbusMaster::ReadDiscreteInputs(short address, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);
    //Increment the id
    transactionID.word += 1;

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
}

//ToDo, verify this is correct for the code
void ModbusMaster::ReadHoldingRegisters(short address, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);
    //Increment the id
    transactionID.word += 1;

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
}

void ModbusMaster::ReadInputRegisters(short address, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);
    //Increment the id
    transactionID.word += 1;

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
}

void ModbusMaster::WriteSingleCoil(short address)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);
    //Increment the id
    transactionID.word += 1;

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
}

void ModbusMaster::WriteSingleRegister(short address)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);
    //Increment the id
    transactionID.word += 1;

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
}

void ModbusMaster::WriteMultipleCoils(short address, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);
    //Increment the id
    transactionID.word += 1;

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
}

void ModbusMaster::WriteMultipleRegisters(short address, short length)
{
    QByteArray message;

    //Append transaction ID
    message.append(transactionID.bytes[1]);
    message.append(transactionID.bytes[0]);
    //Increment the id
    transactionID.word += 1;

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
}
