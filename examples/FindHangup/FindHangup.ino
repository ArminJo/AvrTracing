/*
 *  FindHangup.cpp
 *
 *  This program hangs, if the I2C lines at pin A4 and A5 are held low, e.g. by connecting a 10k ohm resistor to ground.
 *  Otherwise the loop hangs because of a subtle programming error I made too often in my life :-(.
 *
 *  Copyright (C) 2015  Armin Joachimsmeyer
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

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "AvrTracing.cpp.h"

#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C myLCD(0x27, LCD_COLUMNS, LCD_ROWS);

void setup() {
    // initialize the digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)
    delay(2000); // To be able to connect Serial monitor after reset and before first printout
#endif

    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_AVR_TRACING));

    Serial.println(F("Low level on PCI0 (pin2) will print program counter"));
    Serial.flush(); // to see the message

    initTrace();
    printNumberOfPushesForISR();
    printTextSectionAddresses();

    myLCD.init();
    // myLCD.init() hangs, if the I2C lines at pin A4 and A5 are held low, e.g. by connecting a 10k ohm resistor to ground

    Serial.println(F("After myLCD.init()"));

}

void loop() {

    Serial.println(F("Sum up all numbers from 0 to 255"));
    unsigned int tSum = 0;
    for (uint8_t i = 0; i < 0xFF + 1; ++i) {
        tSum += i;
    }
    // do you see this output?
    Serial.print(F("The sum of all numbers from 0 to 255 is: "));
    Serial.println(tSum);

    delay(1000);
}
