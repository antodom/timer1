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
 * File: timer1.h 
 * Description: This is the header file of a library for 
 * abstracting the use of ATmega328P microcontroller's Timer/Counter1.
 * Date: April 11th, 2020
 * Author: Antonio C. Dominguez-Brito <antonio.dominguez@ulpgc.es>
 * ROC-SIANI - Universidad de Las Palmas de Gran Canaria - Spain
 */

#ifndef TIMER1_H
  #define TIMER1_H

  #include <Arduino.h>
  #include <stdint.h>
  #include <avr/interrupt.h>

  namespace arduino_uno
  {
    
    namespace timer1
    {

      constexpr uint32_t clock_hz=16000000;

      enum prescaler_codes: uint8_t
      {
        no_clock,
        prescaler_1,
        prescaler_8,
        prescaler_64,
        prescaler_256,
        prescaler_1024,
        prescalers
      };

      constexpr uint8_t prescaler_bits[prescalers]=
      {
                                              0, // no_clock
                                    (1<<(CS10)), // prescaler_1,
                      (1<<(CS11))              , // prescaler_8,
                      (1<<(CS11)) | (1<<(CS10)), // prescaler_64,
        (1<<(CS12))                            , // prescaler_256
        (1<<(CS12))               | (1<<(CS10))  // prescaler_1024
      };
      
      constexpr uint16_t prescaler_values[prescalers]=
      {
        0, // no_clock,
        1, // prescaler_1,
        8, // prescaler_8,
        64, // prescaler_64,
        256, // prescaler_256,
        1024 // prescaler_1024
      };

      class input_capture
      {
        public:

          using overflow_callback_t=void (*)();
          using input_capture_callback_t=overflow_callback_t;

          static constexpr uint16_t overflow_value=
            (1<<10); // 1024

          static constexpr uint16_t overflow_half_value
            =(overflow_value>>1); // 512

          input_capture() {}

          // disallow copying
          input_capture(const input_capture&) = delete;
          input_capture& operator=(const input_capture&) = delete;
          
          // disallow moving
          input_capture(const input_capture&&) = delete;
          input_capture& operator=(const input_capture&&) = delete;

          void start(
            prescaler_codes prescaler,
            double capture_window  // seconds
              = 0.1 // default value (100 msecs.) 
          )
          {
            if(prescaler==prescaler_codes::no_clock) return;

            tick_time__=
              static_cast<double>(prescaler_values[prescaler])
              /clock_hz;

            capture_window_ticks=
              static_cast<uint64_t>(capture_window/tick_time__);

            // Timer1 Input Capture settings:
            //   - setting fast 10-bit pwm mode, in this mode the
            //     timer overflows when counts 1024 timer ticks
            TCCR1A = (1<<(WGM11)) | (1<<(WGM10));
            TCCR1B = (1<<(WGM12)); // implicitly WGM13 is unset
            
            //   - noise canceller activated,implies a 
            //     4-oscillator-cycle delay)
            TCCR1B |= (1<<(ICNC1));

            //   - prescaler bits
            TCCR1B |= prescaler_bits[prescaler];

            // input capture on rising edge
            TCCR1B |= (1<<(ICES1));

            //   - enabling input capture interrupt
            //   - enabling timer1 overflow interrupt
            TIMSK1 = (1<<(ICIE1)) | (1<<(TOIE1));
          }

          double get_tick_time() { return tick_time__; }
          
          double get_capture_window() 
          { return capture_window_ticks*tick_time__; }

          void get_duty_and_period(
            double& duty, uint64_t& the_duty_ticks,
            double& period, uint64_t& the_period_ticks
          ); 

          void set_overflow_callback(
           overflow_callback_t callback = nullptr 
          ) 
          { 
            if(callback) 
            {
              interrupt_lock__ lock; 
              overflow_callback=callback; 
            }
          }

          void set_input_capture_callback(
           input_capture_callback_t callback = nullptr
          ) 
          { 
            if(callback) 
            { 
              interrupt_lock__ lock; 
              input_capture_callback=callback; 
            }
          }

          void stop()
          {
            // disabl Timer/Counter1's interrupts
            TIMSK1 &= ~((1<<(ICIE1)) | (1<<(TOIE1)));
            
            // stop timer 
            TCCR1B |= ((TCCR1B) & 0b11111000) 
              | prescaler_bits[prescaler_codes::no_clock];
          }

          volatile static uint64_t overflows;

          volatile static uint64_t rising_ticks;
          volatile static uint64_t previous_rising_ticks;
          volatile static uint64_t falling_ticks;

          volatile static uint64_t capture_window_ticks;

          static overflow_callback_t overflow_callback;
          static input_capture_callback_t input_capture_callback;
      
        private:

          struct interrupt_lock__
          {
            interrupt_lock__() { noInterrupts(); }
            
            ~interrupt_lock__() { interrupts(); }
          };

          prescaler_codes prescaler__;

          double tick_time__;

      };

    } // namespace timer1

  } // namespace arduino_uno


#endif // TIMER1_H
