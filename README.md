## timer1 v1.0

This is *timer1* library for the Arduino UNO electronic prototyping platform. 

Copyright (C) 2020 Antonio C. Domínguez Brito (<antonio.dominguez@ulpgc.es>). División de Robótica y Oceanografía Computacional (<http://www.roc.siani.es>) and Departamento de Informática y Sistemas (<http://www.dis.ulpgc.es>). Universidad de Las Palmas de Gran  Canaria (ULPGC) (<http://www.ulpgc.es>).
  
### 0. License 

The *timer1* C++ library is an open source project which is openly available under the GNU General Public License (GPL) license.

### 1. Introduction

This is a library which abstracts the use of Arduino UNO's ATmega328P's Timer/Counter1. In its current version it only abstracts the use of its input capture unit for measuring period and duty of digital signals.

### 2. input_capture object

To use *timer1*'s input_capture object you have just to declare it using a C++ template:

```
#include "timer1.h"
using namespace arduino_uno::timer1;
... 
input_capture my_capture_object;
```
For using the input_capture object it is just necessary to use its member function *start()*:

```
my_capture_object.start(prescaler_codes::prescaler_8,10);
```
The first parameter of this function is the prescaler using for configuring the timer according to ATmega328P's datasheet, and the second parameter is the capture window in seconds. This second parameter specifies the maximum time the input_capture object will be waiting for determining the period of the signal, if during this window time nothing is detected, the period and duty will be reset to zero starting a new capture window. This time should be chosen much larger than the period of the measured signal, at least twice this period.

### 3. Download & installation

The  library  is  available  through  an  open	git  repository  available   at:

* <https://github.com/antodom/timer1>

For using it you just have to copy the library on the libraries folder used by your Arduino IDE, the folder should be named *timer1*.

In addition you must add the flag *-std=gnu++11* for compiling. For doing that add *-std=gnu++11* to the *platform.txt* file, concretely to *compiler.cpp.flags*. In Arduino IDE 1.6.6 and greater versions this flag is already set.

### 4. Examples

On directory *examples* you have available three examples, namely: *basic_test.ino* and *pwm_measuring_test.ino*, who illustrate respectively the use of the input_capture object.

### 5. Version changes

#### 5.1 v1.0

This is the first version of the library.

### 6. Feedback & Suggestions

Please be free to send any comment, doubt of use, or suggestion in relation to *timer1* to <antonio.dominguez@ulpgc.es>.
