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

/** @file sci_adc.c
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

#include "f28027_setup.h"
#include "sci_adc.h"

void main(void)
{
    system_setup();
    pie_setup();
    adc_setup();

    int temperature = 0;
    int potmeter = 0;

    scia_tx_string("\r\nInitialization complete.\r\n\0");

    for(;;)
    {
        scia_tx_string("Starting conversion SOC0.\r\n\0");
        AdcRegs.ADCSOCFRC1.bit.SOC0 = 1;

        while(AdcRegs.ADCINTFLG.bit.ADCINT1 == 0){}
        temperature = AdcResult.ADCRESULT0;
        AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
        scia_tx_string("SOC0 result:\0");
        scia_tx_number_as_string(temperature);

        DELAY_US(10000);

        scia_tx_string("Starting conversion SOC1.\r\n\0");
        AdcRegs.ADCSOCFRC1.bit.SOC1 = 1;

        while(AdcRegs.ADCINTFLG.bit.ADCINT2 == 0){}
        potmeter = AdcResult.ADCRESULT1;
        AdcRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;
        scia_tx_string("\r\nSOC1 result:\0");
        scia_tx_number_as_string(potmeter);


        DELAY_US(10000);
    }
}

void scia_tx(char data)
{
    while(SciaRegs.SCIFFTX.bit.TXFFST != 0){}
    SciaRegs.SCITXBUF = data;
}

void scia_tx_string(char * data)
{
    int i = 0;

    while(data[i] != '\0')
    {
        scia_tx(data[i]);
        i++;
    }
}

void scia_tx_number_as_string(uint16_t data)
{
    char lower_byte = data & 0x00ff;
    char higher_byte = (data & 0xff00) >> 8;

    char string[3];

    string[0] = num_to_ascii(higher_byte);
    string[1] = num_to_ascii(lower_byte);
    string[2] = '\0';

    scia_tx_string(string);
}

/**
 * Because we don't want to include <stdio.h>
 */
char num_to_ascii(char data)
{
    char return_data;

    switch(data)
    {
    case 48:
        return_data = '0';
    case 49:
        return_data = '1';
    case 50:
        return_data = '2';
    case 51:
        return_data = '3';
    case 52:
        return_data = '4';
    case 53:
        return_data = '5';
    case 54:
        return_data = '6';
    case 55:
        return_data = '7';
    case 56:
        return_data = '8';
    case 57:
        return_data = '9';
    default:
        return_data = '0';
    }

    return return_data;
}
