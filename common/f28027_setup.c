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

/** @file f28027_setup.c
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

#include "f28027_setup.h"

/**
*This function disables the watchdog, and initializes the clock and PLL.
*@author Eirik Haustveit
*@param none
*@date 26/12/2017
*/
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

    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;       // ADC
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

/**
*This function initializes the peripheral interrupt expansion (PIE).
*@author Eirik Haustveit
*@param none
*@date 26/12/2017
*/
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


void adc_setup(void)
{
    EALLOW;
    AdcRegs.ADCCTL1.bit.ADCBGPWD  = 1;      // Power ADC BG
    AdcRegs.ADCCTL1.bit.ADCREFPWD = 1;      // Power reference
    AdcRegs.ADCCTL1.bit.ADCPWDN   = 1;      // Power ADC
    AdcRegs.ADCCTL1.bit.ADCENABLE = 1;      // Enable ADC
    AdcRegs.ADCCTL1.bit.ADCREFSEL = 0;      // Select internal BG
    EDIS;

    DELAY_US(ADC_usDELAY);         // Delay before converting ADC channels

    EALLOW;
    /* The ADCCTL registers contains various general
     * configuration options for the ADC.
     */
    AdcRegs.ADCCTL1.bit.TEMPCONV = 1; /* Connect the internal temperature sensor to ADC input 5. */
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1; /* Interrupt pulse is generated one clock cycle prior to the ADC result latching into result register.*/

    AdcRegs.INTSEL1N2.bit.INT1E = 1;
    AdcRegs.INTSEL1N2.bit.INT1CONT = 0;
    AdcRegs.INTSEL1N2.bit.INT1SEL = 0; /* Select "End of conversion 0" as the interrupt trigger. */
    AdcRegs.INTSEL1N2.bit.INT2E = 1;
    AdcRegs.INTSEL1N2.bit.INT2CONT = 0;
    AdcRegs.INTSEL1N2.bit.INT2SEL = 1; /* Select "End of conversion 1" as the interrupt trigger. */

    AdcRegs.ADCSOC0CTL.bit.CHSEL = 5; /* Select channel 5 for ADC SOC 0 */
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 0; /* Select software as the trigger source.  */
    AdcRegs.ADCSOC0CTL.bit.ACQPS = 0x24; /* Set the sample and hold window to 36+1 clock cycles */

    AdcRegs.ADCSOC1CTL.bit.CHSEL = 0; /* ADC channel 0, available on J5 pin 6. */
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL = 0;
    AdcRegs.ADCSOC1CTL.bit.ACQPS = 0x24;

    EDIS;
}

void sci_setup(void)
{
    /*
     * Configure the SCI communication control register.
     */
    SciaRegs.SCICCR.bit.STOPBITS = 1;
    SciaRegs.SCICCR.bit.LOOPBKENA = 0;
    SciaRegs.SCICCR.bit.PARITY = 0;
    SciaRegs.SCICCR.bit.PARITYENA = 0;
    SciaRegs.SCICCR.bit.LOOPBKENA = 0;
    SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0;
    SciaRegs.SCICCR.bit.SCICHAR = 7;

    SciaRegs.SCICTL1.bit.RXENA = 1;
    SciaRegs.SCICTL1.bit.TXENA = 1;
    SciaRegs.SCICTL1.bit.SLEEP = 0;
    SciaRegs.SCICTL1.bit.TXWAKE = 0;
    SciaRegs.SCICTL1.bit.RXERRINTENA = 0;
    SciaRegs.SCICTL1.bit.SWRESET = 0;

    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

    SciaRegs.SCIHBAUD = 0x0000;
    SciaRegs.SCILBAUD = 0x00C2;

    SciaRegs.SCICTL1.bit.SWRESET = 1;
}

void timer_setup(void)
{

}
