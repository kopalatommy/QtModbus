#include "helpers.h"

Helpers::Helpers()
{

}

QString Helpers::HexToDec(QByteArray arr)
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
