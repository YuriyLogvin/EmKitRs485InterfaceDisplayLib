/*
 * Bms.h
 *
 *  Created on: 2 זמגע. 2018 נ.
 *      Author: banz
 */

#ifndef BMS_H_
#define BMS_H_

struct Bms {
	unsigned char DeviceState;
	unsigned char ElementsCount;
	bool PowerIsOn;
	bool ChargerIsOn;
	bool PowerOutEnable;
	bool ChargeEnable;
	short Thermo;
	short Voltage;
	short Current;
	short BoardVoltage;
};

#endif /* BMS_H_ */
