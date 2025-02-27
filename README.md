# EmKitRs485InterfaceDisplayLib
Allows to get BMS info via EmKit RS485 bus.

## Description

EmKitRs485InterfaceDisplayLib is an **experimental** library to give 
access **EmKitBMS data**.
The library implements EmKitRs485InterfaceDisplay class.


#### Init class

'''
#include <EmKitRs485InterfaceDisplay.h>
void setup() 
{
  ...
  _EmKitRs485InterfaceDisplay = new EmKitRs485InterfaceDisplay(&Serial, PA1);
  ...
}

'''
**PA1** - pin enables transmit via RS485. If using avtomatic resolving direction use **NC**


#### Using class

'''
  int val1 = 0, val2 = 0;
  EmKitRs485InterfaceDisplay::DataTypes res = _EmKitRs485InterfaceDisplay->TryGetData(val1, val2);
  switch (res)
  {
    case EmKitRs485InterfaceDisplay::DataTypes::Voltage:
      sprintf(strVoltage, "%i.%i", val1/10, val1%10);
      break;
      ...
    case EmKitRs485InterfaceDisplay::DataTypes::CellState:
	{
          const Cell& lci = _EmKitRs485InterfaceDisplay->GetLastCellsInfo();
	  /* use lci for giving cell information */
	}
      break;
      ...
  }

'''