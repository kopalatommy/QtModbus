#include "modbusdatatable.h"

ModbusDatatable::ModbusDatatable()
{

}

ModbusDatatable::ModbusDatatable(int numberOfCoils, int numberOfDiscreteInputs,
                                 int numberOfInputRegisters, int numberOfHoldingRegisters)
{
    AddCoils(numberOfCoils);
    qDebug() << "Added coils";
    AddDiscreteInputs(numberOfDiscreteInputs);
    qDebug() << "Added discrete inputs";
    AddDiscreteInputs(numberOfInputRegisters);
    qDebug() << "Added input registers";
    AddHoldingRegisters(numberOfHoldingRegisters);
    qDebug() << "Added holding registers";
}

void ModbusDatatable::AddCoils(int quantity)
{
    //Check if can add coils
    if(quantity > 0 && numberOfCoils + quantity < MAX_COILS)
    {
        //Create the new array
        bool * newCoils = new bool[numberOfCoils + quantity];

        for(int i = 0; i < numberOfCoils + quantity; i++)
            newCoils[i] = 0;

        //Copy previous data
        CopyBools(coils, newCoils, numberOfCoils);
        //Free the old memory
        delete [] (coils);
        //Store the new array
        coils = newCoils;
        //Set the coil size value
        numberOfCoils += quantity;
    }
    else
    {
        qDebug() << "Error adding coils. Tried adding  " << quantity;
    }
}

void ModbusDatatable::AddCoils(int quantity, QList<bool> initialValues)
{
    if(quantity > 0 && quantity == initialValues.length() && numberOfCoils + quantity < MAX_COILS)
    {
        //Create the new array
        bool * newCoils = new bool[numberOfCoils + quantity];
        for(int i = 0; i < numberOfCoils + quantity; i++)
            newCoils[i] = 0;
        //Copy previous data
        CopyBools(coils, newCoils, numberOfCoils);
        //Free the old memory
        delete [] (coils);
        //Store the new array
        coils = newCoils;
        //Set the number of coils
        numberOfCoils += quantity;
        //Apply the initial values
        for(int i = numberOfCoils; i < numberOfCoils + quantity; i++)
        {
            *(newCoils + i) = initialValues[i];
        }
        //Update the number of coils
        numberOfCoils += quantity;
    }
}

void ModbusDatatable::ResizeCoils(int quantity)
{
    if(quantity >= 0 && quantity <= MAX_COILS && quantity != numberOfCoils)
    {
        if(quantity > numberOfCoils)
        {
            AddCoils(numberOfCoils - quantity);
        }
        else
        {
            //Create the new array
            bool * newCoils = new bool[quantity];
            //Copy the previous data
            CopyBools(coils, newCoils, quantity);
            //Free the previous version
            delete [] (coils);
            //Store the new array
            coils = newCoils;
            //Set the number of coils
            numberOfCoils = quantity;
        }
    }
    else
    {
        qDebug() << "Error resizing coils. Quantity: " << quantity;
    }
}

int ModbusDatatable::GetNumberOfCoils()
{
    return numberOfCoils;
}

bool ModbusDatatable::SetCoil(int address, bool value)
{
    if(address >= 0 && address < numberOfCoils)
    {
        coils[address] = value;
        return true;
    }
    else
    {
        qDebug() << "Unable to set coil at " << address << " to " << value;
        return false;
    }
}

bool ModbusDatatable::SetCoils(int address, QList<bool> values)
{
    if(address >= 0 && address + values.length() < numberOfCoils)
    {
        for(int i = 0; i < values.length(); i++)
        {
            coils[i + address] = values[i];
        }
        return true;
    }
    else
    {
        qDebug() << "Unable to sets coil starting at " << address << " for " << values.length();
        return false;
    }
}


bool ModbusDatatable::GetCoil(int address)
{
    if(address >= 0 && address < numberOfCoils)
    {
        return coils[address];
    }
    else
    {
        throw new std::runtime_error("Out of bounds in GetCoil");
    }
}

QList<bool> ModbusDatatable::GetCoils(int startAddress, int quantity)
{
    if(startAddress >= 0 && startAddress + quantity < numberOfCoils)
    {
        QList<bool> values;
        for(int i = 0; i < quantity; i++)
        {
            values.append(coils[i + startAddress]);
        }
        return values;
    }
    else
    {
        throw new std::runtime_error("Error: Out of bounds in GetCoils");
    }
}

