# [AvrTracing](https://github.com/ArminJo/AvrTracing)
Available as Arduino library "AvrTracing"

### [Version 1.0.0](https://github.com/ArminJo/AvrTracing/releases)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Installation instructions](https://www.ardu-badge.com/badge/AvrTracing.svg?)](https://www.ardu-badge.com/AvrTracing)
[![Commits since latest](https://img.shields.io/github/commits-since/ArminJo/AvrTracing/latest)](https://github.com/ArminJo/AvrTracing/commits/master)
[![Build Status](https://github.com/ArminJo/AvrTracing/workflows/LibraryBuild/badge.svg)](https://github.com/ArminJo/AvrTracing/actions)
![Hit Counter](https://visitor-badge.laobi.icu/badge?page_id=ArminJo_AvrTracing)

**A small (344 bytes) Arduino library to have real program traces and to find the place where your program hangs.**<br/>
Trace your program by pressing a button **connected at pin 2** or use `startTracing()` and `stopTracing()` to trace selected parts of your code.<br/>
With tracing enabled and 115200 baud, the **effective CPU frequency is around 1 kHz**.<br/>
Currently only for ATmega type processors like on the Arduino Uno, Nano, or Leonardo boards.

# Usage
```
//#define NUMBER_OF_PUSH 0x11 // Static mode if enabled. Saves 60 bytes program memory.
#include "AvrTracing.cpp.h"

setup() {
    Serial.begin(115200);
    initTrace();
    // optional info output
    printTextSectionAddresses();
    printNumberOfPushesForISR();
    ...
}
```
### Resulting output
```
Start of text section=0x184 end=0xABE
Dynamic found # of pushes in ISR=17
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
    startTracing();
    _NOP()
    ;
    _NOP()
    ; // Both nop's are not printed, but they allow to see the program counter of the call instructions of digitalWrite().
    digitalWrite(LED_BUILTIN, HIGH); // Takes 24 ms for 27 prints.
    stopTracing(); // the first 2 instructions of stopTracing() are printed at last.
    digitalWrite(LED_BUILTIN, LOW);
```

# Generating the assembler file
In order to **match PC values to your code**, you need the assembler (*.lss) file.<br/>
This assembler file can be generated with `avr-objdump --section-headers --source --line-numbers <myfilename>.elf > <myfilename>.lss`.

### Arduino IDE
You have to extend the *platform.txt* file. On my PC it is located at *C:\Users\\\<Username>\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.3*.<br/>
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

# Other tracing methods
Besides of using `Serial.print()` statements, there is an extension of the simple print method, the [ArduinoTrace library](https://github.com/bblanchon/ArduinoTrace). But be aware, that the calls and especially the strings used by this methods require a lot of program memory too.

# Program memory size
If `NUMBER_OF_PUSH` is defined (static mode): 284 bytes + 52 if `DEBUG_INIT` is defined (336 total)<br/>
If `NUMBER_OF_PUSH` is not defined (recommended dynamic mode): **344 bytes** (60 bytes more than static) + 196 if `DEBUG_INIT` is defined (540 total)

You can first use the dynamic mode without `DEBUG_INIT` defined, and call `printNumberOfPushesForISR()`
to get the right number of pushes and then switch to static mode using this value, to save around 60 bytes of program memory
or to proof, that you have counted the pushes of the ISR correct :-).

# Compile options / macros for this library
If you coomment out the line `#define DEBUG_INIT` you see internal information at the call of `initTrace()`. This costs 52 (static) / 196 (dynamic) bytes of program space.

#### If you find this library useful, please give it a star.


# Revision History
### Version 1.0.0
Initial Arduino library version.
