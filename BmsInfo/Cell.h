/*
 * Cell.h
 *
 *  Created on: 2 זמגע. 2018 נ.
 *      Author: banz
 */

#ifndef CELL_H_
#define CELL_H_

#include <stdint.h>

struct Cell
{
	Cell();
    unsigned char No;
    unsigned char State;
    short Voltage;
    short Thermo;
    short ThermoBallancer;
    unsigned char Version;
};

#endif /* CELL_H_ */
