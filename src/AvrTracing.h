/*
 *  AvrTracing.h
 *
 *  Attach button at pin 2 and press it if you want to see actual program counter.
 *  OR use startTracingSignal() and stopTracingSignal().
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
#ifndef AVR_TRACING_H_
#define AVR_TRACING_H_

#include <Arduino.h>

extern char __init;
extern char _etext;

void initTrace();
void enableINT0InterruptOnLowLevel();
void startTracing();
void stopTracing();

/*
 * Info functions
 */
void printNumberOfPushesForISR();
void printTextSectionAddresses();

/*
 * Internal functions for foolproof sending
 */
void sendUSARTForTrace(char aChar);
void sendUnsignedByteHex(uint8_t aByte);
void sendUnsignedIntegerHex(uint16_t aInteger);
void sendLineFeed();
void sendStringForTrace(const char *aStringPtr);
void sendPCHex(uint16_t aPC);
void sendHex(uint16_t aInteger, char aName);
void sendHexNoInterrupts(uint16_t aInteger, char aName);
char nibbleToHex(uint8_t aByte);

#endif // AVR_TRACING_H_

#pragma once
