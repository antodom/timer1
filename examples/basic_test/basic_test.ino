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
/* basic_test.ino
 * This is a basic example of library timer1's input capture object.
 * author: Antonio C. Domínguez Brito <antonio.dominguez@ulpgc.es>
 * 
 * Description: this program measures the period and period of a digital signal,
 * for example, a digital clock, or a PWM signal. It uses the input capture 
 * facility provided by ATmega328P's Timer/Counter1.
 */

#include "timer1.h"

using namespace arduino_uno::timer1;
 
constexpr uint32_t pseudo_period_ms=1000;
constexpr size_t fractional_digits=12;
constexpr prescaler_codes prescaler=prescaler_codes::prescaler_1;

input_capture my_capture_object;

// function prototypes
void print_uint64_t(const uint64_t& value);

// the setup routine runs once when you press reset:
void setup() {       
  
  //initialize the serial port  
  Serial.begin(9600);

  // NOTES: PIN 13 (ATmega328P's pin PB5) sets as ouput
  // In this pin a digital signal will be generated for measuring
  // its duty and period using timer1's input capture object.
  // Take into account that Timer/Counter1's input capture pin is
  // PIN 8 (ATmega328P's pin PB0), so PIN 13 output should be con-
  // nected to PIN 8 for correct behavior
  DDRB |= (1<<DDB5);

  my_capture_object.set_overflow_callback(
    []() { 
      static auto last_time=millis();

      if(millis()-last_time>(pseudo_period_ms>>1))
      {
        // togge PIN 13
        PINB = (1<<PINB5); 
        last_time=millis();
      }
    }
  );

  my_capture_object.start(prescaler,10);

  Serial.println("============================================");
  Serial.println("======== TIMER1 - INPUT CAPTURE ============");
  Serial.println("=============== BASIC TEST =================");
  Serial.println("============================================");
  Serial.print("prescaler code: "); Serial.println(prescaler); 
  Serial.print("prescaler value: "); Serial.println(prescaler_values[prescaler]); 
  Serial.print("tick time: "); Serial.print(my_capture_object.get_tick_time(),fractional_digits); Serial.println(" secs");
  Serial.print("generated duty on PIN 13: "); Serial.print(pseudo_period_ms>>1); Serial.println(" ms.");
  Serial.print("generated period on PIN 13: "); Serial.print(pseudo_period_ms); Serial.println(" ms.");
  Serial.println("============================================");
  Serial.println("---->   PIN 13 must be connected to    <----");
  Serial.println("---->   PIN 8 for correct behavior!    <----");
  Serial.println("============================================");

  interrupts();
}

// the loop routine runs over and over again forever:
void loop() 
{
  uint64_t duty_ticks,period_ticks;
  double duty,period; 
  my_capture_object.get_duty_and_period(
    duty,duty_ticks,
    period,period_ticks
  );

  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

  Serial.print("["); Serial.print(millis()); Serial.print("] ");
  Serial.print("----> measured duty: "); Serial.print(duty,fractional_digits); Serial.print(" secs. - ");
  print_uint64_t(duty_ticks); Serial.println(" ticks.");

  Serial.print("["); Serial.print(millis()); Serial.print("] ");
  Serial.print("----> measured period: "); Serial.print(period,fractional_digits); Serial.print(" secs. - ");
  print_uint64_t(period_ticks); Serial.println(" ticks.");

  Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");

  delay(pseudo_period_ms);
}  

void print_uint64_t(const uint64_t& value)
{
  constexpr static size_t uint64_t_max_length=64;
  static char uint64_t_str__[uint64_t_max_length];

  uint64_t q=value; size_t digit=0;
  char* ptr=&(uint64_t_str__[uint64_t_max_length-1]); 
  *(ptr--)=0;
  while(
    (digit<uint64_t_max_length-1) && (q>0)
  )
  { *(ptr--)='0'+q%10; q=q/10; digit++; }

  Serial.print(ptr+1);   
}
