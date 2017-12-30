/*
 * sci_adc  - 
 * Copyright (C) 2017, Switchcraft AS.
 *
 *
 *     This file is part of sci_adc.
 *
 *   sci_adc is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   sci_adc is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with sci_adc.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file sci_adc.h
 *  @brief Main C source file.
 *
 *  This is the main C source file.
 *
 *  Code is written for clarity, not efficiency.
 *
 *  Created on: Dec 28, 2017
 *
 *  @author Eirik Haustveit (la2kta)
 *  @bug No known bugs.
 */
#ifndef SCI_ADC_H_
#define SCI_ADC_H_

#include "f2802x_headers/include/F2802x_Device.h" /* Includes all the pheripheral header files. */
#include "f2802x_common/include/f2802x_examples.h" /* Useful for the DELAY_US function, and some clock definitions. */


void scia_tx_string(char * data);
void scia_tx(char data);

void scia_tx_number_as_string(uint16_t data);
char num_to_ascii(char data);

#endif /* SCI_ADC_H_ */
