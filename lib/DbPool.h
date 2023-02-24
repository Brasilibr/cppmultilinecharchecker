
#include "fdb.h"
#include "ConcurrentQueue.hpp"
#include <iostream>

class DbPool;

class QueuedFdb{
    private:
        DbPool *dbpool;
        std::string dbName="";
    public:
    QueuedFdb(DbPool *dbpoolarg);
    QueuedFdb(QueuedFdb &&source);
    QueuedFdb& operator=(QueuedFdb&& source) noexcept;//move assignment
    QueuedFdb& operator=(QueuedFdb& source) noexcept;//copy assignment
    void setDb(std::string dbNameArg);
    /*Send a command to MysqlServer, for selecting databases, creating databases, etc, not for data (unsafe, use it only with trusted input)*/
    jvar runCommand(std::string query, std::vector<std::string> params);
    /*use for simple queries/data alteration that do not need parameters (run as parametrized query)*/
    jvar runQuery(std::string query);
    /*use for param queries/data alteration */
    jvar runParamQuery(std::string query, std::vector<MysqlInput> paramsarg);
};

class ParallelDb
{
    private:
        jvar parallelQueriesResults;
        std::vector<std::thread> parallelQueriesWorkers;
        std::mutex resultMutex;
        std::string dbName="";
        size_t threadIds=0;
    public:
    void setDb(std::string dbNameArg);
    void runCommand(std::string resultName,std::string query, std::vector<std::string> params);
    void runParamQuery(std::string resultName,std::string query, std::vector<MysqlInput> paramsarg);
    void runQuery(std::string resultName,std::string query);
    void _addParallelQueryResult(std::string id, jvar &result);
    jvar collectResults();
};

class DbPool {
private:
    ConcurrentQueue<Fdb *> queue;
    DbPool(DbPool const&);              // Don't Implement
    void operator=(DbPool const&);
    DbPool() {};
public:
    ~DbPool();
    static DbPool& singleton() {
      static DbPool INSTANCE;
      return INSTANCE;
    }
    void addDb(Fdb *db);
    Fdb* getFdbInstance();
    QueuedFdb getDb();
};  

