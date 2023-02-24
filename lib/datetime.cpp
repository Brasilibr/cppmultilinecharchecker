#include "datetime.h"
#include <iostream>
#include "../lib/ParallelLogger.hpp"
#include "../lib/jvar.h"

DateTime::DateTime()
{
    time_t current_time = time(NULL);
    struct tm * timeinfo;
    timeinfo = localtime64 (&current_time);
    setInfo(*timeinfo);
}

void DateTime::setInfo(const struct tm &timeinfo)
{
    years = timeinfo.tm_year+1900;
    months = timeinfo.tm_mon+1;
    days = timeinfo.tm_mday;
    hour = timeinfo.tm_hour;
    minutes = timeinfo.tm_min;
    seconds = timeinfo.tm_sec;
}

DateTime::DateTime(int yearsarg,char monthsarg,char daysarg , short hourarg, char minarg, char secarg)
{
    days = daysarg;
    months = monthsarg;
    years = yearsarg;
    hour = hourarg;
    minutes = minarg;
    seconds = secarg;
}
DateTime::DateTime (Time64_T timeStamp)
{
    struct tm * timeinfo;
    timeinfo = localtime64 (&timeStamp);
    setInfo(*timeinfo);
}
DateTime::DateTime (const struct tm &timeinfo)
{
    setInfo(timeinfo);
}
DateTime::DateTime (const MYSQL_TIME &sqlTime)
{
    years=sqlTime.year;
    months=sqlTime.month;
    days=sqlTime.day;
    hour=sqlTime.hour;
    minutes=sqlTime.minute;
    seconds=sqlTime.second;
}
short DateTime::getHour(){return hour;}
char DateTime::getMinutes(){return minutes;}
char DateTime::getSeconds(){return seconds;}
char DateTime::getDays(){return days;}
char DateTime::getMonths(){return months;}
int DateTime::getYears(){return years;}
MYSQL_TIME DateTime::toMYSQLTime(){
    MYSQL_TIME sqlTime;
    memset(&sqlTime,0,sizeof(MYSQL_TIME));
    sqlTime.year = years;
    sqlTime.month = months;
    sqlTime.day = days;
    sqlTime.hour = hour;
    sqlTime.minute = minutes;
    sqlTime.second = seconds;
    return sqlTime;
}
struct tm DateTime::toLocalTime()
{
    struct tm timeinfo;
    memset(&timeinfo,0,sizeof(struct tm));
    timeinfo.tm_year = years-1900;
    timeinfo.tm_mon =months-1;
    timeinfo.tm_mday =days;
    timeinfo.tm_hour =hour;
    timeinfo.tm_min =minutes;
    timeinfo.tm_sec = seconds;
    return timeinfo;
}
/*TimeStamp in Seconds*/
Time64_T DateTime::toTimeStamp()
{
    struct tm timeinfo = this->toLocalTime();
    Time64_T stamp = mktime64(&timeinfo);
    return stamp;
}
std::string DateTime::toString()
{
    std::string ret ="";
    if (years<1000 && years>=0)
        ret += "0";
    if (years<100 && years>=0)
        ret += "0";
    if (years<10 && years>=0)
        ret += "0"+std::to_string(years) + "-";
    else
        ret += std::to_string(years) + "-";

    if (months<10 && months>=0)
        ret +=  "0"+std::to_string(months) + "-";
    else
        ret += std::to_string(months) + "-";

    if (days<10 && days>=0)
        ret +=  "0"+std::to_string(days);
    else
        ret += std::to_string(days);

    ret += " ";   
    if (hour<10 && hour>=0)
        ret += "0"+std::to_string(hour) + ":";
    else
        ret += std::to_string(hour) + ":";
    
    if (minutes<10 && minutes>=0)
        ret +=  "0"+std::to_string(minutes) + ":";
    else
        ret += std::to_string(minutes) + ":";

    if (seconds<10 && seconds>=0)
        ret +=  "0"+std::to_string(seconds) ;
    else
        ret += std::to_string(seconds) ;
    return ret;
}


