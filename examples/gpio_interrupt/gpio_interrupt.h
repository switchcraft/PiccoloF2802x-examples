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

/** @file gpio_interrupt.h
 *  @brief Function prototypes for the main source file.
 *
 *  This is the main header file.
 *
 *  @author Eirik Haustveit (la2kta)
 *  @bug No known bugs.
 */

#ifndef GPIO_INTERRUPT_H_
#define GPIO_INTERRUPT_H_

void gpio_setup(void);

__interrupt void push_button_isr(void);

uint16_t interrupt_counter = 0;
bool blink_enable = 0;


#endif /* MAIN_H_ */
