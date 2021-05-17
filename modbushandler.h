#ifndef MODBUSHANDLER_H
#define MODBUSHANDLER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

#include "modbusmaster.h"
#include "modbusslave.h"

class ModbusHandler : public QObject
{
    Q_OBJECT
public:
    ModbusHandler();

    bool IsMaster();
    bool IsSlave();
    bool IsConnected();

    void Disconnect();

public: // Master
    void ConnectToSlave(QString IPAddress, unsigned short targetPort, char unitID);

    ModbusMaster * GetMaster();
};

#endif // MODBUSHANDLER_H
