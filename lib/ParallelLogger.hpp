#ifndef plogger_h
#define plogger_h

#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#include "ConcurrentQueue.hpp"
#include <cstdarg>
#include "jvar.h"
#include "datetime.h"
#include <map>

void loggerThread();
void closeLogger();
class logData{
public:
  DateTime now;
  int logLevel;
  std::thread::id threadId;
  std::string msg;
  logData(){
    msg="";
    logLevel=1;
  }
};
class LogStuff{
public:
  static ConcurrentQueue<logData> inputQueue;
  static std::string logLevelsText[4];
};


inline void _logLoop(logData &data,std::stringstream &strm)
{
  DateTime now;
  data.msg = strm.str();
  LogStuff::inputQueue.push(data);
}
template <typename T, typename... Types>
inline void _logLoop(struct logData &data,std::stringstream &strm,T var1, Types... var2)
{
    strm << var1 <<' '; //ja{var2...}
    //inputQueue.push(strm.str());
    _logLoop(data, strm,var2...);
}
template <typename... Types>
inline void log(Types... var2)
{
     logData data;
     data.logLevel=1;
     data.threadId = std::this_thread::get_id();
     std::stringstream strm;
    _logLoop(data,strm,var2...);
}

template <typename... Types>
inline void logdebug(Types... var2)
{
     logData data;
     data.logLevel=0;
     data.threadId = std::this_thread::get_id();
     std::stringstream strm;
    _logLoop(data,strm,var2...);
}
template <typename... Types>
inline void loginfo(Types... var2)
{
     logData data;
     data.logLevel=1;
     data.threadId = std::this_thread::get_id();
     std::stringstream strm;
    _logLoop(data,strm,var2...);
}
template <typename... Types>
inline void logwarn(Types... var2)
{
     logData data;
     data.logLevel=2;
     data.threadId = std::this_thread::get_id();
     std::stringstream strm;
    _logLoop(data,strm,var2...);
}
template <typename... Types>
inline void logerror(Types... var2)
{
     logData data;
     data.logLevel=3;
     data.threadId = std::this_thread::get_id();
     std::stringstream strm;
    _logLoop(data,strm,var2...);
}

class ThreadContextData
{
public:
  jvar data;
  std::mutex contextMutex;
};

class MDC{
private:
static std::map<std::string,ThreadContextData> contexts;
//static jvar contexts;
public:
  static void put(std::string key, jvar value);
  static jvar get(std::thread::id id);
  static void clear();
};

void loggerThread();


#endif

/*
Coloring CMD output
Here, \033 is the ESC character, ASCII 27. It is followed by [, then zero or more numbers separated by ;, and finally the letter m. The numbers describe the colour and format to switch to from that point onwards.

The codes for foreground and background colours are:

         foreground background
black        30         40
red          31         41
green        32         42
yellow       33         43
blue         34         44
magenta      35         45
cyan         36         46
white        37         47
Additionally, you can use these:

reset             0  (everything back to normal)
bold/bright       1  (often a brighter shade of the same colour)
underline         4
inverse           7  (swap foreground and background colours)
bold/bright off  21
underline off    24
inverse off      27
*/