void ModbusDatatable::AddDiscreteInputs(int quantity)
{
    if(quantity > 0 && quantity + numberOfDiscreteInputs < MAX_DISCRETE_INPUTS)
    {
        //Create the new array
        bool * newInputs = new bool[quantity + numberOfDiscreteInputs];
        for(int i = 0; i < numberOfDiscreteInputs + quantity; i++)
            newInputs[i] = 0;
        //Copy the new values
        CopyBools(discreteInputs, newInputs, numberOfDiscreteInputs);
        //Free the old array
        delete [] (discreteInputs);
        //Store the new array
        discreteInputs = newInputs;
        //Update the quantity value
        numberOfDiscreteInputs += quantity;
    }
    else
    {
        throw new std::runtime_error("Error: Failed to create new discrete inputs");
    }
}

void ModbusDatatable::AddDiscreteInputs(int quantity, QList<bool> initialValues)
{
    if(quantity > 0 && quantity == initialValues.count() && quantity + numberOfDiscreteInputs < MAX_DISCRETE_INPUTS)
    {
        //Create the new array
        bool * newInputs = new bool[quantity + numberOfDiscreteInputs];
        for(int i = 0; i < numberOfDiscreteInputs + quantity; i++)
            newInputs[i] = 0;
        //Copy the new values
        CopyBools(discreteInputs, newInputs, numberOfDiscreteInputs);
        //Free the old array
        delete [] (discreteInputs);
        //Store the new array
        discreteInputs = newInputs;
        //Apply the initial values
        for(int i = numberOfCoils; i < numberOfCoils + quantity; i++)
        {
            *(discreteInputs + i) = initialValues[i];
        }
        numberOfDiscreteInputs += quantity;
    }
    else
    {
        throw new std::runtime_error("Error: Failed to create new discrete inputs");
    }
}

void ModbusDatatable::ResizeDiscreteInputs(int quantity)
{
    if(quantity >= 0 && quantity <= MAX_DISCRETE_INPUTS && quantity != numberOfDiscreteInputs)
    {
        if(numberOfDiscreteInputs < quantity)
        {
            AddDiscreteInputs(quantity - numberOfDiscreteInputs);
        }
        else
        {
            //Create the new array
            bool * newInputs = new bool[quantity];
            //Copy the previous data
            CopyBools(newInputs, newInputs, quantity);
            //Free the previous version
            delete [] (discreteInputs);
            //Store the new array
            discreteInputs = newInputs;
            //Set the quantity
            numberOfDiscreteInputs = quantity;
        }
    }
    else
    {
        throw new std::runtime_error("Error: Failed to resize Discrete Inputs");
    }
}

int ModbusDatatable::GetNumberOfDiscreteInputs()
{
    return numberOfDiscreteInputs;
}

bool ModbusDatatable::SetDiscreteInput(int address, bool value)
{
    if(address >= 0 && address < numberOfDiscreteInputs)
    {
        discreteInputs[address] = value;
        return true;
    }
    else
    {
        return false;
    }
}

bool ModbusDatatable::SetDiscreteInputs(int address, QList<bool> values)
{
    if(address >= 0 && address + values.length() < numberOfDiscreteInputs)
    {
        for(int i = 0; i < values.length(); i++)
        {
            discreteInputs[address + i] = values[i];
        }
        return true;
    }
    else
    {
        throw new std::runtime_error("Error: Index out of bounds in Set Discrete Inputs");
    }
}

bool ModbusDatatable::GetDiscreteInput(int address)
{
    if(address >= 0 && address < numberOfDiscreteInputs)
    {
        return discreteInputs[address];
    }
    else
    {
        throw new std::runtime_error("Error: Index out of bounds in GetDiscreteInput");
    }
}

QList<bool> ModbusDatatable::GetDiscreteInputs(int startAddress, int quantity)
{
    if(startAddress >= 0 && startAddress + quantity < numberOfDiscreteInputs)
    {
        QList<bool> values;
        for(int i = startAddress; i < startAddress + quantity; i++)
        {
            values.append(discreteInputs[i]);
        }
        return values;
    }
    else
    {
        throw new std::runtime_error("Error: Index out of bounds in GetDiscreteInputs");
    }
}

void ModbusDatatable::AddInputRegisters(int quantity)
{
    if(quantity >= 0 && numberOfInputRegisters + quantity < MAX_HOLDING_REGISTERS)
    {
        //Create the new array
        short * newRegs = new short[quantity + numberOfInputRegisters];
        for(int i = 0; i < numberOfInputRegisters + quantity; i++)
            newRegs[i] = 0;
        //Copy over the new values
        CopyShorts(inputRegisters, newRegs, numberOfInputRegisters);
        //Free the old array
        delete [] (inputRegisters);
        //Store the new data array
        inputRegisters = newRegs;
        //Update the quantity
        numberOfInputRegisters += quantity;
    }
    else
    {
        throw new std::runtime_error("Error: Unable to add new Input Registers");
    }
}