void TimeUtil::startTimer(mtimer & start)
{
    start = std::chrono::high_resolution_clock::now();
}
void TimeUtil::finishTimer(mtimer & start)
{
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - start ).count();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - start ).count();
    jvar outputs = jo{"TimeElapsed" << jo{"in_ms" << jv duration2, "in_us" << jv duration }};
    loginfo(outputs);
}

void Time::setInfo(const struct tm &timeinfo)
{
    hour = timeinfo.tm_hour;
    minutes = timeinfo.tm_min;
    seconds = timeinfo.tm_sec;
}

Time::Time()
{
    time_t current_time = time(NULL);
    struct tm * timeinfo;
    timeinfo = localtime64 (&current_time);
    setInfo(*timeinfo);
}   
Time::Time(short hourarg, char minarg, char secarg)
{
    hour = hourarg;
    minutes = minarg;
    seconds = secarg;
}
Time::Time (Time64_T timeStamp)
{
    struct tm * timeinfo;
    timeinfo = localtime64 (&timeStamp);
    setInfo (*timeinfo);
}
Time::Time (const struct tm &timeinfo)
{
    setInfo(timeinfo);
}
Time::Time (const MYSQL_TIME &sqlTime)
{
    hour=sqlTime.hour;
    minutes=sqlTime.minute;
    seconds=sqlTime.second;
}
short Time::getHour(){return hour;}
char Time::getMinutes(){return minutes;}
char Time::getSeconds(){return seconds;}
MYSQL_TIME Time::toMYSQLTime(){
    MYSQL_TIME sqlTime;
    memset(&sqlTime,0,sizeof(MYSQL_TIME));
    sqlTime.hour = hour;
    sqlTime.minute = minutes;
    sqlTime.second = seconds;
    return sqlTime;
}
struct tm Time::toLocalTime()
{
    struct tm timeinfo;
    memset(&timeinfo,0,sizeof(struct tm));
    timeinfo.tm_hour =hour;
    timeinfo.tm_min =minutes;
    timeinfo.tm_sec = seconds;
    return timeinfo;
}
std::string Time::toString()
{
    std::string ret ="";
    if (hour<10 && hour>=0)
        ret += "0"+std::to_string(hour) + ":";
    else
        ret += std::to_string(hour) + ":";
    
    if (minutes<10 && minutes>=0)
        ret +=  "0"+std::to_string(minutes) + ":";
    else
        ret += std::to_string(minutes) + ":";

    if (seconds<10 && seconds>=0)
        ret +=  "0"+std::to_string(seconds) + ":";
    else
        ret += std::to_string(seconds) + ":";
    return ret;
}


