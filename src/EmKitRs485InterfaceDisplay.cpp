#include <Arduino.h>
#include "EmKitRs485InterfaceDisplay.h"


EmKitRs485InterfaceDisplay::EmKitRs485InterfaceDisplay(HardwareSerial* ioStream, uint32_t transmitEnPin)
{
    _IoStream = ioStream;
    _IoStream->begin(19200);
    _TxBuffSize = _IoStream->availableForWrite();

    _TransmitEnPin = transmitEnPin;
    if (_TransmitEnPin != NC)
    {
        pinMode(_TransmitEnPin, OUTPUT);
        digitalWrite(_TransmitEnPin, LOW);
    }

    _StateBuff[0] = 0;

    _ProtocolHost = new ProtocolHost(EmkAddr::Display);
	_ProtocolHost->DestAddr(EmkAddr::Host);

	_ReceiveMetodHost = new ReceiveMetodHost();

	_SendMetodHost = new SendMetodHost();
}

EmKitRs485InterfaceDisplay::~EmKitRs485InterfaceDisplay()
{
    _IoStream = NULL;
	if (_ProtocolHost)
        delete _ProtocolHost; _ProtocolHost = NULL;

	if (_ReceiveMetodHost) 
        delete _ReceiveMetodHost; _ReceiveMetodHost = NULL;

	if (_SendMetodHost) 
        delete _SendMetodHost; _SendMetodHost = NULL;
}

uint8_t _DebB[8];
uint8_t _DebBpos = 0;

EmKitRs485InterfaceDisplay::DataTypes EmKitRs485InterfaceDisplay::TryGetData(int& value1, int& value2)
{
    if (_IoStream == NULL)
        return DataTypes::None;

    uint8_t b = 0;
	uint8_t* data = 0;
	uint8_t len = 0;
    int readRes = 0;
    //__disable_irq();
    readRes = _IoStream->read();
    //__enable_irq();
    if (readRes < 0)
        return DataTypes::None;
    
    data = _ProtocolHost->ReceiveData(readRes, len);
	if (data == NULL)
        return DataTypes::None;
    
	_ReceiveMetodHost->InitNewMetod(data, len);
    auto pdpRes = _ProcessDataPacket(value1, value2);
    
    return pdpRes;
}

int _Pings = 0;

EmKitRs485InterfaceDisplay::DataTypes EmKitRs485InterfaceDisplay::_ProcessDataPacket(int& value1, int& value2)
{
    value1 = 0; value2 = 0;
	auto mNum = _ReceiveMetodHost->GetMetodNumber();

	if (_ProtocolHost->PacketAddr() != EmkAddr::Display)
        return DataTypes::None;

	if ((EmkMetods)mNum == EmkMetods::Ping)
	{
        sprintf(_StateBuff, "ping %i", _Pings++);

		_ResponsePing();
		return DataTypes::None;
	};

    short sValue = 0;

	switch ((InterfaceMetodsDisplay)mNum)
	{
    case InterfaceMetodsDisplay::DriveState:
		_ReceiveMetodHost->GetArgumentShort(0, sValue);
        value1 = sValue;
        return DataTypes::DischargeState;
    case InterfaceMetodsDisplay::ChargeState:
		_ReceiveMetodHost->GetArgumentShort(0, sValue);
        value1 = sValue;
        return DataTypes::ChargeState;
	case InterfaceMetodsDisplay::Voltage:
		_ReceiveMetodHost->GetArgumentShort(0, sValue);
        value1 = sValue;
        return DataTypes::Voltage;
	case InterfaceMetodsDisplay::Current:
        _ReceiveMetodHost->GetArgumentShort(0, sValue);
        value1 = sValue;
        return DataTypes::Current;
    case InterfaceMetodsDisplay::Ah:
        value1 = _GetInt32FromReceiveMetodHost();
        return DataTypes::Ah;	
    case InterfaceMetodsDisplay::AhTotal:
        value1 = _GetInt32FromReceiveMetodHost();
        return DataTypes::AhTotal;	
    case InterfaceMetodsDisplay::Wh:
        value1 = _GetInt32FromReceiveMetodHost();
        return DataTypes::Wh;	
    case InterfaceMetodsDisplay::WhTotal:
        value1 = _GetInt32FromReceiveMetodHost();
        return DataTypes::WhTotal;	
    case InterfaceMetodsDisplay::Soc:
        _ReceiveMetodHost->GetArgumentShort(0, sValue);
        value1 = sValue;
        if (value1 < 0)
            value1 = 0;
        return DataTypes::Soc;
    case InterfaceMetodsDisplay::Temperature1:
        _ReceiveMetodHost->GetArgumentShort(0, sValue);
        value1 = sValue;
        return DataTypes::MainTempSensor;

    case InterfaceMetodsDisplay::CellState:
        if (!_ReceiveMetodHost->GetArgumentByte(0, _LastCellsInfo.No))
            break;
        _ReceiveMetodHost->GetArgumentByte(1, _LastCellsInfo.State);
        _ReceiveMetodHost->GetArgumentShort(2, _LastCellsInfo.Voltage);
        _ReceiveMetodHost->GetArgumentShort(3, _LastCellsInfo.Thermo);
        _ReceiveMetodHost->GetArgumentShort(4, _LastCellsInfo.ThermoBallancer);
        _ReceiveMetodHost->GetArgumentByte(5, _LastCellsInfo.Version);
        return DataTypes::CellState;
	default:
        return DataTypes::None;
	};

	return DataTypes::None;
}

const Cell& EmKitRs485InterfaceDisplay::GetLastCellsInfo()
{
    return _LastCellsInfo;
}

void EmKitRs485InterfaceDisplay::_ResponsePing()
{
	_SendMetodHost->InitNewMetod((uint8_t)EmkMetods::Ping);

	_SendMetodHost->AddArgumentBool(true);

	//version
	_SendMetodHost->AddArgumentByte(2);

	_SendData();
}

void EmKitRs485InterfaceDisplay::_SendData()
{

	_ProtocolHost->InitSendData(*_SendMetodHost);

	uint8_t buff[32];

	uint8_t pos = 0;
	for (; pos < sizeof(buff); pos++)
	{
		uint8_t b;
		if (!_ProtocolHost->SendData(b))
			break;
		buff[pos] = b;
	}

	if (pos)
	{
		//between receive and send response must be more 100 mkSec
        delay(1);
        if (_TransmitEnPin != NC)
            digitalWrite(_TransmitEnPin, HIGH);
        
        _IoStream->write(buff, pos);

        while (_IoStream->availableForWrite() < _TxBuffSize);

        if (_TransmitEnPin != NC)
            digitalWrite(_TransmitEnPin, LOW);
	}
}

int32_t EmKitRs485InterfaceDisplay::_GetInt32FromReceiveMetodHost()
{
	int32_t res = 0;
	int16_t val = 0;
	_ReceiveMetodHost->GetArgumentShort(0, val);
	res = (uint16_t)val;
	if (_ReceiveMetodHost->GetArgumentShort(1, val))
		res |= (int32_t)val << 16;
	return res;
}
