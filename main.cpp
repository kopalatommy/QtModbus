#include <QCoreApplication>

#include <QDebug>

#include "modbus.h"
#include "modbusslave.h"
#include "modbusmaster.h"
#include "modbusdatatable.h"

#define BUILD_NUMBER 2

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Started modbus library ver " << BUILD_NUMBER;

    ModbusDatatable * datatable = new ModbusDatatable(100, 100, 100, 100);

    ModbusSlave m(datatable, 1);
    m.Start(502, 1);

    /*ModbusMaster m;
    m.Start("172.22.0.100", 502, 1);
    m.ReadCoils(0, 10);*/

    return a.exec();
}
