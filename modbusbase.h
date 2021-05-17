#ifndef MODBUSBASE_H
#define MODBUSBASE_H

#include <QObject>

#include "modbusdatatable.h"

//Function codes
#define READ_COILS              0x01
#define READ_INPUTS             0x02
#define READ_HOLDING_REGISTERS  0x03
#define READ_INPUT_REGISTERS    0x04
#define SET_SINGLE_COIL         0x05
#define SET_SINGLE_REGISTER     0x06
#define SET_MULTIPLE_COILS      0x0F
#define SET_MULTIPLE_REGISTERS  0x10

enum ModbusState{
    NotConnected,
    Waiting,
    Connected
};

union byteArray{
    char bytes[2];
    short word;
};

class ModbusBase : public QObject
{
    Q_OBJECT

public:
    ModbusBase();
    ModbusBase(ModbusDatatable * datatable);

    bool IsConnected();
    ModbusState GetState();
    QString GetErrorString();

signals:
    void NewState(ModbusState newState);

protected:
    bool isConnected = false;
    ModbusState state = ModbusState::NotConnected;
    QString errorString = "No Error";

    ModbusDatatable * dataTable = Q_NULLPTR;
};

#endif // MODBUSBASE_H
