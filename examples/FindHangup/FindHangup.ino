/*
 *  FindHangup.cpp
 *
 *  The loop hangs because of a subtle programming error I made too often in my life :-(.
 *
 *  Copyright (C) 2020-2021  Armin Joachimsmeyer
 *  Email: armin.joachimsmeyer@gmail.com
 *
 *  This file is part of AvrTracing https://github.com/ArminJo/AvrTracing.
 *
 *  Arduino-Utils is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>

#include "AvrTracing.hpp"

void setup() {
    // initialize the digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);

#if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/ \
    || defined(SERIALUSB_PID)  || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_attiny3217)
    // Wait until Serial Monitor is attached.
    // Required for boards using USB code for Serial like Leonardo.
    // Is void for USB Serial implementations using external chips e.g. a CH340.
    while (!Serial)
        ;
    // !!! Program will not proceed if no Serial Monitor is attached !!!
#endif

    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_AVR_TRACING));

    Serial.println(F("Connecting PCI0 (pin2) with ground will print trace information"));
    Serial.flush(); // guarantee to see the message

    initTrace();
    printNumberOfPushesForISR();
    printTextSectionAddresses();

    Serial.println(F("Sum up all numbers from 0 to 255"));
    Serial.flush(); // guarantee to see the message
    unsigned int tSum = 0;
//    startTracing(); // activate this, to see the forever loop here
    for (uint8_t i = 0; i < 0xFF + 1; ++i) {
        tSum += i;
    }
    stopTracing();
    /*
     * Do you really see this output?
     * Connect PCI0 (pin2) with ground or activate startTracing() above and look where the program hangs / loops forever
     */
    Serial.print(F("The sum of all numbers from 0 to 255 is: "));
    Serial.println(tSum);

    Serial.println(F("End of setup()"));

}

void loop() {
    delay(1000);
}

/*
 * The program output with default optimization -Os:
 */
//START ../src/FindHangup.cpp from Oct 29 2021
//Using library version 1.0.0
//Low level on PCI0 (pin2) will print program counter
//PC=0x376
//Found 17 pushes in ISR
//Start of text section=0x190 end=0x10DC
//Sum up all numbers from 0 to 255
//PC=0x434
//PC=0x434
//PC=0x434

/*
 * The generated assembler with optimization -Os
 * You see what the compiler has optimized?
 * Since it recognized that the condition in the for loop is always true, it skipped the tSum += i
 * The sbi, cbi are the optimized startTracing.
 */
//Serial.println(F("Sum up all numbers from 0 to 255"));
// 422:   70 e0           ldi r23, 0x00   ; 0
// 424:   8b e8           ldi r24, 0x8B   ; 139
// 426:   91 e0           ldi r25, 0x01   ; 1
// 428:   f0 d3           rcall   .+2016      ; 0xc0a <_ZN5Print7printlnEPK19__FlashStringHelper>
// 42a:   8b e8           ldi r24, 0x8B   ; 139
// 42c:   91 e0           ldi r25, 0x01   ; 1
//Serial.flush(); // guarantee to see the message
// 42e:   2b d2           rcall   .+1110      ; 0x886 <_ZN14HardwareSerial5flushEv>
// 430:   52 9a           sbi 0x0a, 2 ; 10
// 432:   5a 98           cbi 0x0b, 2 ; 11
// 434:   ff cf           rjmp    .-2         ; 0x434 <__LOCK_REGION_LENGTH__+0x34>

/*
 * The program output with no optimization -O0:
 */
//Sum up all numbers from 0 to 255
//PC=0x1704
//PC=0x1706
//PC=0x19F2
//PC=0x19F4
//PC=0x19F6
//PC=0x19F8
//PC=0x19FA
//PC=0x19FC
//PC=0x19FE
//PC=0x1A00
//PC=0x1A02
//PC=0x1A04
//PC=0x1A06
//PC=0x1A08
//PC=0x1A0A
//PC=0x1A0C
//PC=0x19F4 // and again
//PC=0x19F6
//PC=0x19F8

/*
 * The generated assembler with disabled optimization by using -O0
 * Here you also see no break condition for the loop, but the sum is computed and stored.
 */
//Serial.println(F("Sum up all numbers from 0 to 255"));
//19e0:   80 e4           ldi r24, 0x40   ; 64
//19e2:   92 e0           ldi r25, 0x02   ; 2
//19e4:   10 d6           rcall   .+3104      ; 0x2606 <_ZN5Print7printlnEPK19__FlashStringHelper>
//19e6:   80 e4           ldi r24, 0x40   ; 64
//19e8:   92 e0           ldi r25, 0x02   ; 2
//19ea:   44 d4           rcall   .+2184      ; 0x2274 <_ZN14HardwareSerial5flushEv>
//19ec:   1a 82           std Y+2, r1 ; 0x02
//Serial.flush(); // guarantee to see the message
//19ee:   19 82           std Y+1, r1 ; 0x01
//19f0:   72 de           rcall   .-796       ; 0x16d6 <_Z12startTracingv>
//19f2:   1b 82           std Y+3, r1 ; 0x03
//19f4:   8b 81           ldd r24, Y+3    ; 0x03
//unsigned int tSum = 0;
//19f6:   88 2f           mov r24, r24
//19f8:   90 e0           ldi r25, 0x00   ; 0
//startTracing(); // activate this, to see the forever loop here
//19fa:   29 81           ldd r18, Y+1    ; 0x01
//19fc:   3a 81           ldd r19, Y+2    ; 0x02
//for (uint8_t i = 0; i < 0xFF + 1; ++i) {
//19fe:   82 0f           add r24, r18
//    tSum += i;
//1a00:   93 1f           adc r25, r19
//1a02:   9a 83           std Y+2, r25    ; 0x02
//1a04:   89 83           std Y+1, r24    ; 0x01
//1a06:   8b 81           ldd r24, Y+3    ; 0x03
//1a08:   8f 5f           subi    r24, 0xFF   ; 255
//1a0a:   8b 83           std Y+3, r24    ; 0x03
//1a0c:   f3 cf           rjmp    .-26        ; 0x19f4 <setup+0x60>

/*
 * The generated assembler with disabled optimization by using -O0 and a modified for loop "for (uint8_t i = 0; i < 0xFF + 1; ++i)"
 * Here you see the break condition (breq .+26) for the loop!
 */
//for (uint8_t i = 0; i < 0xFF ; ++i) {
//19f6:   8f 3f           cpi r24, 0xFF   ; 255
//19f8:   69 f0           breq    .+26        ; 0x1a14 <setup+0x80>
//19fa:   8b 81           ldd r24, Y+3    ; 0x03
