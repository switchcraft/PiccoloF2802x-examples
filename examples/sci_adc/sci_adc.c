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
    sci_setup();

    /* Configure the GPIO ports connected to the FTDI device. */
    EALLOW;
    GpioCtrlRegs.GPACTRL.bit.QUALPRD3 =
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 11; /* No qualification, i.e. asynchronous */

    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1; /* Configure port 28 as SCI RX. */
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0; /* Enable pullup on the RX pin. */

    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; /* Configure port 29 as SCI TX. */
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 1; /* Disable pullup on the TX pin. */
    EDIS;

    /* Configure the GPIO ports connected to the LED's */
    EALLOW;
    GpioDataRegs.GPASET.bit.GPIO0 = 1;         /* Load the output latch */
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;        /* Configure the MUX for GPIO */
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;         /* Configure the port for output */

    GpioDataRegs.GPASET.bit.GPIO1 = 1;         /* Load the output latch */
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;        /* Configure the MUX for GPI1 */
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;         /* Configure the port for output */

    GpioDataRegs.GPASET.bit.GPIO2 = 1;         /* Load the output latch */
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;        /* Configure the MUX for GPI2 */
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;         /* Configure the port for output */

    GpioDataRegs.GPASET.bit.GPIO3 = 1;         /* Load the output latch */
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;        /* Configure the MUX for GPI3 */
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;         /* Configure the port for output */
    EDIS;

    int temperature = 0;
    int potmeter = 0;

    scia_tx_string("\r\nInitialization complete.\r\n\0");

    for(;;)
    {
        scia_tx_string("=========================================\r\n\0");
        scia_tx_string("Starting conversion SOC0.\r\n\0");
        AdcRegs.ADCSOCFRC1.bit.SOC0 = 1;

        while(AdcRegs.ADCINTFLG.bit.ADCINT1 == 0){}
        temperature = AdcResult.ADCRESULT0;
        AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
        scia_tx_string("SOC0 result:\0");
        scia_tx_number_as_string(temperature);
        scia_tx_string("\r\n\0");

        GpioDataRegs.GPASET.bit.GPIO3 = 1;
        DELAY_US(100000);
        GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;

        scia_tx_string("Starting conversion SOC1.\r\n\0");
        AdcRegs.ADCSOCFRC1.bit.SOC1 = 1;

        while(AdcRegs.ADCINTFLG.bit.ADCINT2 == 0){}
        potmeter = AdcResult.ADCRESULT1;
        AdcRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;
        scia_tx_string("SOC1 result:\0");
        scia_tx_number_as_string(potmeter);
        scia_tx_string("\r\n\0");


        DELAY_US(100000);
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
    char nibble[4] = {0,0,0,0};
    char string[5] = {0,0,0,0};
    int i = 0;

    /**
     * Split 16 bit integer into 4-bit BCD coded nibbles.
     */
    while (data > 0) {
       nibble[i++] = (data % 10);
       data /= 10;
    }

    for(i = 0; i < 4; i++)
        string[i] = num_to_ascii(nibble[3-i]);

    string[4] = '\0';

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
    case 1:
        return_data = '1';
        break;
    case 2:
        return_data = '2';
        break;
    case 3:
        return_data = '3';
        break;
    case 4:
        return_data = '4';
        break;
    case 5:
        return_data = '5';
        break;
    case 6:
        return_data = '6';
        break;
    case 7:
        return_data = '7';
        break;
    case 8:
        return_data = '8';
        break;
    case 9:
        return_data = '9';
        break;
    default:
        return_data = '0';
        break;
    }

    return return_data;
}
