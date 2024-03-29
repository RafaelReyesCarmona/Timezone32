<img src="images/icons8-timezone-48.png" width=48 height=48 align=right>

# Timezone32
[![Version: v1.1](https://img.shields.io/badge/Version-v1.1-blue?style=for-the-badge&logo=v)]()

This is an adaption of Arduino Timezone Library

https://github.com/JChristensen/Timezone  -> https://github.com/RafaelReyesCarmona/Time32


## Introduction
The **Timezone32** library is designed to work in conjunction with the [Time32 library](https://github.com/RafaelReyesCarmona/Time32), which must also be installed on your system. This documentation assumes some familiarity with the Time32 library.

The primary aim of the **Timezone32** library is to convert Universal Coordinated Time (UTC) to the correct local time, whether it is daylight saving time (a.k.a. summer time) or standard time. The time source could be a GPS receiver, an NTP server, or a Real-Time Clock (RTC) set to UTC.  But whether a hardware RTC or other time source is even present is immaterial, since the Time32 library can function as a software RTC without additional hardware (although its accuracy is dependent on the accuracy of the microcontroller's system clock.)

The **Timezone32** library implements two objects to facilitate time zone conversions:
- A **TimeChangeRule** object describes when local time changes to daylight (summer) time, or to standard time, for a particular locale.
- A **Timezone** object uses **TimeChangeRule**s to perform conversions and related functions.  It can also write its **TimeChangeRule**s to EEPROM, or read them from EEPROM.  Multiple time zones can be represented by defining multiple **Timezone** objects.

## Examples
The following example sketches are included with the **Timezone32** library:

- **Clock:** A simple self-adjusting clock for a single time zone.  **TimeChangeRule**s may be optionally read from EEPROM.
- **HardwareRTC:** A self-adjusting clock for one time zone using an external real-time clock, either a DS1307 or DS3231 (e.g. Chronodot) which is set to UTC.  
- **WorldClock:** A self-adjusting clock for multiple time zones.
- **WriteRules:** A sketch to write **TimeChangeRule**s to EEPROM.
- **Change_TZ_1:** Changes between time zones by modifying the TimeChangeRules.
- **Change_TZ_2:** Changes between time zones by selecting from an array of Timezone objects.

## Coding TimeChangeRules
Normally these will be coded in pairs for a given time zone: One rule to describe when daylight (summer) time starts, and one to describe when standard time starts.

As an example, here in the Eastern US time zone, Eastern Daylight Time (EDT) starts on the 2nd Sunday in March at 02:00 local time. Eastern Standard Time (EST) starts on the 1st Sunday in November at 02:00 local time.

Define a **TimeChangeRule** as follows:

`TimeChangeRule myRule = {abbrev, week, dow, month, hour, offset};`

Where:

**abbrev** is a character string abbreviation for the time zone; it must be no longer than five characters.

**week** is the week of the month that the rule starts.

**dow** is the day of the week that the rule starts.

**hour** is the hour in local time that the rule starts (0-23).

**offset** is the UTC offset _in minutes_ for the time zone being defined.

For convenience, the following symbolic names can be used:

**week:** First, Second, Third, Fourth, Last  
**dow:** Sun, Mon, Tue, Wed, Thu, Fri, Sat  
**month:** Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec

For the Eastern US time zone, the **TimeChangeRule**s could be defined as follows:

```c++
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //UTC - 5 hours
```

## Coding Timezone objects
There are three ways to define **Timezone** objects.

By first defining **TimeChangeRule**s (as above) and giving the daylight time rule and the standard time rule (assuming usEDT and usEST defined as above):  
`Timezone usEastern(usEDT, usEST);`

For a time zone that does not change to daylight/summer time, pass a single rule to the constructor. For example:  
`Timezone usAZ(usMST, usMST);`

By reading rules previously stored in EEPROM.  This reads both the daylight and standard time rules previously stored at EEPROM address 100:  
`Timezone usPacific(100);`

Note that **TimeChangeRule**s require 12 bytes of storage each, so the pair of rules associated with a Timezone object requires 24 bytes total.  This could possibly change in future versions of the library.  The size of a **TimeChangeRule** can be checked with `sizeof(usEDT)`.

## Timezone library methods
Note that the `time32_t` data type is defined by the Time32 library <TimeLib32.h>. See the Time32 library documentation [here](https://github.com/RafaelReyesCarmona/Time32) for additional details.

### time32_t toLocal(time32_t utc);
##### Description
Converts the given UTC time to local time, standard or daylight as appropriate.
##### Syntax
`myTZ.toLocal(utc);`
##### Parameters
***utc:*** Universal Coordinated Time *(time32_t)*  
##### Returns 
Local time *(time32_t)*  
##### Example
```c++
time32_t eastern, utc;
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //UTC - 5 hours
Timezone usEastern(usEDT, usEST);
utc = now();	//current time from the Time32 Library
eastern = usEastern.toLocal(utc);
```

### time32_t toLocal(time32_t utc, TimeChangeRule **tcr);
##### Description
As above, converts the given UTC time to local time, and also returns a pointer to the **TimeChangeRule** that was applied to do the conversion. This could then be used, for example, to include the time zone abbreviation as part of a time display.  The caller must take care not to alter the pointed **TimeChangeRule**, as this will then result in incorrect conversions.
##### Syntax
`myTZ.toLocal(utc, &tcr);`  
##### Parameters
***utc:*** Universal Coordinated Time *(time32_t)*  
***tcr:*** Address of a pointer to a **TimeChangeRule** _(\*\*TimeChangeRule)_   
##### Returns
Local time *(time32_t)*  
Pointer to **TimeChangeRule**  _(\*\*TimeChangeRule)_    
##### Example
```c++
time32_t eastern, utc;
TimeChangeRule *tcr;
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //UTC - 5 hours
Timezone usEastern(usEDT, usEST);
utc = now();	//current time from the Time32 Library
eastern = usEastern.toLocal(utc, &tcr);
Serial.print("The time zone is: ");
Serial.println(tcr -> abbrev);
```

### bool utcIsDST(time32_t utc);
### bool locIsDST(time32_t local);
##### Description
These functions determine whether a given UTC time or a given local time is within the daylight saving (summer) time interval, and return true or false accordingly.
##### Syntax
`utcIsDST(utc);`  
`locIsDST(local);`  
##### Parameters
***utc:*** Universal Coordinated Time *(time32_t)*  
***local:*** Local Time *(time32_t)*  
##### Returns
true or false *(bool)*
##### Example
`if (usEastern.utcIsDST(utc)) { /*do something*/ }`

### void readRules(int address);
### void writeRules(int address);
##### Description
These functions read or write a **Timezone** object's two **TimeChangeRule**s from or to EEPROM.
##### Syntax
`myTZ.readRules(address);`  
`myTZ.writeRules(address);`  
##### Parameters
***address:*** The beginning EEPROM address to write to or read from *(int)*
##### Returns
None.
##### Example
`usEastern.writeRules(100);  //write rules beginning at EEPROM address 100`

### void setRules(TimeChangeRule dstStart, TimeChangeRule stdStart);
##### Description
This function reads or updates the daylight and standard time rules from RAM. Can be used to change TimeChangeRules dynamically while a sketch runs.
##### Syntax
`myTZ.setRules(dstStart, stdStart);`  
##### Parameters
***dstStart:*** A TimeChangeRule denoting the start of daylight saving (summer) time.  
***stdStart:*** A TimeChangeRule denoting the start of standard time.
##### Returns
None.
##### Example
```c++
TimeChangeRule EDT = {"EDT", Second, Sun, Mar, 2, -240};
TimeChangeRule EST = {"EST", First, Sun, Nov, 2, -300};
Timezone ET(EDT, EST);
...
tz.setRules(EDT, EST);

```
### time32_t toUTC(time32_t local);
##### Description
Converts the given local time to UTC time.

**WARNING:** This function is provided for completeness, but should seldom be needed and should be used sparingly and carefully.

Ambiguous situations occur after the Standard-to-DST and the DST-to-Standard time transitions. When changing to DST, there is one hour of local time that does not exist, since the clock moves forward one hour. Similarly, when changing to standard time, there is one hour of local time that occurs twice since the clock moves back one hour.

This function does not test whether it is passed an erroneous time value during the Local-to-DST transition that does not exist. If passed such a time, an incorrect UTC time value will be returned.

If passed a local time value during the DST-to-Local transition that occurs twice, it will be treated as the earlier time, i.e. the time that occurs before the transition.

Calling this function with local times during a transition interval should be avoided!
##### Syntax
`myTZ.toUTC(local);`
##### Parameters
***local:*** Local Time *(time32_t)*  
##### Returns
UTC *(time32_t)*  

### time32_t WhendstStart(int yr);
### time32_t WhenstdStart(int yr);
##### Description
These functions returns when dst or std start. Give time in local time.
##### Syntax
`WhendstStart(year);`  
`WhenstdStart(year);`  
##### Parameters
***year:*** The beginning EEPROM address to write to or read from *(int)*
##### Returns
***local:*** Local Time *(time32_t)* 
##### Example
`timedst = WhendstStart(2022);  //return time when dst time start (local time) for give year 2022.`
`timestd = WhenstdStart(2022);  //return time when std time start (local time) for give year 2022.`

## Changelog
### V1.1
    * added functions WhendstStart and WhenstdStart.
### V1.0.1
    * Fixed 
### V1.0
    * Initial version adapted to time32_t.

## License for Timezone
Arduino Timezone Library Copyright (C) 2018 Jack Christensen GNU GPL v3.0

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License v3.0 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/gpl.html>

## License ##

This file is part of Timezone32 Library.

Timezone32 Library is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Timezone32 Library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Timezone32 Library.  If not, see <https://www.gnu.org/licenses/>.

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)

## Authors ##
### Timezone32
Copyright © 2019-2022 Francisco Rafael Reyes Carmona.
Contact me: rafael.reyes.carmona@gmail.com

### Timezone
Copyright (C) 2018 Jack Christensen

## Credits ##

<a target="_blank" href="https://icons8.com/icon/7V6GsPO2nMqV/timezone">Timezone</a> icon by <a target="_blank" href="https://icons8.com">Icons8</a>
