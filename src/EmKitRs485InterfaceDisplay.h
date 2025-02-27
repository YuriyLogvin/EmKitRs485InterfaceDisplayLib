#pragma once

#include "Stream.h"
#include "BmsInfo/Cell.h"
#include "BmsInfo/Bms.h"
#include "Protocol/ProtocolHost.h"
#include "Interface/EmkStandartMetods.h"
#include "Interface/InterfaceMetodsDisplay.h"

class EmKitRs485InterfaceDisplay
{
    uint32_t _TransmitEnPin;

    HardwareSerial* _IoStream;
    int _TxBuffSize;

    ProtocolHost* _ProtocolHost;
    ReceiveMetodHost* _ReceiveMetodHost;
    SendMetodHost* _SendMetodHost;

    Cell _LastCellsInfo;
    
public:
    EmKitRs485InterfaceDisplay(HardwareSerial* ioStream, uint32_t transmitEnPin = NC);
    virtual ~EmKitRs485InterfaceDisplay();

    enum class DataTypes
    {
        None,
        Voltage,
        Current,
        Ah,
        AhTotal,
        Wh,
        WhTotal,
        Soc,
        DischargeState,
        ChargeState,
        VoltageExt,
        CurrentExt,
        CellState,
        MainTempSensor
    };

    DataTypes TryGetData(int& value1, int& value2);
    const Cell& GetLastCellsInfo();

    char* State() {return _StateBuff;};

private:
    char _StateBuff[128];
    DataTypes _ProcessDataPacket(int& value1, int& value2);
    void _ResponsePing();    
    void _SendData();
    int32_t _GetInt32FromReceiveMetodHost();    
};