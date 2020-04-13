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
/* pwm_output.ino
 * This is a basic example of library timer1's input capture object, in this case
 * measuring a PWM signal generated with ATmega328P's Timer/Counter2 on pin OC2A.
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

void setup() {
  // put your setup code here, to run once:

  //NOTES: Pin 11 is pin PORTB3 which is pin OC2A where using Timer/Counter2 a PWM
  // signal will be generated.
  DDRB=(1<<(DDB3)); // setting pin OC2A (PORTB3) as output
  
  //initialize the serial port  
  Serial.begin(9600);
  
  // Timer2settings:
  //   - fast pwm with max on 0xff
  //   - prescaler of 32 
  //   - OC2A pwm not inverted output
  //   - OC2B pwm not inverted output
  TCCR2A = (1<<(WGM21)) | (1<<(WGM20)) 
    | (1<<(COM2A1)) 
    | (1<<(COM2B1));
  TCCR2B = //(1<<(CS20));  
  //   (1<<(CS21));
  //    (1<<(CS22))
    (1<<(CS21))
    | (1<<(CS20));
  
  // setting PWM duty
  constexpr uint8_t duty_value=1;//3;//7;//15;//31;//63;//127;
  OCR2A=duty_value; // this is Arduino UNO pin 11
  
  constexpr double pwm_period=256*static_cast<double>(32)/clock_hz;
  constexpr double duty=(duty_value+1)*pwm_period/256;

  my_capture_object.start(prescaler,10);

  Serial.println("============================================");
  Serial.println("======== TIMER1 - INPUT CAPTURE ============");
  Serial.println("========   PWM MEASURING TEST   ============");
  Serial.println("============================================");
  Serial.print("prescaler code: "); Serial.println(prescaler); 
  Serial.print("prescaler value: "); Serial.println(prescaler_values[prescaler]); 
  Serial.print("tick time: "); Serial.print(my_capture_object.get_tick_time(),fractional_digits); Serial.println(" secs.");
  Serial.print("capture window: "); Serial.print(my_capture_object.get_capture_window(),fractional_digits); Serial.println(" secs.");
  Serial.print("generated PWM duty on PIN 11 (OCR2A): "); Serial.print(duty,fractional_digits); Serial.println(" secs.");
  Serial.print("generated PWM period on PIN 11 (OCR2A): "); Serial.print(pwm_period,fractional_digits); Serial.println(" secs.");
  Serial.println("============================================");
  Serial.println("---->   PIN 11 must be connected to    <----");
  Serial.println("---->   PIN 8 for correct behavior!    <----");
  Serial.println("============================================");
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
