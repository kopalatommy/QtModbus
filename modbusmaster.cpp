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
    char byte;
    while(socket.bytesAvailable())
    {
        socket.read(&byte, 1);
        current.ReadByte(byte);

        if(current.ReceivedFull())
        {
            ParseMessage(current);
            current.Clear();
        }
    }
    if(current.GetMessage().length() != 0)
        qDebug() << "Waiting for more bytes";
}

void ModbusMaster::ParseMessage(ModbusResponse message)
{
    qDebug() << "Received: " << Helpers::HexToDec(message.GetMessage());

    //Check address
    if(slaveAddress == message.GetUnitID())
    {
        //Check function code
        switch (message.GetFunctionCode())
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
            qDebug() << "Bad funct code: " << QString::number(message.GetFunctionCode());
            qDebug() << "As bits: " << QString::number(message.GetFunctionCode(), 2);
            qDebug() << "As uint: " << QString::number(0xFF & static_cast<uchar>(message.GetFunctionCode()));
        }
    }
    else
    {
        qDebug() << "Received bad Unit ID: " << QString::number(message.GetUnitID()) << " Address: " << QString::number(slaveAddress);
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

    messages[transactionID.word] = QPair<QTime, ModbusResponse>(QTime::currentTime(), ModbusResponse(message));

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
    messages[transactionID.word] = QPair<QTime, ModbusResponse>(QTime::currentTime(), ModbusResponse(message));

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
    messages[transactionID.word] = QPair<QTime, ModbusResponse>(QTime::currentTime(), ModbusResponse(message));

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

    messages[transactionID.word] = QPair<QTime, ModbusResponse>(QTime::currentTime(), ModbusResponse(message));

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

    start.word = dataTable->GetCoil(address) ? 0xFF : 0;
    message.append(start.bytes[1]);
    message.append(start.bytes[0]);

    socket.write(message);

    messages[transactionID.word] = QPair<QTime, ModbusResponse>(QTime::currentTime(), ModbusResponse(message));

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

    //Append the value of the register
    byteArray reg;
    reg.word = dataTable->GetHoldingRegister(address);
    message.append(reg.bytes[1]);
    message.append(reg.bytes[0]);

    socket.write(message);

    messages[transactionID.word] = QPair<QTime, ModbusResponse>(QTime::currentTime(), ModbusResponse(message));

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

    //Append the length of the remaining bytes
    byteArray len;
    len.word = 7 + (length / 8);
    if(length % 8 != 0)
        len.word++;
    message.append(len.bytes[1]);
    message.append(len.bytes[0]);

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
    len.word = length;
    message.append(len.bytes[1]);
    message.append(len.bytes[0]);

    //Append the bytes
    qDebug() << "Values: " << dataTable->GetCoils(address, length);
    QList<char> bytes = ModbusDatatable::ConvertList(dataTable->GetCoils(address, length));

    message.append(static_cast<char>(bytes.length()));

    /*for(int i = bytes.length() - 1; i >= 0; i--)
    {
        qDebug() << i << " : " << QString::number(bytes[i], 2);
        message.append(bytes[i]);
    }*/
    for(int i = 0; i < bytes.length(); i++)
    {
        qDebug() << i << " : " << QString::number(bytes[i], 2);
        message.append(bytes[i]);
    }

    qDebug() << "WritingMultipleCoils: " << Helpers::HexToDec(message);

    socket.write(message);

    messages[transactionID.word] = QPair<QTime, ModbusResponse>(QTime::currentTime(), ModbusResponse(message));

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

    //Append the length of the remaining bytes
    byteArray len;
    len.word = 7 + (length * 2);
    message.append(len.bytes[1]);
    message.append(len.bytes[0]);

    //Append slave Unit ID
    message.append(slaveAddress);

    //Append the function code
    message.append(SET_MULTIPLE_REGISTERS);

    //Append the starting address
    byteArray start;
    start.word = address;
    message.append(start.bytes[1]);
    message.append(start.bytes[0]);

    //Append the number of coils
    len.word = length;
    message.append(len.bytes[1]);
    message.append(len.bytes[0]);

    //Add remaining bytes count
    message.append(static_cast<char>(len.word * 2));

    byteArray reg;
    QList<short> values = dataTable->GetHoldingRegisters(start.word, len.word);
    for(int i = 0; i < len.word; i++)
    {
        reg.word = values[i];
        message.append(reg.bytes[1]);
        message.append(reg.bytes[0]);
    }

    socket.write(message);

    qDebug() << Helpers::HexToDec(message);

    messages[transactionID.word] = QPair<QTime, QByteArray>(QTime::currentTime(), message);

    //Increment the id
    transactionID.word += 1;
}

void ModbusMaster::HandleReadCoilsResponse(ModbusResponse message)
{
    ModbusResponse response(message);
    ModbusResponse sent;

    if(!messages.contains(response.GetTransactionID()))
    {
        qDebug() << "Not expecting a response to a message with transaction id " << response.GetTransactionID();
        return;
    }

    sent = messages[response.GetTransactionID()].second;
    messages.remove(response.GetTransactionID());

    //Check if the received equals the request
    if(response.GetDataBytes() * 8 < sent.GetQuantity() )
    {
        qDebug() << "Error: Received incorrect bytes from ReadCoils response";
        return;
    }

    //Parse the data
    QByteArray data = response.GetData();
    //Remove non data bytes
    data.remove(0, 3);
    dataTable->SetCoils(sent.GetStartAddress(), data, sent.GetQuantity());
}

