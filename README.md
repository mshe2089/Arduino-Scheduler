# Arduino-Scheduler

### What's this?
A physical scheduler device built on an arduino.

### Description
Formatted schedules can be loaded into an SD card.<br />
The scheduler will parse the schedule and display relevant information on a liquid crystal display.<br />
When the time comes for a new event to initiate the arduino will raise an alarm, in the form of a flashing LED, <br />
which needs to be disarmed via button as indication of acknowledgement.<br />

### Contents
As of now, only the arduino sketch, written in C.
Uses Petre Rodan's fantastic DS3231 library. https://github.com/rodan/ds3231

Schematic coming