void ModbusDatatable::AddInputRegisters(int quantity, QList<short> initialValues)
{
    if(quantity >= 0 && numberOfInputRegisters + quantity < MAX_HOLDING_REGISTERS)
    {
        //Create the new array
        short * newRegs = new short[quantity + numberOfInputRegisters];
        for(int i = 0; i < numberOfInputRegisters + quantity; i++)
            newRegs[i] = 0;
        //Copy over the new values
        CopyShorts(inputRegisters, newRegs, numberOfInputRegisters);
        //Free the old array
        delete [] (inputRegisters);
        //Store the new data array
        inputRegisters = newRegs;
        //Set the initial values
        for(int i = 0; i < initialValues.length(); i++)
        {
            inputRegisters[quantity + i] = initialValues[i];
        }
        //Update the quantity
        numberOfInputRegisters += quantity;
    }
    else
    {
        throw new std::runtime_error("Error: Unable to add new Holding Registers");
    }
}

void ModbusDatatable::ResizeInputRegisters(int quantity)
{
    if(quantity >= 0 && quantity <= MAX_HOLDING_REGISTERS && quantity != numberOfInputRegisters)
    {
        if(quantity > numberOfInputRegisters)
        {
            AddHoldingRegisters(quantity - numberOfInputRegisters);
        }
        else
        {
            //Create the new array
            short * newRegs = new short[quantity];
            //Copy the data
            CopyShorts(inputRegisters, newRegs, quantity);
            //Free the old array
            delete [] (inputRegisters);
            //Save the new array
            inputRegisters = newRegs;
            //Update the size value
            numberOfInputRegisters = quantity;
        }
    }
    else
    {
        throw new std::runtime_error("Error: Unable to resize Input Registers");
    }
}

int ModbusDatatable::GetNumberOfInputRegisters()
{
    return numberOfInputRegisters;
}

bool ModbusDatatable::SetInputRegister(int address, short value)
{
    if(address >= 0 && address <= numberOfInputRegisters)
    {
        inputRegisters[address] = value;
        return true;
    }
    else
    {
        throw new std::runtime_error("Error: Unable to set Input Register");
    }
}

bool ModbusDatatable::SetInputRegisters(int address, QList<short> values)
{
    if(address >= 0 && address + values.length() < numberOfInputRegisters)
    {
        for(int i = 0; i < values.length(); i++)
        {
            inputRegisters[i + address] = values[i];
        }
        return true;
    }
    else
    {
        throw new std::runtime_error("Error: Unable to add Input Registers");
    }
}

short ModbusDatatable::GetInputRegister(int address)
{
    if(address >= 0 && address < numberOfInputRegisters)
    {
        return inputRegisters[address];
    }
    else
    {
        throw new std::runtime_error("Error: Unable to GetInputRegister");
    }
}

QList<short> ModbusDatatable::GetInputRegisters(int startAddress, int quantity)
{
    if(startAddress >= 0 && startAddress + quantity < numberOfInputRegisters)
    {
        QList<short> values;
        for(int i = startAddress; i < startAddress + quantity; i++)
        {
            values.append(inputRegisters[i]);
        }
        return values;
    }
    else
    {
        throw new std::runtime_error("Error: Unable to get input registers");
    }
}

void ModbusDatatable::AddHoldingRegisters(int quantity)
{
    if(quantity > 0 && quantity + numberOfHoldingRegisters < MAX_HOLDING_REGISTERS)
    {
        //Create the new array
        short * newRegs = new short[quantity + numberOfHoldingRegisters];
        for(int i = 0; i < numberOfHoldingRegisters + quantity; i++)
            newRegs[i] = 0;
        //Copy the new values
        CopyShorts(holdingRegisters, newRegs, numberOfHoldingRegisters);
        //Free up the old array
        delete [] (holdingRegisters);
        //Save the new array
        holdingRegisters = newRegs;
        //Update the quantity value
        numberOfHoldingRegisters += quantity;
    }
    else
    {
        throw new std::runtime_error("Error: Unable to add holding registers");
    }
}

