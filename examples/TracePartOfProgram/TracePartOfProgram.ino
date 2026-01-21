/*
 *  TracePartOfProgram.cpp
 *
 *  Traces the call to digitalWrite(LED_BUILTIN,HIGH);
 *
 *  Copyright (C) 2020-2026  Armin Joachimsmeyer
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>

//#define SEARCH_LOWEST_STACKPOINTER_MODE // Instead of printing program addresses of traced section, store lowest SP and corresponding PC
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

//    Serial.println(F("Low level on PCI0 (pin2) will print program counter"));
    Serial.flush(); // guarantee to see the message

    initTrace();
    printNumberOfPushesForISR();
    printTextSectionAddresses();

}

void loop() {
    Serial.println();
    Serial.println(F("Start tracing now"));
    Serial.flush(); // guarantee to see the message

    startTracing(); // This connects Pin 2 to ground
    _NOP()
    ;
    _NOP()
    ; // Both nop's are not printed, but they allow to see the program counter of the call instructions of digitalWrite().
    digitalWrite(LED_BUILTIN, HIGH); // Takes 24 ms for 27 prints.
    stopTracing(); // This releases connection to ground. The first 2 instructions of stopTracing() are printed at last.
    Serial.println(F("Stop tracing"));
    Serial.println();
#if defined(SEARCH_LOWEST_STACKPOINTER_MODE)
    printLowestStackpointerAndProgramAddress();
#endif
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}