void ModbusMaster::HandleReadDiscreteInputsResponse(ModbusResponse message)
{
    ModbusResponse response(message);
    ModbusResponse sent;

    if(!messages.contains(response.GetTransactionID()))
    {
        qDebug() << "Not expecting a response to a message with transaction id " << response.GetTransactionID();
        return;
    }

    sent = messages[response.GetTransactionID()].second;
    messages.remove(response.GetTransactionID());

    //Check if the received equals the request
    if(response.GetDataBytes() * 8 < sent.GetQuantity() )
    {
        qDebug() << "Error: Received incorrect bytes from ReadCoils response";
        return;
    }

    //Parse the data
    QByteArray data = response.GetData();
    //Remove non data bytes
    data.remove(0, 3);
    dataTable->SetDiscreteInputs(sent.GetStartAddress(), data, sent.GetQuantity());
}

void ModbusMaster::HandleReadHoldingRegistersResponse(ModbusResponse message)
{
    ModbusResponse response(message);
    ModbusResponse sent;

    if(!messages.contains(response.GetTransactionID()))
    {
        qDebug() << "Not expecting a response to a message with transaction id " << response.GetTransactionID();
        return;
    }

    sent = messages[response.GetTransactionID()].second;
    messages.remove(response.GetTransactionID());

    //Check if the received equals the request
    if((response.GetDataBytes() / 2) != sent.GetQuantity())
    {
        qDebug() << "Error: Received incorrect bytes from ReadCoils response";
        return;
    }

    //Parse the data
    QByteArray data = response.GetData();
    //Remove non data bytes
    data.remove(0, 3);
    dataTable->SetHoldingRegisters(sent.GetStartAddress(), data);
}

void ModbusMaster::HandleReadInputRegistersResponse(ModbusResponse message)
{
    ModbusResponse response(message);
    ModbusResponse sent;

    if(!messages.contains(response.GetTransactionID()))
    {
        qDebug() << "Not expecting a response to a message with transaction id " << response.GetTransactionID();
        return;
    }

    sent = messages[response.GetTransactionID()].second;
    messages.remove(response.GetTransactionID());

    //Check if the received equals the request
    if((response.GetDataBytes() / 2) != sent.GetQuantity())
    {
        qDebug() << "Error: Received incorrect bytes from ReadCoils response";
        return;
    }

    //Parse the data
    QByteArray data = response.GetData();
    //Remove non data bytes
    data.remove(0, 3);
    dataTable->SetInputRegisters(sent.GetStartAddress(), data);
}

//This does nothing, the message is echoed back by the slave
void ModbusMaster::HandleWriteSingleCoilResponse(ModbusResponse message)
{
    ModbusResponse response(message);
    ModbusResponse sent;

    if(!messages.contains(response.GetTransactionID()))
    {
        qDebug() << "Not expecting a response to a message with transaction id " << response.GetTransactionID();
        return;
    }

    sent = messages[response.GetTransactionID()].second;
    messages.remove(response.GetTransactionID());

    if(response.GetMessage() != sent.GetMessage())
        qDebug() << "Error: received bad response for WriteSingleCoil";
}

void ModbusMaster::HandleWriteSingleRegisterResponse(ModbusResponse message)
{
    ModbusResponse response(message);
    ModbusResponse sent;

    if(!messages.contains(response.GetTransactionID()))
    {
        qDebug() << "Not expecting a response to a message with transaction id " << response.GetTransactionID();
        return;
    }

    sent = messages[response.GetTransactionID()].second;
    messages.remove(response.GetTransactionID());

    if(response.GetMessage() != sent.GetMessage())
        qDebug() << "Error: received bad response for WriteSingleRegister";
}

void ModbusMaster::HandleWriteMultipleCoilsResponse(ModbusResponse message)
{
    ModbusResponse response(message);
    ModbusResponse sent;

    if(!messages.contains(response.GetTransactionID()))
    {
        qDebug() << "Not expecting a response to a message with transaction id " << response.GetTransactionID();
        return;
    }

    sent = messages[response.GetTransactionID()].second;
    messages.remove(response.GetTransactionID());

    if(response.GetStartAddress() != sent.GetStartAddress() || response.GetQuantity() != sent.GetQuantity())
        qDebug() << "Error: received bad response for WriteMultipleCoils";
}

void ModbusMaster::HandleWriteMultipleRegistersResponse(ModbusResponse message)
{
    ModbusResponse response(message);
    ModbusResponse sent;

    if(!messages.contains(response.GetTransactionID()))
    {
        qDebug() << "Not expecting a response to a message with transaction id " << response.GetTransactionID();
        return;
    }

    sent = messages[response.GetTransactionID()].second;
    messages.remove(response.GetTransactionID());

    if(response.GetStartAddress() != sent.GetStartAddress() || response.GetQuantity() != sent.GetQuantity())
        qDebug() << "Error: received bad response for WriteMultipleRegisters";
}
