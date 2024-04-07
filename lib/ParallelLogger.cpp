#include "ParallelLogger.hpp"

bool openLogger = true;


int LogStuff::logLevel=1;
std::mutex LogStuff::logLevelMutex;

void LogStuff::setLogLevel(int level){
  std::unique_lock<std::mutex> ulk(logLevelMutex);
  if (level>3)
    level=3;
  if (level<0)
    level=0;
  logLevel=level;
};

void cleanContext(jvar & myvar)
{
  size_t maxSize=256;
  for(jvar & item: myvar)
  {
    if (item.isString())
    {
      if (item.size()>maxSize)
      {
        item = item.substr(0,maxSize)+"...";
      }
    }
    if (item.isObject())
    {
      cleanContext(item);
    }
  }
}
void loggerThread()
{
  std::cout << "Logging Started" << std::endl;
  std::string start[4]= {"1;37m","0m","1;33m","1;31m"};

  while(openLogger)
  {
    logData mydata = LogStuff::inputQueue.wait_and_pop();
    jvar context = MDC::get(mydata.threadId);
    cleanContext(context);
    //context=jo{};
    std::cout << "\033[" << start[mydata.logLevel]; //1 = bold; 31 = red
    std::cout <<"\033[4m" << mydata.now.toString() << "::" << LogStuff::logLevelsText[mydata.logLevel] <<"\033[24m" << " " << mydata.msg << " ctx:" << context << "   tid:" << mydata.threadId << "\n";
    std::cout << "\033[0m";
  }
}
std::map<std::string, ThreadContextData> MDC::contexts;

void MDC::put(std::string key, jvar value){
  std::stringstream ss;
  ss << std::this_thread::get_id();
  std::string myId = ss.str();
  try{
    auto &context = contexts.at(myId);
    std::lock_guard<std::mutex> ulk(context.contextMutex);
    context.data[key]=value;
  }catch(std::out_of_range e)
  {
    try{
    ThreadContextData a;
    contexts.try_emplace(myId);
    auto &context = contexts.at(myId);
    std::lock_guard<std::mutex> ulk(context.contextMutex);
    context.data[key]=value;
    }catch(std::out_of_range e)
    {
      std::cout << "ERR::::: LOGGER MDC PUT UNABLE TO GET CONTEXT FOR THREAD: " << myId << std::endl;
    }
  }
}

jvar MDC::get(std::thread::id id)
{
  std::stringstream ss;
  ss << id;
  std::string myId = ss.str();
  try{
    auto &context = contexts.at(myId);
    std::lock_guard<std::mutex> ulk(context.contextMutex);
    return context.data;
  }catch(std::out_of_range e)
  {
    try{
    contexts.try_emplace(myId);
    auto &context = contexts.at(myId);
    std::lock_guard<std::mutex> ulk(context.contextMutex);
    context.data=MNULL;
    return context.data;
    }catch(std::out_of_range e)
    {
      std::cout << "ERR::::: LOGGER MDC UNABLE TO GET CONTEXT FOR THREAD: " << myId << std::endl;
      return MNULL;
    }
  }
}
void MDC::clear()
{
std::stringstream ss;
ss << std::this_thread::get_id();
std::string myId = ss.str();
try{
    auto &context = contexts.at(myId);
    std::lock_guard<std::mutex> ulk(context.contextMutex);
    context.data=MNULL;
  }catch(std::out_of_range e)
  {
    try{
      contexts.try_emplace(myId);
      auto &context = contexts.at(myId);
      std::lock_guard<std::mutex> ulk(context.contextMutex);
      context.data=MNULL;
    }catch(std::out_of_range e)
    {
      std::cout << "ERR::::: LOGGER MDC CLEAR UNABLE TO GET CONTEXT FOR THREAD: " << myId << std::endl;
    }
  }
}


ConcurrentQueue<logData> LogStuff::inputQueue;
std::string LogStuff::logLevelsText[4] = {"debug","info","warn","error"};

auto t1 = std::thread(loggerThread);


void closeLogger()
{
  openLogger=false;
  loginfo("closing logger");
  t1.join();
}