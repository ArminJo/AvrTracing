# [AvrTracing](https://github.com/ArminJo/AvrTracing)
Available as Arduino library "AvrTracing"

### [Version 1.0.0](https://github.com/ArminJo/AvrTracing/releases)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Installation instructions](https://www.ardu-badge.com/badge/AvrTracing.svg?)](https://www.ardu-badge.com/AvrTracing)
[![Commits since latest](https://img.shields.io/github/commits-since/ArminJo/AvrTracing/latest)](https://github.com/ArminJo/AvrTracing/commits/master)
[![Build Status](https://github.com/ArminJo/AvrTracing/workflows/LibraryBuild/badge.svg)](https://github.com/ArminJo/AvrTracing/actions)
![Hit Counter](https://visitor-badge.laobi.icu/badge?page_id=ArminJo_AvrTracing)

**A small (344 bytes) Arduino library to have real program traces and to find the place where your program hangs.**<br/>
Trace your program by pressing a button **connected at pin 2** or use `startTracing()` and `stopTracing()` to trace selected parts of your code. `startTracing()` sets pin 2 to LOW!<br/>
**Currently only running on ATmega type processors like on the Arduino Uno, Nano, or Leonardo boards.**

# Timing
With tracing enabled and 115200 baud, the **effective CPU frequency is around 2 kHz i.e. 7500 times slower**.<br/>
E.g. delayMicroseconds(1000) is slowed down by the factor of 7500 and lasts 7.5 seconds.<br/>
Interrupt service routines cannot be traced by this library. This results in millis() and micros() are slow, but they tell the real time.
Thus delay() *only* takes 48 times the original value.<br/>

## Disclaimer
I observed, that Wire will hang if traced and no timeout is specified with `Wire.setWireTimeout()`. 
In general, **functions depending on timing may not work or behave strange if traced**.

# Usage
```
#include "AvrTracing.hpp"

setup() {
    Serial.begin(115200);
    initTrace();
    // optional info output
    printTextSectionAddresses();
    printNumberOfPushesForISR();
    startTracing();
    // the code to trace
    ...
    stopTracing();
}

```
### Resulting output
```
Start of text section=0x184 end=0xABE
Found 17 pushes in ISR
...
PC=0x01D0
PC=0x01D2
PC=0x01D4
PC=0x01D6
PC=0x01D8
PC=0x01DA
PC=0x01DC
PC=0x01DE
PC=0x01E0
```
## Trace only part of your program
```
    ...
    startTracing();
    _NOP();
    _NOP(); // Both nop's are not printed, but they allow to see the program counter of the call instructions of digitalWrite().
    digitalWrite(LED_BUILTIN, HIGH); // Takes 24 ms for 27 prints.
    stopTracing(); // the first 2 instructions of stopTracing() are printed at last.
    digitalWrite(LED_BUILTIN, LOW);
    ...
```

# Generating the assembler file
In order to **match PC values to your code**, you require the assembler (*.lss) file to be generated.<br/>
This assembler file can be generated with `avr-objdump --section-headers --source --line-numbers <myfilename>.elf > <myfilename>.lss`.

### Arduino IDE
You have to extend the *platform.txt* file. On my PC it is located at *C:\Program Files\arduino-1.8.16\hardware\arduino\avr* or *C:\Users\\\<Username>\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.3*.<br/>
Insert the following line after the `## Save hex` block.

```
recipe.hooks.objcopy.postobjcopy.1.pattern.windows=cmd /C "{compiler.path}avr-objdump" --disassemble --source --line-numbers --demangle --section=.text "{build.path}/{build.project_name}.elf" > "{build.path}/{build.project_name}.lss"
```
The path of the resulting *.lss assembler file is like C:\Users\\\<Username>\AppData\Local\Temp\arduino_build_\\<number>. The path is printed in the Arduino console if you check *show verbose output during compilations* in the *File/Arduino/Preferences* settings.<br/>
The ATTinyCore board package still generates this assembler file as a `*.lst` file.

### [Sloeber IDE](http://eclipse.baeyens.it/)
Insert `avr-objdump -h -S ${BuildArtifactFileBaseName}.elf  > ${BuildArtifactFileBaseName}.lss` in *Project/Properties/C/C++ Build/Settings/Build Steps/Post-build steps/Command*.<br/>
Examples for such a project configuration can be found [here](https://github.com/ArminJo/Arduino-Lessons-for-School).

### AVR Eclipse Plugin
For the AVR Eclipse Plugin (de.innot.avreclipse.p2repository-2.4.2.zip), check the *Create Extended Listing* option in *Project/Properties/C/C++ Build/Settings/Additional Tools in toolchain*  and insert `-g` in *Project/Properties/C/C++ Build/Settings/Linker/General/Other Arguments*.<br/>

## Hint for assembler creation
Sometimes the assembler output is easier to understand, if you disable the compiler optimization. For this, in the  the *platform.txt* file, change all occurences of `-Os` to `-Og` and remove all occurences of `-flto`. This also increases the code size and therefore might not be applicable for large programs, they may not fit into the program memory any more.

# TraceBasic example
<table>
<thead>
  <tr>
    <th>Program output</th>
    <th>Generated assembler from .lss file</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>START ../src/TraceBasic.cpp from Oct 28 2021<br/>Using library version 1.0.0<br/>Low level on PCI0 (pin2) will print program counter<br/>PC=0x2B4<br/>PC=0x776<br/>PC=0x778<br/>PC=0x77A</td>
    <td><b>Startup code and start of loop</b></td>
  </tr>
  <tr>
    <td>PC=0x1EA<br/>PC=0x1EC<br/>PC=0x1EE<br/>PC=0x1F0<br/>PC=0x1F2</td>
    <td><b>Loop content</b><pre>void loop() { // 11 clock cycles // 687,5 ns
    digitalWriteFast(TEST_OUT_PIN, HIGH); // 2 clock cycles / 125 ns 15 ms with trace
 1ea:   5c 9a           sbi 0x0b, 4 ; 11
    digitalWriteFast(TEST_OUT_PIN, LOW); // 2 clock cycles / 125 ns 15 ms with trace
 1ec:   5c 98           cbi 0x0b, 4 ; 11
    digitalWriteFast(TEST_OUT_PIN, HIGH);
 1ee:   5c 9a           sbi 0x0b, 4 ; 11
    digitalWriteFast(TEST_OUT_PIN, LOW);
 1f0:   5c 98           cbi 0x0b, 4 ; 11
}
 1f2:   08 95           ret</pre></td>
  </tr>
  <tr>
    <td>PC=0x77C<br/>PC=0x77E<br/>PC=0x780</td>
    <td><b>1. part of Arduino internal loop</b><pre>    for (;;) {
        loop();
 77a:   37 dd           rcall   .-1426      ; 0x1ea <loop>
        if (serialEventRun) serialEventRun();
 77c:   20 97           sbiw    r28, 0x00   ; 0
 77e:   e9 f3           breq    .-6         ; 0x77a <main+0xa>
 780:   fc dd           rcall   .-1032      ; 0x37a <_Z14serialEventRunv>
 782:   fb cf           rjmp    .-10        ; 0x77a <main+0xa></pre></td>
  </tr>
  <tr>
   <td>PC=0x37A<br/>PC=0x37C<br/>PC=0x37E<br/>PC=0x380<br/>PC=0x382<br/>PC=0x384<br/>PC=0x386<br/>PC=0x388<br/>PC=0x392</td>
    <td><b>The called serialEventRun() function</b><pre>void serialEventRun(void)
{
#if defined(HAVE_HWSERIAL0)
  if (Serial0_available && serialEvent && Serial0_available()) serialEvent();
 37a:   8e e0           ldi r24, 0x0E   ; 14
 37c:   93 e0           ldi r25, 0x03   ; 3
 37e:   89 2b           or  r24, r25
 380:   41 f0           breq    .+16        ; 0x392 <_Z14serialEventRunv+0x18>
 382:   80 e0           ldi r24, 0x00   ; 0
 384:   90 e0           ldi r25, 0x00   ; 0
 386:   89 2b           or  r24, r25
 388:   21 f0           breq    .+8         ; 0x392 <_Z14serialEventRunv+0x18>
 38a:   48 d1           rcall   .+656       ; 0x61c <_Z17Serial0_availablev>
 38c:   81 11           cpse    r24, r1
 38e:   0c 94 00 00     jmp 0   ; 0x0 <__vectors>
  if (Serial2_available && serialEvent2 && Serial2_available()) serialEvent2();
#endif
#if defined(HAVE_HWSERIAL3)
  if (Serial3_available && serialEvent3 && Serial3_available()) serialEvent3();
#endif
}
 392:   08 95           ret</pre></td>
  </tr>
  <tr>
    <td>PC=0x782<br/>PC=0x77A</td>
    <td><b>2. part of Arduino internal loop</b></td>
  </tr>
  <tr>
    <td>PC=0x1EA<br/>PC=0x1EC<br/>PC=0x1EE</td>
    <td><b>2. run of loop content</b></td>
  </tr>
</tbody>
</table>

# Other tracing methods
Besides of using `Serial.print()` statements, there is an extension of the simple print method, the [ArduinoTrace library](https://github.com/bblanchon/ArduinoTrace). But be aware, that the calls and especially the strings used by this methods require a lot of program memory.

# Program memory size
If `NUMBER_OF_PUSH` is defined (static mode): **284 bytes**<br/>
If `NUMBER_OF_PUSH` is not defined **(recommended dynamic mode): 344 bytes** (60 bytes more than static)

You can first use the dynamic mode without `DEBUG_INIT` defined, and call `printNumberOfPushesForISR()`
to get the right number of pushes and then switch to static mode using this value, to save around 60 bytes of program memory
or to proof, that you have counted the pushes of the ISR correct :-).

# Compile options / macros for this library
If you coomment out the line `#define DEBUG_INIT` you see internal information at the call of `initTrace()`. This costs 52 (static) / 196 (dynamic) bytes of program space.

# Related links
https://github.com/jdolinay/avr_debug
https://hinterm-ziel.de/index.php/2021/07/19/debugging3-debugging-is-like-being-the-detective-in-a-crime-movie-where-you-are-also-the-murderer

#### If you find this library useful, please give it a star.


# Revision History
### Version 1.0.0
Initial Arduino library version.
