// Arduino Timezone Library Copyright (C) 2018 by Jack Christensen and
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Arduino Timezone Library example sketch.
// Write TimeChangeRules to EEPROM.
// Jack Christensen Mar 2012
// Rafael Reyes Carmona 15Jul2022

#define USE_UINT64_T 0  // USE_UINT64_T 1 only on ESP32 to fix 2106 problem.

#include <Timezone32.h>   // https://github.com/RafaelReyesCarmona/Timezone32

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEdt = {"EDT", Second, Sun, Mar, 2, -240};    // UTC - 4 hours
TimeChangeRule usEst = {"EST", First, Sun, Nov, 2, -300};     // UTC - 5 hours
Timezone usEastern(usEdt, usEst);

void setup()
{
    pinMode(13, OUTPUT);
    usEastern.writeRules(100);    // write rules to EEPROM address 100
}

void loop()
{
    // fast blink to indicate EEPROM write is complete
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
}