void ModbusDatatable::AddHoldingRegisters(int quantity, QList<short> initialValues)
{
    if(quantity > 0 && quantity + numberOfHoldingRegisters < MAX_HOLDING_REGISTERS)
    {
        //Create the new array
        short * newRegs = new short[quantity + numberOfHoldingRegisters];
        for(int i = 0; i < numberOfHoldingRegisters + quantity; i++)
            newRegs[i] = 0;
        //Copy the new values
        CopyShorts(holdingRegisters, newRegs, quantity + numberOfHoldingRegisters);
        //Free up the old array
        delete [] (holdingRegisters);
        //Save the new array
        holdingRegisters = newRegs;
        for(int i = 0; i < initialValues.length(); i++)
        {
            holdingRegisters[i + numberOfHoldingRegisters] = initialValues[i];
        }
        //Update the quantity value
        numberOfHoldingRegisters += quantity;
    }
    else
    {
        throw new std::runtime_error("Error: Unable to add holding registers");
    }
}

void ModbusDatatable::ResizeHoldingRegisters(int quantity)
{
    if(quantity >= 0 && quantity < MAX_HOLDING_REGISTERS && quantity != numberOfHoldingRegisters)
    {
        if(quantity > numberOfHoldingRegisters)
        {
            AddHoldingRegisters(quantity - numberOfHoldingRegisters);
        }
        else
        {
            //Create new array
            short * newRegs = new short[quantity];
            //Copy the new data
            CopyShorts(holdingRegisters, newRegs, quantity);
            //Free up the old array
            delete [] (holdingRegisters);
            //Save the new array
            holdingRegisters = newRegs;
        }
    }
    else
    {
        throw new std::runtime_error("Error: Unable to resize holding registers");
    }
}

int ModbusDatatable::GetNumberOfHoldingRegisters()
{
    return numberOfHoldingRegisters;
}

bool ModbusDatatable::SetHoldingRegister(int address, short value)
{
    if(address >= 0 && address < numberOfHoldingRegisters)
    {
        return holdingRegisters[address];
    }
    else
    {
        throw new std::runtime_error("Error: Unable to set holding register");
    }
}

bool ModbusDatatable::SetHoldingRegisters(int address, QList<short> values)
{
    if(address >= 0 && address + values.length() < numberOfHoldingRegisters)
    {
        for(int i = 0; i < values.length(); i++)
        {
            holdingRegisters[i + address] = values[i];
        }
        return true;
    }
    else
    {
        throw new std::runtime_error("Error: Unable to set holding registers");
    }
}

short ModbusDatatable::GetHoldingRegister(int address)
{
    if(address >= 0 && address < numberOfHoldingRegisters)
    {
        return holdingRegisters[address];
    }
    else
    {
        throw new std::runtime_error("Error: Unable to get holding register. Index out of bounds");
    }
}

QList<short> ModbusDatatable::GetHoldingRegisters(int startAddress, int quantity)
{
    if(startAddress >= 0 && startAddress + quantity < numberOfHoldingRegisters)
    {
        QList<short> values;
        for(int i = startAddress; i < startAddress + quantity; i++)
        {
            values.append(holdingRegisters[i]);
        }
        return values;
    }
    else
    {
        throw new std::runtime_error("Error: Unable to get holding registers. Index out of bounds.");
    }
}

QList<bool> ModbusDatatable::ConvertList(QList<char> original)
{
    QList<bool> values;

    for(int i = 0, j; i < original.length(); i++)
    {
        for(j = 0; j < 8; j++)
        {
            values.append( original[i] & (1 << j) );
        }
    }
    return values;
}

QList<char> ModbusDatatable::ConvertList(QList<bool> original)
{
    QList<char> values;

    char temp = 0;
    for(int i = 0; i < original.length(); i++)
    {
        if(i % 8 == 0 && i != 0)
        {
            values.append(temp);
            temp = 0;
        }

        temp |= static_cast<char>(original[i]);
        temp <<= 1;
    }

    values.append(temp);

    return values;
}

bool ModbusDatatable::CheckBit(int val, int bit)
{
    return val & (1 << bit);
}

int ModbusDatatable::SetBit(int val, int bit)
{
    return val | (1 << bit);
}

int ModbusDatatable::ClearBit(int val, int bit)
{
    return val & ~(1 << bit);
}

void ModbusDatatable::CopyBools(bool * source, bool * dest, int quantity)
{
    for(int i = 0; i < quantity; i++)
    {
        dest[i] = source[i];
    }
}

void ModbusDatatable::CopyShorts(short * source, short * dest, int quantity)
{
    for(int i = 0; i < quantity; i++)
    {
        dest[i] = source[i];
    }
}