void Date::setInfo(const struct tm &timeinfo)
{
    years = timeinfo.tm_year+1900;
    months = timeinfo.tm_mon+1;
    days = timeinfo.tm_mday;
}
Date::Date()
{
    time_t current_time = time(NULL);
    struct tm * timeinfo;
    timeinfo = localtime64 (&current_time);
    setInfo(*timeinfo);
}
/*string in format: yyyy-mm-dd*/
Date::Date(std::string yyyymmdd)
{
    try{
    years =  std::stoi(yyyymmdd.substr(0,4));
    months = std::stoi(yyyymmdd.substr(5,2));
    days = std::stoi(yyyymmdd.substr(8,2));
    }catch (const std::invalid_argument & e) {
        logerror("Date string to date conversion failed:",e.what());
        years = 1900;months = 1;days = 1;
    }
    catch (const std::out_of_range & e) {
        logerror("Date string to date conversion failed:",e.what());
        years = 1900;months = 1;days = 1;
    }
}
Date::Date(int yearsarg, char monthsarg,char daysarg)
{
    years = yearsarg;
    days = daysarg;
    months = monthsarg;
}
Date::Date (Time64_T timeStamp)
{
    struct tm * timeinfo;
    timeinfo = localtime64 (&timeStamp);
    setInfo (*timeinfo);
}
Date::Date (const struct tm &timeinfo)
{
    setInfo(timeinfo);
}
Date::Date (const MYSQL_TIME &sqlTime)
{
    years=sqlTime.year;
    months=sqlTime.month;
    days=sqlTime.day;
}
struct tm Date::toLocalTime()
{
    struct tm timeinfo;
    memset(&timeinfo,0,sizeof(struct tm));
    timeinfo.tm_year = years;
    timeinfo.tm_mon = months-1;
    timeinfo.tm_mday = days;
    timeinfo.tm_hour =0;
    timeinfo.tm_min =0;
    timeinfo.tm_sec =1;
    return timeinfo;
}
Time64_T Date::toTimeStamp()
{
    struct tm timeinfo = this->toLocalTime();
    timeinfo.tm_year -= 1900;
    Time64_T stamp = mktime64(&timeinfo);
    return stamp;
}
char Date::getDays(){return days;}
char Date::getMonths(){return months;}
int Date::getYears(){return years;}
MYSQL_TIME Date::toMYSQLTime(){
    MYSQL_TIME sqlTime;
    memset(&sqlTime,0,sizeof(MYSQL_TIME));
    sqlTime.year = years;
    sqlTime.month = months;
    sqlTime.day = days;
    return sqlTime;
}
std::string Date::toString()
{
    std::string ret ="";
    if (years<10 && years>=0)
        ret += "0"+std::to_string(years) + "-";
    else
        ret += std::to_string(years) + "-";

    if (months<10 && months>=0)
        ret +=  "0"+std::to_string(months) + "-";
    else
        ret += std::to_string(months) + "-";

    if (days<10 && days>=0)
        ret +=  "0"+std::to_string(days);
    else
        ret += std::to_string(days);

    return ret;
}
bool operator>(Date left, const Date &right)
{
    return (left.toTimeStamp() > ((Date&)right).toTimeStamp());
}
bool operator<=(Date left, const Date &right)
{
    return !(left>right);
}
bool operator<(Date left, const Date &right)
{
    return (left.toTimeStamp() < ((Date&)right).toTimeStamp());
}
bool operator>=(Date left, const Date &right)
{
    return !(left<right);
}
bool operator==(Date left, const Date &right)
{
    return (left.toTimeStamp() == ((Date&)right).toTimeStamp());
}



Time64_T TimeStamp::getStamp(){return stamp;}

TimeStamp::TimeStamp(Time64_T stamparg)
{
    stamp = stamparg;
}
struct tm  TimeStamp::toLocalTime()
{
    struct tm * timeinfo;
    timeinfo = localtime64 (&stamp);
    return (*timeinfo);
}
TimeStamp::TimeStamp(const MYSQL_TIME &mytime)
{
    struct tm timeinfo;
    memset(&timeinfo,0,sizeof(struct tm));
    timeinfo.tm_year = mytime.year-1900;
    timeinfo.tm_mon = mytime.month-1;
    timeinfo.tm_mday =mytime.day;
    timeinfo.tm_hour =mytime.hour;
    timeinfo.tm_min =mytime.minute;
    timeinfo.tm_sec =mytime.second;
    stamp = mktime64(&timeinfo);
}
MYSQL_TIME  TimeStamp::toMYSQLTime()
{
    struct tm * timeinfo;
    MYSQL_TIME sqlTime;
    memset(&sqlTime,0,sizeof(MYSQL_TIME));
    timeinfo = localtime64 (&stamp);
    sqlTime.year = timeinfo->tm_year+1900;
    sqlTime.month = timeinfo->tm_mon+1;
    sqlTime.day = timeinfo->tm_mday;
    sqlTime.hour = timeinfo->tm_hour;
    sqlTime.minute = timeinfo->tm_min;
    sqlTime.second = timeinfo->tm_sec;
    return sqlTime;
}