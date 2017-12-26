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

void system_setup(void);
void pie_setup(void);
void gpio_setup(void);

__interrupt void push_button_isr(void);

uint16_t interrupt_counter = 0;

void system_setup(void)
{

    /* Disable the watchdog.
     * Bit 6 disables the watchdog.
     * Bit 3 to 5 must be written as 1,0,1, unless a software reset is desired.
     * 0x0060 = 0b01101000*/
    EALLOW;
    SysCtrlRegs.WDCR = 0x0068;
    EDIS;

    /* Activate the internal oscillator, and disable all other clock sources. */
    EALLOW;
    SysCtrlRegs.CLKCTL.bit.INTOSC1OFF = 0;
    SysCtrlRegs.CLKCTL.bit.OSCCLKSRCSEL=0;  // Clk Src = INTOSC1
    SysCtrlRegs.CLKCTL.bit.XCLKINOFF=1;     // Turn off XCLKIN
    SysCtrlRegs.CLKCTL.bit.XTALOSCOFF=1;    // Turn off XTALOSC
    SysCtrlRegs.CLKCTL.bit.INTOSC2OFF=1;    // Turn off INTOSC2
    EDIS;

    /* Calibrate ADC and internal oscillators using calibration data from the factory. */
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;     /* Enable ADC clock. */
    (*Device_cal)();                          /* Call calibration routine in ROM address 0x3D7C80. */
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 0;     /* Disable ADC clock. */
    EDIS;

    /* Configure PLL
     * The internal oscillator is 10MHz.
     * Multiply by 12 and divide by 2 yields 60MHz.
     */
    EALLOW;
    SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;
    SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1; /* Disable missing clock detection. */
    SysCtrlRegs.PLLCR.bit.DIV = DSP28_PLLCR; /* PLL clocking ratio */
    SysCtrlRegs.PLLSTS.bit.DIVSEL = DSP28_DIVSEL; /* Divide by selection for the PLL*/
    EDIS;

    DINT;

    IER = 0x0000;
    IFR = 0x0000;

    /*
     * Initialize peripheral clocks.
     *
     * In order to save power you should only enable the clocks that you need.
     */
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3;  // GPIO18 = XCLKOUT
    SysCtrlRegs.LOSPCP.all = 0x0002;
    SysCtrlRegs.XCLK.bit.XCLKOUTDIV=00; // Set XCLKOUT = SYSCLKOUT/4

//    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;       // ADC
//    SysCtrlRegs.PCLKCR3.bit.COMP1ENCLK = 1;     // COMP1
//    SysCtrlRegs.PCLKCR3.bit.COMP2ENCLK = 1;     // COMP2
//    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer-0
//    SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1; // CPU Timer-1
//    SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1; // CPU Timer-2
//    SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK = 1;     // eCAP1
//    SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;     // EPWM1
//    SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;     // EPWM2
//    SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1;     // EPWM3
//    SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 1;     // EPWM4
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;    // GPIO
//    SysCtrlRegs.PCLKCR0.bit.HRPWMENCLK=1;       // HRPWM
//    SysCtrlRegs.PCLKCR0.bit.I2CAENCLK = 1;      // I2C
//    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;      // SCI-A
//    SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;      // SPI-A

//    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      // Enable TBCLK within the EPWM

    EDIS;
}

void pie_setup(void)
{
    DINT; /* Disable interrupts */

    PieCtrlRegs.PIECTRL.bit.ENPIE = 0; /* Disable the Peripheral interrupt expansion (PIE). */

    /* Disable all PIE groups.
     * PIEIER - Peripheral interrupt expansion Interrupt enable register. */
    PieCtrlRegs.PIEIER1.all = 0;
    PieCtrlRegs.PIEIER2.all = 0;
    PieCtrlRegs.PIEIER3.all = 0;
    PieCtrlRegs.PIEIER4.all = 0;
    PieCtrlRegs.PIEIER5.all = 0;
    PieCtrlRegs.PIEIER6.all = 0;
    PieCtrlRegs.PIEIER7.all = 0;
    PieCtrlRegs.PIEIER8.all = 0;
    PieCtrlRegs.PIEIER9.all = 0;
    PieCtrlRegs.PIEIER10.all = 0;
    PieCtrlRegs.PIEIER11.all = 0;
    PieCtrlRegs.PIEIER12.all = 0;

    /* Clear all the PIE flag registers.
     * PIEIFR - Peripheral interrupt expansion interrupt flag register. */
    PieCtrlRegs.PIEIFR1.all = 0;
    PieCtrlRegs.PIEIFR2.all = 0;
    PieCtrlRegs.PIEIFR3.all = 0;
    PieCtrlRegs.PIEIFR4.all = 0;
    PieCtrlRegs.PIEIFR5.all = 0;
    PieCtrlRegs.PIEIFR6.all = 0;
    PieCtrlRegs.PIEIFR7.all = 0;
    PieCtrlRegs.PIEIFR8.all = 0;
    PieCtrlRegs.PIEIFR9.all = 0;
    PieCtrlRegs.PIEIFR10.all = 0;
    PieCtrlRegs.PIEIFR11.all = 0;
    PieCtrlRegs.PIEIFR12.all = 0;

}

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

    for(;;)
    {
        GpioDataRegs.GPASET.bit.GPIO3 = 1;
        GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
        DELAY_US(600000);
        GpioDataRegs.GPASET.bit.GPIO2 = 1;
        GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
        DELAY_US(600000);

//        if (interrupt_counter > 5)
//        {
//            GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1;
//            interrupt_counter = 0;
//        }

        if(GpioDataRegs.GPADAT.bit.GPIO12 == 1)
            GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
        else
            GpioDataRegs.GPASET.bit.GPIO1 = 1;
    }
}

__interrupt void push_button_isr(void)
{
    interrupt_counter++;

    if(GpioDataRegs.GPADAT.bit.GPIO0 == 0)
    {
        GpioDataRegs.GPASET.bit.GPIO0 = 1;
    }
    else
    {
        GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
