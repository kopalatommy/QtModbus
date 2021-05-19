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

    char initialCoils = 0b01010101;
    char initialInputs = 0b00000001;

    QList<char> coils = {initialCoils};
    QList<char> inputs = {initialInputs};

    QList<short> discrete = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    QList<short> regs = {10, 11, 12, 13, 14 ,15, 16, 17, 18, 19};

    datatable->SetCoils(0, ModbusDatatable::ConvertList(coils));
    datatable->SetDiscreteInputs(0, ModbusDatatable::ConvertList(inputs));
    datatable->SetInputRegisters(0, regs);
    datatable->SetHoldingRegisters(0, discrete);

    /*ModbusSlave m(datatable, 1);
    m.Start(502, 1);

    QByteArray arr;
    arr.append('\0');
    arr.append('\0');
    arr.append('\0');
    arr.append('\0');
    arr.append('\0');
    arr.append(6);
    arr.append('\1');
    arr.append(0x03);
    arr.append('\0');
    arr.append('\0');
    arr.append('\0');
    arr.append('\2');

    m.ReadHoldingRegisters(arr);*/

    ModbusMaster m;
    m.Start("172.22.0.100", 502, 1, datatable);
    m.ReadCoils(0, 10);

    return a.exec();
}
