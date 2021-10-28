/*
 *  TraceBasic.cpp
 *
 *  Traces the loop and 4 sbi/cbi instructions
 *
 *  Copyright (C) 2020-2021  Armin Joachimsmeyer
 *  Email: armin.joachimsmeyer@gmail.com
 *
 *  More info: https://github.com/ArminJo/AvrTracing
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>

#include "AvrTracing.hpp"
#include "digitalWriteFast.h"

const uint8_t TEST_OUT_PIN = 4;

void setup() {
    // initialize the digital pin as an output.
    pinMode(TEST_OUT_PIN, OUTPUT);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL) || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif

    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_AVR_TRACING));

    Serial.println(F("Low level on PCI0 (pin2) will print program counter"));
    Serial.flush(); // to see the message

    initTrace();
    startTracing();
}

void loop() { // 11 clock cycles // 687,5 ns
    digitalWriteFast(TEST_OUT_PIN, HIGH); // 2 clock cycles / 125 ns 15 ms with trace
    digitalWriteFast(TEST_OUT_PIN, LOW); // 2 clock cycles / 125 ns 15 ms with trace
    digitalWriteFast(TEST_OUT_PIN, HIGH);
    digitalWriteFast(TEST_OUT_PIN, LOW);
}

/*
 * The program output with my comments :-)
 */
//START ../src/TraceBasic.cpp from Oct 28 2021
//Using library version 1.0.0
//Low level on PCI0 (pin2) will print program counter
//PC=0x2B4
//PC=0x776
//PC=0x778
//PC=0x77A
// --- End of setup code, start of loop content
//PC=0x1EA
//PC=0x1EC
//PC=0x1EE
//PC=0x1F0
//PC=0x1F2
// --- End of loop content, start of Arduino internal loop
//PC=0x77C
//PC=0x77E
//PC=0x780
// --- Start of Arduino internal serialEventRun()
//PC=0x37A
//PC=0x37C
//PC=0x37E
//PC=0x380
//PC=0x382
//PC=0x384
//PC=0x386
//PC=0x388
//PC=0x392
// --- End of Arduino internal serialEventRun(), continue of Arduino internal loop
//PC=0x782
//PC=0x77A
// --- End of Arduino internal loop, start of loop content again
//PC=0x1EA
//PC=0x1EC
//PC=0x1EE

/*
 * Parts of the generated assembler file
 */
/*
 * The loop content
 */
//   loop() { // 11 clock cycles // 687,5 ns
//    digitalWriteFast(TEST_OUT_PIN, HIGH); // 2 clock cycles / 125 ns 15 ms with trace
// 1ea:   5c 9a           sbi 0x0b, 4 ; 11
//    digitalWriteFast(TEST_OUT_PIN, LOW); // 2 clock cycles / 125 ns 15 ms with trace
// 1ec:   5c 98           cbi 0x0b, 4 ; 11
//    digitalWriteFast(TEST_OUT_PIN, HIGH);
// 1ee:   5c 9a           sbi 0x0b, 4 ; 11
//    digitalWriteFast(TEST_OUT_PIN, LOW);
// 1f0:   5c 98           cbi 0x0b, 4 ; 11
//}
// 1f2:   08 95           ret

/*
 * The Arduino internal loop
 */
//for (;;) {
//    loop();
//77a:   37 dd           rcall   .-1426      ; 0x1ea <loop>
//    if (serialEventRun) serialEventRun();
//77c:   20 97           sbiw    r28, 0x00   ; 0
//77e:   e9 f3           breq    .-6         ; 0x77a <main+0xa>
//780:   fc dd           rcall   .-1032      ; 0x37a <_Z14serialEventRunv>
//782:   fb cf           rjmp    .-10        ; 0x77a <main+0xa>

/*
 * The called serialEventRun() function
 */
//void serialEventRun(void)
//{
//#if defined(HAVE_HWSERIAL0)
//  if (Serial0_available && serialEvent && Serial0_available()) serialEvent();
// 37a:   8e e0           ldi r24, 0x0E   ; 14
// 37c:   93 e0           ldi r25, 0x03   ; 3
// 37e:   89 2b           or  r24, r25
// 380:   41 f0           breq    .+16        ; 0x392 <_Z14serialEventRunv+0x18>
// 382:   80 e0           ldi r24, 0x00   ; 0
// 384:   90 e0           ldi r25, 0x00   ; 0
// 386:   89 2b           or  r24, r25
// 388:   21 f0           breq    .+8         ; 0x392 <_Z14serialEventRunv+0x18>
// 38a:   48 d1           rcall   .+656       ; 0x61c <_Z17Serial0_availablev>
// 38c:   81 11           cpse    r24, r1
// 38e:   0c 94 00 00     jmp 0   ; 0x0 <__vectors>
//  if (Serial2_available && serialEvent2 && Serial2_available()) serialEvent2();
//#endif
//#if defined(HAVE_HWSERIAL3)
//  if (Serial3_available && serialEvent3 && Serial3_available()) serialEvent3();
//#endif
//}
// 392:   08 95           ret
