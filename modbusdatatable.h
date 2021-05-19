#ifndef MODBUSDATATABLE_H
#define MODBUSDATATABLE_H

#include <QObject>
#include <QDebug>

#define MAX_COILS 65535
#define MAX_DISCRETE_INPUTS 65535
#define MAX_INPUT_REGISTERS 65535
#define MAX_HOLDING_REGISTERS 65535

class ModbusDatatable : public QObject
{
    Q_OBJECT

public:
    ModbusDatatable();
    ModbusDatatable(int numberOfCoils, int numberOfDiscreteInputs,
                    int numberOfInputRegisters, int numberOfHoldingRegisters);

    void AddCoils(int quantity);
    void AddCoils(int quantity, QList<bool> initialValues);
    void ResizeCoils(int quantity);
    int GetNumberOfCoils();
    void SetCoil(int address, bool value);
    void SetCoils(int address, QList<bool> values);
    bool GetCoil(int address);
    QList<bool> GetCoils(int startAddress, int quantity);

    void AddDiscreteInputs(int quantity);
    void AddDiscreteInputs(int quantity, QList<bool> initialValues);
    void ResizeDiscreteInputs(int quantity);
    int GetNumberOfDiscreteInputs();
    bool SetDiscreteInput(int address, bool value);
    bool SetDiscreteInputs(int address, QList<bool> values);
    bool GetDiscreteInput(int address);
    QList<bool> GetDiscreteInputs(int startAddress, int quantity);

    void AddInputRegisters(int quantity);
    void AddInputRegisters(int quantity, QList<short> initialValues);
    void ResizeInputRegisters(int quantity);
    int GetNumberOfInputRegisters();
    bool SetInputRegister(int address, short value);
    bool SetInputRegisters(int address, QList<short> values);
    short GetInputRegister(int address);
    QList<short> GetInputRegisters(int startAddress, int quantity);

    void AddHoldingRegisters(int quantity);
    void AddHoldingRegisters(int quantity, QList<short> initialValues);
    void ResizeHoldingRegisters(int quantity);
    int GetNumberOfHoldingRegisters();
    bool SetHoldingRegister(int address, short value);
    bool SetHoldingRegisters(int address, QList<short> values);
    short GetHoldingRegister(int address);
    QList<short> GetHoldingRegisters(int startAddress, int quantity);

    static QList<bool> ConvertList(QList<char> original);
    static QList<char> ConvertList(QList<bool> original);
    static bool CheckBit(int val, int bit);
    static int SetBit(int val, int bit);
    static int ClearBit(int val, int bit);

    static void CopyBools(bool * source, bool * dest, int quantity);
    static void CopyShorts(short * source, short * dest, int quantity);

public slots:

signals:
    void AlteredCoilsTable();
    void AlteredDiscreteInputs();
    void AlteredInputRegisters();
    void AlteredHoldingRegisters();

private slots:

private:
    int numberOfCoils = 0;
    int numberOfDiscreteInputs = 0;
    int numberOfInputRegisters = 0;
    int numberOfHoldingRegisters = 0;

    bool * coils = Q_NULLPTR;
    bool * discreteInputs = Q_NULLPTR;
    short * inputRegisters = Q_NULLPTR;
    short * holdingRegisters = Q_NULLPTR;
};

#endif // MODBUSDATATABLE_H
