#include "modbus.h"

Modbus::Modbus()
{
    if(tcpServer.listen(QHostAddress::Any, 502))
    {
        qDebug() << "Started server";
        connect(&tcpServer, &QTcpServer::newConnection, this, &Modbus::OnReceiveNewConnection);
    }
}

void Modbus::OnReceiveNewConnection()
{
    //Get the new connection
    QTcpSocket * newSocket = tcpServer.nextPendingConnection();

    //connect(newSocket, &QTcpSocket::connected, this, &ModbusHandler::OnSlaveConnected);
    connect(newSocket, &QTcpSocket::disconnected, this, &Modbus::OnSlaveDisconnected);
    connect(newSocket, &QTcpSocket::readyRead, this, &Modbus::OnSlaveReadyRead);
}

void Modbus::OnSlaveReadyRead()
{
    QByteArray data = reinterpret_cast<QTcpSocket*>(sender())->readAll();

    qDebug() << "Modbus: " << data << " : " << HexToDec(data);

    ParseMessage(data);
}

void Modbus::OnSlaveDisconnected()
{
    QTcpSocket * socket = reinterpret_cast<QTcpSocket*>(sender());
    qDebug() << "Disconnected";
    disconnect(socket, &QTcpSocket::disconnected, this, &Modbus::OnSlaveDisconnected);
    socket->deleteLater();
}

QString Modbus::HexToDec(QByteArray arr)
{
    if(arr.length() > 1)
    {
        QString t = QString::number(arr.at(0)).append(',');
        t.append(HexToDec(arr.remove(0, 1)));
        return t;
    }
    else
    {
        QString temp = QString::number(arr.at(0));
        return temp;
    }
}

void Modbus::ParseMessage(QByteArray arr)
{
    byteArray messageID;
    messageID.bytes[0] = arr[0];
    messageID.bytes[1] = arr[1];
    qDebug() << "Package ID: " << messageID.value;

    byteArray protocol;
    protocol.bytes[0] = arr[2];
    protocol.bytes[1] = arr[3];
    qDebug() << "Protocol: " << protocol.value;


   byteArray length;
   length.bytes[0] = arr[4];
   length.bytes[1] = arr[5];
   qDebug() << "Length: " << length.value;

   qDebug() << "Slave ID: " << QString::number(arr[6]);

   qDebug() << "Function: " << QString::number(arr[7]);
}

/*void ModbusSlave::HandleReadCoils(QByteArray data)
{
    char slaveAddress = data.at(0);

    char function = data.at(1);

    byteArray startAddress;
    startAddress.bytes[0] = data.at(2);
    startAddress.bytes[1] = data.at(3);

    byteArray quantity;
    startAddress.bytes[0] = data.at(4);
    startAddress.bytes[1] = data.at(5);

    qDebug() << "Remaining: " << data.remove(0, 6);

    QByteArray response;
    //Add slave address
    response.append(slaveAddress);
    //Add function
    response.append(function);
    //Add count

}*/



