#ifndef lib_h
#define lib_h

#include "Arduino.h"
#include <timer.h>
#include <LiquidCrystal.h>
#include <RCSwitch.h>

enum state {CLOCK, ALARM_SWITCH, ALARM_ON,
            SET_TIME_HOURS, SET_TIME_MINUTES, SET_ALARM_HOURS, SET_ALARM_MINUTES,
            INC_TIME_HOURS, INC_TIME_MINUTES, INC_ALARM_HOURS, INC_ALARM_MINUTES,
            DEC_TIME_HOURS, DEC_TIME_MINUTES, DEC_ALARM_HOURS, DEC_ALARM_MINUTES};
enum button {DECREMENT, INCREMENT, SET_TIME, SET_ALARM, SWITCH_ALARM, NONE};

class AlarmClock
{
  public:
    AlarmClock();
//    AlarmClock(int blinkPeriod);
    void handleButtonPress(enum button btn);
    void setState(enum state newState);
    long getSeconds(long seconds);
    int getHours(long seconds);
    int getMinutes(long seconds);
    String getTimeString(long fullSeconds);
    String getLCDOutput();
    String getLCDSecondOutput();
    bool isAlarmActive();
    void tickTimer();
    void handleAlarm();
    void switchLight(bool state);
    void setTime(int hours, int minutes);
  private:
    long getFullSeconds();
    static bool updateLCD(AlarmClock *alarmClock);
    enum state _state = CLOCK;
    long _millisOffset;
    long _startSeconds;
    int _blinkPeriod;
    bool _alarmState = 0;
    long _alarmSeconds = 0;
    bool _alarmActive = 0;
    bool _lightState = 0;
    int _lightOnCode = 5201;
    int _lightOffCode = 5204;
    int _transmitterPin = 2;
    RCSwitch _transmitter = RCSwitch();
    Timer<> _timer = timer_create_default();
    LiquidCrystal _lcd = LiquidCrystal(7, 8, 9, 10, 11, 12);
};

#endif
