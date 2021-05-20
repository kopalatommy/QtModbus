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
    /*m.ReadCoils(0, 10);
    m.ReadDiscreteInputs(0, 10);
    m.ReadHoldingRegisters(0, 10);
    m.ReadInputRegisters(0, 10);*/

    /*datatable->SetCoil(0, false);
    m.WriteSingleCoil(0);

    datatable->SetHoldingRegister(0, 0xFF);
    m.WriteSingleRegister(0);*/

    /*QList<bool> nCoils = {false, false, false, false, false, true, true, true, true, true};
    datatable->SetCoils(0, nCoils);
    qDebug() << "Before";
    m.WriteMultipleCoils(0, 10);
    qDebug() << "After";*/

    /*QList<bool> nCoils = {false, true, false, false, true, true, true, true, true, true, true, true, true, true, true, true};
    datatable->SetCoils(0, nCoils);
    QList<bool> base = datatable->GetCoils(0, 10);
    qDebug() << "Base: " << base;
    QList<char> values = ModbusDatatable::ConvertList(base);
    qDebug() << "Converted: " << values;
    for(int i = 0; i < values.length(); i++)
    {
        qDebug() << i << " - " << QString::number(values[i]);
    }

    qDebug() << "Before";
    m.WriteMultipleCoils(0, 10);
    qDebug() << "After";*/

    /*QList<bool> testLst = {false, true, true, true, true, true, true, true, true, false};
    datatable->SetCoils(0, testLst);
    m.WriteMultipleCoils(0, 10);*/

    QList<short> nRegs = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    datatable->SetHoldingRegisters(0, nRegs);
    m.WriteMultipleRegisters(0, 10);

    /*void WriteSingleCoil(short address);
    void WriteSingleRegister(short address);
    void WriteMultipleCoils(short address, short length);
    void WriteMultipleRegisters(short address, short length);*/

    return a.exec();
}
