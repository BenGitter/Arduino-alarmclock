#include "Arduino.h"
#include "AlarmClock.h"

#define SECONDS_IN_DAY 86400
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_MINUTE 60
#define HOURS_IN_DAY 24

AlarmClock::AlarmClock()
{
    _startSeconds = (long)3600 * 0; //(long)3600*45;
    _alarmSeconds = 5;
    _alarmState = 0;
    _millisOffset = 0;
    _blinkPeriod = 1000;

    // Setup LCD
    _lcd.begin(16, 2);
    _lcd.setCursor(5, 0);
    _lcd.print("00:00");

    _timer.every(100, updateLCD, this);

    // LED
    pinMode(LED_BUILTIN, OUTPUT);

    // Transmitter
    _transmitter.enableTransmit(_transmitterPin);
    _transmitter.setPulseLength(326);
    _transmitter.setRepeatTransmit(20);

    // Get time
    //  getInternetTime();
}

void AlarmClock::setTime(int hours, int minutes)
{
    Serial.print("Server Time: ");
    Serial.print(hours);
    Serial.print(':');
    Serial.println(minutes);

    _startSeconds = (long)hours * SECONDS_IN_HOUR + (long)minutes * SECONDS_IN_MINUTE;
    _millisOffset = millis();
}

void AlarmClock::tickTimer()
{
    _timer.tick();
}

static bool AlarmClock::updateLCD(AlarmClock *alarmClock)
{
    alarmClock->_lcd.setCursor(5, 0);
    alarmClock->_lcd.print(alarmClock->getLCDOutput());

    alarmClock->_lcd.setCursor(0, 1);
    alarmClock->_lcd.print(alarmClock->getLCDSecondOutput());

    alarmClock->handleAlarm();

    return 1;
}

void AlarmClock::handleAlarm()
{
    bool active = isAlarmActive();

    // Debugging Serial print
    if (active)
        Serial.println("ALARM!!");

    if (active && !_lightState)
    {
        Serial.println("SWITCH LIGHT ON");
        _lightState = 1;
        switchLight(1);
    }

    if (!active && _lightState)
    {
        Serial.println("SWITCH LIGHT OFF");
        _lightState = 0;
        switchLight(0);
    }

    digitalWrite(LED_BUILTIN, active);
}

void AlarmClock::switchLight(bool state)
{
    int code;
    if (state)
        code = _lightOnCode;
    else
        code = _lightOffCode;

    Serial.println(state);
    Serial.println(code);

    _transmitter.send(code, 24);
}

//AlarmClock::AlarmClock(int blinkPeriod)
//{
//  _startSeconds = 0; //(long)3600*45;
//  _millisOffset = 0;
//  _blinkPeriod = blinkPeriod;
//}

void AlarmClock::handleButtonPress(enum button btn)
{
    if (_state == CLOCK)
    {
        if (btn == SET_TIME)
            setState(SET_TIME_HOURS);
        else if (btn == SET_ALARM)
            setState(SET_ALARM_HOURS);
        else if (btn == SWITCH_ALARM)
            setState(ALARM_SWITCH);
    }
    else if (_state == SET_TIME_HOURS || _state == INC_TIME_HOURS || _state == DEC_TIME_HOURS)
    {
        if (btn == INCREMENT)
            setState(INC_TIME_HOURS);
        else if (btn == DECREMENT)
            setState(DEC_TIME_HOURS);
        else if (btn == SET_TIME)
            setState(SET_TIME_MINUTES);
    }
    else if (_state == SET_TIME_MINUTES || _state == INC_TIME_MINUTES || _state == DEC_TIME_MINUTES)
    {
        if (btn == INCREMENT)
            setState(INC_TIME_MINUTES);
        else if (btn == DECREMENT)
            setState(DEC_TIME_MINUTES);
        else if (btn == SET_TIME)
            setState(CLOCK);
    }
    else if (_state == SET_ALARM_HOURS || _state == INC_ALARM_HOURS || _state == DEC_ALARM_HOURS)
    {
        if (btn == INCREMENT)
            setState(INC_ALARM_HOURS);
        else if (btn == DECREMENT)
            setState(DEC_ALARM_HOURS);
        else if (btn == SET_ALARM)
            setState(SET_ALARM_MINUTES);
    }
    else if (_state == SET_ALARM_MINUTES || _state == INC_ALARM_MINUTES || _state == DEC_ALARM_MINUTES)
    {
        if (btn == INCREMENT)
            setState(INC_ALARM_MINUTES);
        else if (btn == DECREMENT)
            setState(DEC_ALARM_MINUTES);
        else if (btn == SET_ALARM)
            setState(ALARM_ON);
    }
}

long AlarmClock::getFullSeconds()
{
    long seconds = floor((millis() - _millisOffset) / 1000) + _startSeconds;

    if (seconds < 0)
        return (seconds % SECONDS_IN_DAY) + SECONDS_IN_DAY;
    else
        return seconds % SECONDS_IN_DAY;
}

int AlarmClock::getHours(long seconds)
{
    return (int)floor(seconds / SECONDS_IN_HOUR) % HOURS_IN_DAY;
}

