/*
 *  TraceBasic.cpp
 *
 *  Traces the loop and 4 sbi/cbi instructions
 *
 *  Copyright (C) 2020  Armin Joachimsmeyer
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>

#include "AvrTracing.cpp.h"
#include "digitalWriteFast.h"

const uint8_t TEST_OUT_PIN = 4;

void setup() {
    // initialize the digital pin as an output.
    pinMode(TEST_OUT_PIN, OUTPUT);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)
    delay(2000); // To be able to connect Serial monitor after reset and before first printout
#endif

    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_AVR_TRACING));

//    Serial.println(F("Low level on PCI0 (pin2) will print program counter"));

    initTrace();
    startTracing();
}

void loop() { // 11 clock cycles // 687,5 ns
    digitalWriteFast(TEST_OUT_PIN, HIGH); // 2 clock cycles / 125 ns 15 ms with trace
    digitalWriteFast(TEST_OUT_PIN, LOW); // 2 clock cycles / 125 ns 15 ms with trace
    digitalWriteFast(TEST_OUT_PIN, HIGH);
    digitalWriteFast(TEST_OUT_PIN, LOW);
}
