
#ifndef datetime_h
#define datetime_h

#include "../lib/time64.h"
#include <mysql.h>
#include <string>
#include <string.h>
#include <chrono>

class DateTime{
private:
    char days;
    char months;
    int years;
    short hour;
    char minutes;
    char seconds;
    char timeZone;
    void setInfo(const struct tm &timeinfo);
public:
    /*starts with now time */
    DateTime();
    DateTime(int yearsarg,char monthsarg,char daysarg , short hourarg, char minarg, char secarg);
    DateTime (Time64_T timeStamp);
    DateTime (const struct tm &timeinfo);
    DateTime (const MYSQL_TIME &sqlTime);
    short getHour();
    char getMinutes();
    char getSeconds();
    char getDays();
    char getMonths();
    int getYears();
    void setTimeZone(char zone);
    MYSQL_TIME toMYSQLTime();
    struct tm toLocalTime();
    Time64_T toTimeStamp();
    std::string toString();
};


class Time{
private:
    short hour;
    char minutes;
    char seconds;
    void setInfo(const struct tm &timeinfo);
public:
    /*starts with now time */
    Time();  
    Time(short hourarg, char minarg, char secarg);
    Time (Time64_T timeStamp);
    Time (const struct tm &timeinfo);
    Time (const MYSQL_TIME &sqlTime);
    short getHour();
    char getMinutes();
    char getSeconds();
    MYSQL_TIME toMYSQLTime();
    struct tm toLocalTime();
    std::string toString();
};

typedef std::chrono::_V2::high_resolution_clock::time_point mtimer;

class TimeUtil{
    public:
    static void startTimer(mtimer & start);
    static void finishTimer(mtimer & start);
};  
   
 
class Date{ 
private:
    char days;
    char months;
    int years;
    void setInfo(const struct tm &timeinfo);
public:
    /*starts with now time */
    Date();
    Date(int yearsarg, char monthsarg,char daysarg);
    Date(std::string yyyymmdd);
    Date (Time64_T timeStamp);
    Date (const struct tm &timeinfo);
    Date (const MYSQL_TIME &sqlTime);
    struct tm toLocalTime();
    Time64_T toTimeStamp();
    char getDays();
    char getMonths();
    int getYears();
    MYSQL_TIME toMYSQLTime();
    std::string toString();

    friend bool operator<(Date left, const Date &right);
    friend bool operator==(Date left, const Date &right);
    friend bool operator>(Date left, const Date &right);
    friend bool operator>=(Date left, const Date &right);
    friend bool operator<=(Date left, const Date &right);
};

class TimeStamp{
private:
    Time64_T stamp;
public:
    Time64_T getStamp();
    TimeStamp(Time64_T stamparg);
    struct tm  toLocalTime();
    TimeStamp(const MYSQL_TIME &mytime);
    MYSQL_TIME  toMYSQLTime();
};

#endif