int AlarmClock::getMinutes(long seconds)
{
    return floor((seconds % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE);
}

long AlarmClock::getSeconds(long seconds)
{
    return seconds % SECONDS_IN_MINUTE;
}

void AlarmClock::setState(enum state newState)
{
    _state = newState;
    Serial.print("State: ");
    Serial.println(_state);

    switch (newState)
    {
    case CLOCK:
        // Do nothing
        break;
    case ALARM_SWITCH:
        _alarmState = !_alarmState;
        _alarmActive = 0;
        setState(CLOCK);
        break;
    case ALARM_ON:
        _alarmState = 1;
        setState(CLOCK);
        break;
    case SET_TIME_HOURS:
        // Do nothing
        break;
    case SET_TIME_MINUTES:
        // Do nothing
        break;
    case SET_ALARM_HOURS:
        // Do nothing
        break;
    case SET_ALARM_MINUTES:
        // Do nothing
        break;
    case INC_TIME_HOURS:
        _startSeconds += SECONDS_IN_HOUR;
        break;
    case INC_TIME_MINUTES:
        _startSeconds += SECONDS_IN_MINUTE;
        _startSeconds -= getSeconds(getFullSeconds()); // Set time to exact minute, remove seconds
        break;
    case INC_ALARM_HOURS:
        _alarmSeconds += SECONDS_IN_HOUR;
        break;
    case INC_ALARM_MINUTES:
        _alarmSeconds += SECONDS_IN_MINUTE;
        break;
    case DEC_TIME_HOURS:
        _startSeconds -= SECONDS_IN_HOUR;
        break;
    case DEC_TIME_MINUTES:
        _startSeconds -= SECONDS_IN_MINUTE;
        _startSeconds -= getSeconds(getFullSeconds()); // Set time to exact minute, remove seconds
        break;
    case DEC_ALARM_HOURS:
        _alarmSeconds -= SECONDS_IN_HOUR;
        break;
    case DEC_ALARM_MINUTES:
        _alarmSeconds -= SECONDS_IN_MINUTE;
        break;
    }
}

String AlarmClock::getTimeString(long fullSeconds)
{
    long hours = getHours(fullSeconds);
    long minutes = getMinutes(fullSeconds);
    long seconds = getSeconds(fullSeconds);

    String sHours = String(hours);
    String sMinutes = String(minutes);
    String sSeconds = String(seconds);

    if (minutes < 10)
        sMinutes = '0' + sMinutes;
    if (hours < 10)
        sHours = '0' + sHours;
    if (seconds < 10)
        sSeconds = '0' + sSeconds;

    return sHours + ':' + sMinutes; // + ':' + sSeconds;
}

String AlarmClock::getLCDOutput()
{
    long timeSeconds = getFullSeconds();
    long alarmSeconds = (_alarmSeconds + SECONDS_IN_DAY) % SECONDS_IN_DAY;
    String timeString = getTimeString(timeSeconds);
    String alarmString = getTimeString(alarmSeconds);
    bool blinkDisplay = (millis() % _blinkPeriod) < (_blinkPeriod / 4);

    if (_state == CLOCK)
    {
        return timeString;
    }
    else if (_state == SET_TIME_HOURS || _state == INC_TIME_HOURS || _state == DEC_TIME_HOURS)
    {
        if (!blinkDisplay)
            return timeString;
        else
            return "  " + timeString.substring(2);
    }
    else if (_state == SET_TIME_MINUTES || _state == INC_TIME_MINUTES || _state == DEC_TIME_MINUTES)
    {
        if (!blinkDisplay)
            return timeString;
        else
            return timeString.substring(0, 3) + "  " + timeString.substring(5);
    }
    else if (_state == SET_ALARM_HOURS || _state == INC_ALARM_HOURS || _state == DEC_ALARM_HOURS)
    {
        if (!blinkDisplay)
            return alarmString;
        else
            return "  " + alarmString.substring(2);
    }
    else if (_state == SET_ALARM_MINUTES || _state == INC_ALARM_MINUTES || _state == DEC_ALARM_MINUTES)
    {
        if (!blinkDisplay)
            return alarmString;
        else
            return alarmString.substring(0, 3) + "  " + alarmString.substring(5);
    }
}

String AlarmClock::getLCDSecondOutput()
{
    if (_alarmState)
    {
        long alarmSeconds = (_alarmSeconds + SECONDS_IN_DAY) % SECONDS_IN_DAY;
        String alarmString = getTimeString(alarmSeconds);
        return "Alarm: " + alarmString;
    }
    else
    {
        return "             ";
    }
}

bool AlarmClock::isAlarmActive()
{
    if (_alarmActive)
        return 1;

    if (!_alarmState)
        return 0;

    long alarmSeconds = (_alarmSeconds + SECONDS_IN_DAY) % SECONDS_IN_DAY;
    long timeSeconds = getFullSeconds();

    // Check if alarm time has passed in recent seconds
    if (timeSeconds > alarmSeconds && timeSeconds < (alarmSeconds + 5))
    {
        _alarmActive = 1;
        return 1;
    }

    return 0;
}
