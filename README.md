# Smart Medicine Reminder System

An Arduino Uno based Medicine Reminder System that helps users remember their medication schedule using a DS1302 Real-Time Clock (RTC), I2C LCD display, push buttons, EEPROM storage, and an active buzzer.

The system supports up to 3 daily medicine alarms, stores alarm settings permanently in EEPROM, and provides visual and audible notifications when it is time to take medication.

## Features

* Real-time clock using DS1302 RTC module
* Supports 3 configurable medicine alarms
* Alarm settings saved in EEPROM
* Alarm settings persist after power loss
* LCD-based user interface
* Audible buzzer notifications
* Alarm triggers only once per day
* Serial Monitor debugging support
* Button-based navigation and alarm editing
* Low-cost and beginner-friendly hardware design

## Hardware Requirements

| Component            | Quantity  |
| -------------------- | --------- |
| Arduino Uno          | 1         |
| DS1302 RTC Module    | 1         |
| 16x2 I2C LCD Display | 1         |
| Active Buzzer        | 1         |
| Push Buttons         | 4         |
| Breadboard           | 1         |
| Jumper Wires         | As needed |
| CR2032 RTC Battery   | 1         |

## Circuit Connections

### DS1302 RTC

| RTC Pin | Arduino Uno |
| ------- | ----------- |
| VCC     | 5V          |
| GND     | GND         |
| DAT     | D7          |
| CLK     | D6          |
| RST     | D8          |

### LCD (I2C)

| LCD Pin | Arduino Uno |
| ------- | ----------- |
| VCC     | 5V          |
| GND     | GND         |
| SDA     | A4          |
| SCL     | A5          |

### Buzzer

| Buzzer Pin | Arduino Uno |
| ---------- | ----------- |
| +          | D9          |
| -          | GND         |

### Buttons

| Function           | Arduino Pin |
| ------------------ | ----------- |
| Menu               | D2          |
| Hour Increment     | D3          |
| Minute Increment   | D4          |
| Save / Acknowledge | D5          |

Buttons are connected between the Arduino pin and GND using INPUT_PULLUP mode.

## Software Requirements

### Libraries

Install the following libraries through Arduino Library Manager:

* Rtc by Makuna
* LiquidCrystal_I2C
* EEPROM (built-in)

## User Interface

### Screen 0

Displays current RTC time.

### Screen 1

Edit Alarm 1.

### Screen 2

Edit Alarm 2.

### Screen 3

Edit Alarm 3.

### Screen 4

View configured alarms.

## Button Functions

| Button | Function                                         |
| ------ | ------------------------------------------------ |
| Menu   | Switch screens                                   |
| Hour   | Increment alarm hour                             |
| Minute | Increment alarm minute                           |
| Save   | Save alarm to EEPROM or acknowledge active alarm |

## Alarm Operation

1. Configure alarm time.
2. Press Save.
3. Alarm settings are written to EEPROM.
4. When RTC time matches an alarm:

   * LCD displays medicine reminder.
   * Buzzer sounds.
5. Press Save to acknowledge and stop the alarm.
6. Alarm will not trigger again until the next day.

## EEPROM Storage

Alarm settings are stored in EEPROM:

| Address | Purpose        |
| ------- | -------------- |
| 0       | Alarm 1 Hour   |
| 1       | Alarm 1 Minute |
| 2       | Alarm 2 Hour   |
| 3       | Alarm 2 Minute |
| 4       | Alarm 3 Hour   |
| 5       | Alarm 3 Minute |

## First-Time RTC Setup

Uncomment the following lines in setup():

```cpp
RtcDateTime compiled(__DATE__, __TIME__);
Rtc.SetDateTime(compiled);
```

Upload the sketch once.

Then comment the lines again and upload a second time.

This prevents the RTC from resetting every time the Arduino is programmed.

## Future Improvements

* Snooze functionality
* Multiple medicine names
* OLED display support
* Mobile app integration
* SMS reminders using GSM module
* Wi-Fi notifications using ESP8266/ESP32
* Temperature and humidity monitoring for medicine storage

## Author

Created using Arduino Uno, DS1302 RTC, EEPROM, and I2C LCD for educational and healthcare assistance purposes.
