#ifndef HELPERS_H
#define HELPERS_H

#include <QDebug>
#include <QString>

class Helpers
{
public:
    Helpers();

    static QString HexToDec(QByteArray arr);
};

#endif // HELPERS_H
