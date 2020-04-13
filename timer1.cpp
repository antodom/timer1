/**
 ** timer1 library
 ** Copyright (C) 2020
 **
 **   Antonio C. Domínguez Brito <antonio.dominguez@ulpgc.es>
 **     División de Robótica y Oceanografía Computacional <www.roc.siani.es>
 **     and Departamento de Informática y Sistemas <www.dis.ulpgc.es>
 **     Universidad de Las Palmas de Gran  Canaria (ULPGC) <www.ulpgc.es>
 **  
 ** This file is part of the timer1 library.
 ** The timer1 library is free software: you can redistribute it and/or modify
 ** it under  the  terms of  the GNU  General  Public  License  as  published  by
 ** the  Free Software Foundation, either  version  3  of  the  License,  or  any
 ** later version.
 ** 
 ** The  timer1 library is distributed in the hope that  it  will  be  useful,
 ** but   WITHOUT   ANY WARRANTY;   without   even   the  implied   warranty   of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE.  See  the  GNU  General
 ** Public License for more details.
 ** 
 ** You should have received a copy  (COPYING file) of  the  GNU  General  Public
 ** License along with the timer1 library.
 ** If not, see: <http://www.gnu.org/licenses/>.
 **/
/*
 * File: timer1.cpp 
 * Description: This is the implementation file of a library for 
 * abstracting the use of ATmega328P microcontroller's Timer/Counter1.
 * Date: April 11th, 2020
 * Author: Antonio C. Dominguez-Brito <antonio.dominguez@ulpgc.es>
 * ROC-SIANI - Universidad de Las Palmas de Gran Canaria - Spain
 */

#define TIMER1_CPP

#include "timer1.h"

using namespace arduino_uno::timer1;

volatile uint64_t input_capture::overflows;

volatile uint64_t input_capture::rising_ticks=0;
volatile uint64_t input_capture::previous_rising_ticks=0;
volatile uint64_t input_capture::falling_ticks=0;

volatile uint64_t input_capture::capture_window_ticks=0;

input_capture::overflow_callback_t 
  input_capture::overflow_callback=nullptr;
input_capture::input_capture_callback_t 
  input_capture::input_capture_callback=nullptr;

// NOTE: Timer1 interrupt handler which is called each time the timer
// counter overflows
ISR(TIMER1_OVF_vect)
{
  // each time the counter overflows we add the overflow value to 
  // static data member overflows, that is we increment the 11th bit
  // so the 10 least significant bits are kept untouched 
  input_capture::overflows+=input_capture::overflow_value;

  // calling overflow callback
  if(input_capture::overflow_callback) 
    input_capture::overflow_callback();
}

// NOTE: Timer1 interrupt handler for Timer/Counter1's input capture
// event
ISR(TIMER1_CAPT_vect)
{
  // IMPORTANT: according to the datasheet (16.6.3) the ICF1 flag 
  // must be cleared each time the input capture flag ICES1 is changed.
  // And Changing the edge sensing must be done as early as possible 
  // after the ICR1 register has been read.  
  bool rising_edge=(TCCR1B & (1<<(ICES1))); // rising edge?
  uint16_t icr1_register=ICR1; // reading ICR1 register
  TCCR1B ^= (1<<(ICES1)); // complement input capture edge flag
  TIFR1 |= (1<<(ICF1)); // clearing ICF1 interrupt flag 

  uint64_t timestamp=input_capture::overflows+icr1_register;

  // NOTE: when overflow and input capture interrupts happends nearly
  // simultaneously, we have to take into account that input capture 
  // interrupts have a greater priority, the following condition takes
  // that into account to contabilize correctly the timestamp as an 
  // overflow can be omitted.
  if(
    (TIFR1 & (1<<TOV1)) // is timer1 overflow flag set? 
    && // and did the capture take place after the overflow?
    (icr1_register<input_capture::overflow_half_value) 
  ) timestamp+=input_capture::overflow_value;

  if(rising_edge) // rising edge?
  {
    input_capture::previous_rising_ticks=input_capture::rising_ticks;
    input_capture::rising_ticks=timestamp;
  }
  else // falling edge?
    input_capture::falling_ticks=timestamp;

  // calling input capture callback
  if(input_capture::input_capture_callback) 
    input_capture::input_capture_callback();
}

void input_capture::get_duty_and_period(
  double& duty, uint64_t& the_duty_ticks,
  double& period, uint64_t& the_period_ticks
) 
{
  uint64_t overs, rising, previous, falling;
  uint16_t tcnt1_register;
  { 
    interrupt_lock__ lock; 

    overs=overflows; 
    rising=rising_ticks;
    previous=previous_rising_ticks;
    falling=falling_ticks;
    tcnt1_register=TCNT1;
  }

  if(
    (
      overs+tcnt1_register-rising
    )>capture_window_ticks
  ) 
  { the_duty_ticks=the_period_ticks=0; }
  else
  {
    the_duty_ticks=
    (falling>=rising)?  falling-rising: falling-previous;
    the_period_ticks=rising-previous;
  }

  duty=(the_duty_ticks)? 
    the_duty_ticks*tick_time__: 0;
  period=(the_period_ticks)? 
    the_period_ticks*tick_time__: 0;
}
