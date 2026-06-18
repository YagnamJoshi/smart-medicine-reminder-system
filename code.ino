#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <EEPROM.h>

// ---------------- RTC ----------------
#define RTC_DAT 7
#define RTC_CLK 6
#define RTC_RST 8

ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST);
RtcDS1302<ThreeWire> Rtc(myWire);

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- BUTTONS ----------------
#define BTN_MENU 2
#define BTN_HOUR 3
#define BTN_MIN 4
#define BTN_SAVE 5

// ---------------- BUZZER ----------------
#define BUZZER 9

// ---------------- ALARMS ----------------
int alarmHour[3];
int alarmMinute[3];

bool alarmDone[3];

int lastDay = -1;

bool alarmActive = false;
int activeAlarm = -1;

// ---------------- UI ----------------
int screen = 0;
int lastScreen = -1;

bool prevMenu = HIGH;
bool prevHour = HIGH;
bool prevMin = HIGH;
bool prevSave = HIGH;

unsigned long lastSerial = 0;

// =====================================

void loadAlarms()
{
    alarmHour[0] = EEPROM.read(0);
    alarmMinute[0] = EEPROM.read(1);

    alarmHour[1] = EEPROM.read(2);
    alarmMinute[1] = EEPROM.read(3);

    alarmHour[2] = EEPROM.read(4);
    alarmMinute[2] = EEPROM.read(5);

    // first boot defaults
    if (alarmHour[0] > 23)
    {
        alarmHour[0] = 8;
        alarmMinute[0] = 0;

        alarmHour[1] = 14;
        alarmMinute[1] = 0;

        alarmHour[2] = 20;
        alarmMinute[2] = 0;
    }
}

void saveAlarm(int idx)
{
    EEPROM.update(idx * 2, alarmHour[idx]);
    EEPROM.update(idx * 2 + 1, alarmMinute[idx]);

    Serial.print("Saved Alarm ");
    Serial.println(idx + 1);
}

// =====================================

void setup()
{
    Serial.begin(9600);

    pinMode(BTN_MENU, INPUT_PULLUP);
    pinMode(BTN_HOUR, INPUT_PULLUP);
    pinMode(BTN_MIN, INPUT_PULLUP);
    pinMode(BTN_SAVE, INPUT_PULLUP);

    pinMode(BUZZER, OUTPUT);

    lcd.init();
    lcd.backlight();

    Rtc.Begin();

    if (Rtc.GetIsWriteProtected())
        Rtc.SetIsWriteProtected(false);

    if (!Rtc.GetIsRunning())
        Rtc.SetIsRunning(true);

    /*
      UNCOMMENT ONLY ONCE

      RtcDateTime compiled(__DATE__, __TIME__);
      Rtc.SetDateTime(compiled);

      Upload.
      Then comment again and re-upload.
    */

    loadAlarms();

    Serial.println("Medicine Reminder Started");
}

// =====================================

void loop()
{
    RtcDateTime now = Rtc.GetDateTime();

    bool menuPressed =
        (digitalRead(BTN_MENU) == LOW && prevMenu == HIGH);

    bool hourPressed =
        (digitalRead(BTN_HOUR) == LOW && prevHour == HIGH);

    bool minPressed =
        (digitalRead(BTN_MIN) == LOW && prevMin == HIGH);

    bool savePressed =
        (digitalRead(BTN_SAVE) == LOW && prevSave == HIGH);

    prevMenu = digitalRead(BTN_MENU);
    prevHour = digitalRead(BTN_HOUR);
    prevMin = digitalRead(BTN_MIN);
    prevSave = digitalRead(BTN_SAVE);

    // -------- New Day --------
    if (now.Day() != lastDay)
    {
        lastDay = now.Day();

        for (int i = 0; i < 3; i++)
            alarmDone[i] = false;

        Serial.println("New Day");
    }

    // -------- Alarm Check --------
    if (!alarmActive)
    {
        for (int i = 0; i < 3; i++)
        {
            if (!alarmDone[i] &&
                now.Hour() == alarmHour[i] &&
                now.Minute() == alarmMinute[i])
            {
                alarmActive = true;
                activeAlarm = i;

                Serial.print("ALARM ");
                Serial.print(i + 1);
                Serial.println(" TRIGGERED");
            }
        }
    }

    // -------- Alarm Screen --------
    if (alarmActive)
    {
        tone(BUZZER, 1000);

        lcd.setCursor(0, 0);
        lcd.print("TAKE MEDICINE ");
        lcd.setCursor(0, 1);
        lcd.print("Alarm ");
        lcd.print(activeAlarm + 1);
        lcd.print(" SAVE");

        if (savePressed)
        {
            alarmDone[activeAlarm] = true;
            alarmActive = false;
            activeAlarm = -1;

            noTone(BUZZER);

            lcd.clear();

            Serial.println("Alarm Acknowledged");
        }

        delay(50);
        return;
    }

    // -------- Menu --------
    if (menuPressed)
    {
        screen++;

        if (screen > 4)
            screen = 0;

        lcd.clear();

        Serial.print("Screen ");
        Serial.println(screen);
    }

    // -------- Draw --------
    if (screen != lastScreen)
    {
        lcd.clear();
        lastScreen = screen;
    }

    switch (screen)
    {
        case 0:
            showTime(now);
            break;

        case 1:
            editAlarm(0, hourPressed, minPressed, savePressed);
            break;

        case 2:
            editAlarm(1, hourPressed, minPressed, savePressed);
            break;

        case 3:
            editAlarm(2, hourPressed, minPressed, savePressed);
            break;

        case 4:
            showAlarms();
            break;
    }

    delay(50);
}

// =====================================

void showTime(RtcDateTime now)
{
    char line[17];

    sprintf(line,
            "%02u:%02u:%02u",
            now.Hour(),
            now.Minute(),
            now.Second());

    lcd.setCursor(0,0);
    lcd.print("Current Time   ");

    lcd.setCursor(0,1);
    lcd.print(line);
    lcd.print("      ");

    if (millis() - lastSerial > 1000)
    {
        Serial.print("TIME ");
        Serial.println(line);

        lastSerial = millis();
    }
}

// =====================================

void editAlarm(
    int idx,
    bool hourPressed,
    bool minPressed,
    bool savePressed)
{
    if (hourPressed)
    {
        alarmHour[idx]++;

        if (alarmHour[idx] > 23)
            alarmHour[idx] = 0;
    }

    if (minPressed)
    {
        alarmMinute[idx]++;

        if (alarmMinute[idx] > 59)
            alarmMinute[idx] = 0;
    }

    if (savePressed)
    {
        saveAlarm(idx);
    }

    lcd.setCursor(0,0);
    lcd.print("Alarm ");
    lcd.print(idx + 1);
    lcd.print("         ");

    char buf[17];

    sprintf(
        buf,
        "%02d:%02d",
        alarmHour[idx],
        alarmMinute[idx]);

    lcd.setCursor(0,1);
    lcd.print(buf);
    lcd.print(" S=Save");
}

// =====================================

void showAlarms()
{
    lcd.setCursor(0,0);

    char top[17];

    sprintf(top,
            "A1 %02d:%02d",
            alarmHour[0],
            alarmMinute[0]);

    lcd.print(top);
    lcd.print("    ");

    char bottom[17];

    sprintf(bottom,
            "A2 %02d A3 %02d",
            alarmHour[1],
            alarmHour[2]);

    lcd.setCursor(0,1);
    lcd.print(bottom);
}
