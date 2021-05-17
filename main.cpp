#include <QCoreApplication>

#include "modbus.h"
#include "modbusslave.h"
#include "modbusmaster.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ModbusSlave m(Q_NULLPTR, 1);
    m.Start(502, 1);

    /*ModbusMaster m;
    m.Start("172.22.0.100", 502, 1);
    m.ReadCoils(0, 10);*/

    qDebug() << "Started modbus test";

    return a.exec();
}
