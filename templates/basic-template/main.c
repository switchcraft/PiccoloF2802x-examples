/*
 * TMS320_ledBlink - "Hello world" application for the LAUNCHXL-TMS320F28027F
 * Copyright (C) 2017, Switchcraft AS.
 *
 *
 *     This file is part of TMS320_ledBlink.
 *
 *   TMS320_ledBlink is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   TMS320_ledBlink is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with TMS320_ledBlink.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file main.c
 *  @brief Main C source file.
 *
 *  This is the main C source file.
 *
 *  @author Eirik Haustveit (la2kta)
 *  @bug No known bugs.
 */

//#include "DSP28x_Project.h"     // DSP28x Headerfile

#include "f2802x_common/include/DSP28x_Project.h"

#include "f2802x_common/include/clk.h"
#include "f2802x_common/include/gpio.h"
#include "f2802x_common/include/pll.h"
#include "f2802x_common/include/wdog.h"

#include "f2802x_common/include/flash.h"
#include "f2802x_common/include/pie.h"
#include "f2802x_common/include/adc.h"
#include "f2802x_common/include/sci.h"
#include "f2802x_common/include/sci_io.h"

#include "main.h"

void main()
{

    // WARNING: Always ensure you call memcpy before running any functions from RAM
    // InitSysCtrl includes a call to a RAM based function and without a call to
    // memcpy first, the processor will go "into the weeds"

//       #ifdef _FLASH
//        memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
//       #endif

    WDOG_Handle myWDog;
    myWDog = WDOG_init((void *)WDOG_BASE_ADDR, sizeof(WDOG_Obj));
    WDOG_disable(myWDog);


    CLK_Handle myClk;
    PLL_Handle myPll;
    CPU_Handle myCpu;

    myClk = CLK_init((void *)CLK_BASE_ADDR, sizeof(CLK_Obj));
    myPll = PLL_init((void *)PLL_BASE_ADDR, sizeof(PLL_Obj));
    myCpu = CPU_init((void *)NULL, sizeof(CPU_Obj));

    ADC_Handle myAdc;
    FLASH_Handle myFlash;
    PIE_Handle myPie;
    SCI_Handle mySci;

    myFlash = FLASH_init((void *)FLASH_BASE_ADDR, sizeof(FLASH_Obj));
    myPie = PIE_init((void *)PIE_BASE_ADDR, sizeof(PIE_Obj));
    mySci = SCI_init((void *)SCIA_BASE_ADDR, sizeof(SCI_Obj));
    myAdc = ADC_init((void *)ADC_BASE_ADDR, sizeof(ADC_Obj));

    //Select the internal oscillator 1 as the clock source
    CLK_setOscSrc(myClk, CLK_OscSrc_Internal);

    PLL_setup(myPll, PLL_Multiplier_12, PLL_DivideSelect_ClkIn_by_2);

    // Disables the peripheral interrupt expansion (PIE)
    PIE_disable(myPie);
    PIE_disableAllInts(myPie);
    CPU_disableGlobalInts(myCpu);
    CPU_clearIntFlags(myCpu);

    /*
     * We have four blue LED's connected to the GPIO of the controller
     * The following configures the GPIO's as general purpose outputs
     *
     * The LED's are common anode, thus a low output is required for
     * light.
     */
    GPIO_Handle myGPIO;

    myGPIO = GPIO_init((void *)GPIO_BASE_ADDR, sizeof(GPIO_Obj));

    GPIO_setMode(myGPIO, GPIO_Number_0, GPIO_0_Mode_GeneralPurpose);
    GPIO_setDirection(myGPIO, GPIO_Number_0, GPIO_Direction_Output);

    GPIO_setMode(myGPIO, GPIO_Number_1, GPIO_0_Mode_GeneralPurpose);
    GPIO_setDirection(myGPIO, GPIO_Number_1, GPIO_Direction_Output);

    GPIO_setMode(myGPIO, GPIO_Number_2, GPIO_0_Mode_GeneralPurpose);
    GPIO_setDirection(myGPIO, GPIO_Number_2, GPIO_Direction_Output);

    GPIO_setMode(myGPIO, GPIO_Number_3, GPIO_0_Mode_GeneralPurpose);
    GPIO_setDirection(myGPIO, GPIO_Number_3, GPIO_Direction_Output);

    // Turn off all LED's
    GPIO_setHigh(myGPIO, GPIO_Number_0);
    GPIO_setHigh(myGPIO, GPIO_Number_1);
    GPIO_setHigh(myGPIO, GPIO_Number_2);
    GPIO_setHigh(myGPIO, GPIO_Number_3);

    for(;;) // Main loop
    {
        DELAY_US(1000000);
        GPIO_setHigh(myGPIO, GPIO_Number_0);
        GPIO_setHigh(myGPIO, GPIO_Number_2);
        DELAY_US(1000000);
        GPIO_setLow(myGPIO, GPIO_Number_0);
        GPIO_setLow(myGPIO, GPIO_Number_2);
    }
}
