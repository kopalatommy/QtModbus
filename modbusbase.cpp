#include "modbusbase.h"

ModbusBase::ModbusBase()
{

}

bool ModbusBase::IsConnected()
{
    return isConnected;
}

ModbusState ModbusBase::GetState()
{
    return state;
}

QString ModbusBase::GetErrorString()
{
    return errorString;
}
