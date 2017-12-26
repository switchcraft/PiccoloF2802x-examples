/*
 * gpio_interrupt - Example application for the LAUNCHXL-TMS320F28027F
 * Copyright (C) 2017, Switchcraft AS.
 *
 *
 *     This file is part of gpio_interrupt.
 *
 *   gpio_interrupt is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   gpio_interrupt is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with gpio_interrupt.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file gpio_interrupt.c
 *  @brief Main C source file.
 *
 *  This is the main C source file.
 *
 *  Code is written for clarity, not efficiency.
 *
 *  @author Eirik Haustveit (la2kta)
 *  @bug No known bugs.
 */

#include "f2802x_headers/include/F2802x_Device.h" /* Includes all the pheripheral header files. */
#include "f2802x_common/include/f2802x_examples.h" /* Useful for the DELAY_US function, and some clock definitions. */

#include "gpio_interrupt.h"
#include "f28027_setup.h"

void gpio_setup(void)
{
    /* The addresses of the ISR routines defined in this file are
     * placed in the PIE vector table.
     */
    EALLOW;
    PieVectTable.XINT1 = &push_button_isr;
    //PieVectTable.XINT2 = &push_button2_isr;
    EDIS;

    /* Enable XINT1, which is connected to PIE group 1 INT 1.4.
     *
     * See table 109 page 121 in the Piccolo System Control and Interrupts Reference Guide .
     */
    EALLOW;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          /* Enable the PIE block */
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          /* Enable PIE Group 1 INT4 */
    PieCtrlRegs.PIEACK.all = 0xFFFF;

    IER |= M_INT1;                              /* Enable CPU INT1 */
    EINT;                                       /* Enable Global Interrupts */
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

    /* Configure the GPIO port connected to the push button
     *
     * QUALPRD1 specifies the sampling period for GPIO 8 to 15.
     */
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;         /* Configure the MUX for GPI12 */
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;          /* Configure the port for input */
    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 0;        /* XINT1 Synch to SYSCLKOUT only */
    GpioCtrlRegs.GPACTRL.bit.QUALPRD1 = 0x0F;    /* Each sampling window is 510*SYSCLKOUT */

    GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 0b01100; // XINT1 is GPIO12

    XIntruptRegs.XINT1CR.bit.POLARITY = 1;      // Rising edge interrupt
    XIntruptRegs.XINT1CR.bit.ENABLE = 1;        // Enable XINT1

    EDIS;


}

void main(void)
{
    system_setup();
    pie_setup();

    IER = 0x0000;
    IFR = 0x0000;

    gpio_setup();

    for(;;) /* for-ever */
    {
        if(!blink_enable)
        {
            GpioDataRegs.GPASET.bit.GPIO3 = 1;
            GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
            DELAY_US(200000);
            GpioDataRegs.GPASET.bit.GPIO2 = 1;
            GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
            DELAY_US(200000);
        }
//        if (interrupt_counter > 5)
//        {
//            GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1;
//            interrupt_counter = 0;
//        }

//        if(GpioDataRegs.GPADAT.bit.GPIO12 == 1)
//            GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
//        else
//            GpioDataRegs.GPASET.bit.GPIO1 = 1;
    }
}

__interrupt void push_button_isr(void)
{
    interrupt_counter++;

    if(blink_enable == 0)
        blink_enable = 1;
    else
        blink_enable = 0;

    GpioDataRegs.GPATOGGLE.bit.GPIO0 = 1;
    DELAY_US(2000);

//    if(GpioDataRegs.GPADAT.bit.GPIO0 == 0)
//    {
//        GpioDataRegs.GPASET.bit.GPIO0 = 1;
//    }
//    else
//    {
//        GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
//    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
