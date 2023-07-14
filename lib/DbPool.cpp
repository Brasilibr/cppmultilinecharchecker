#include "DbPool.h"
#include "ParallelLogger.hpp"

    DbPool::~DbPool() 
    {
        while(!queue.empty())
        {
            Fdb *db;
            db = queue.wait_and_pop();
            delete db;
        }
    }
    void DbPool::addDb(Fdb *db)
    {
        queue.push(db); 
    }
    Fdb* DbPool::getFdbInstance()
    {
        Fdb *db;
        db = queue.wait_and_pop();
        return db;
    }
    QueuedFdb DbPool::getDb()
    {
        QueuedFdb ret(this);
        return ret;
    }
    void QueuedFdb::setDb(std::string dbNameArg)
    {
        dbName = dbNameArg;
    }
    jvar QueuedFdb::runCommand(std::string query, std::vector<std::string> params)
    {
        Fdb* db = dbpool->getFdbInstance();
        if (dbName!="")
            db->setDb(dbName);
        jvar ret = db->runCommand(query,params);
        dbpool->addDb(db);
        return ret;
    }
    jvar QueuedFdb::runQuery(std::string query)
    {
        return this->runParamQuery(query,{});
    }
    jvar QueuedFdb::runParamQuery(std::string query, std::vector<MysqlInput> paramsarg)
    {
        Fdb* db = dbpool->getFdbInstance();
        if (dbName!="")
            db->setDb(dbName);
        jvar ret = db->runParamQuery(query,paramsarg);
        dbpool->addDb(db);
        return ret;
    }

    QueuedFdb::QueuedFdb(DbPool *dbpoolarg)
    {
        dbpool = dbpoolarg;
    }
    QueuedFdb::QueuedFdb(QueuedFdb &&source)
    {
        *this = std::move(source);
    }
    QueuedFdb& QueuedFdb::operator=(QueuedFdb&& source) noexcept//move assignment
    {
        if(this != &source) {
            dbpool = source.dbpool;
        }
        return *this;
    }
    QueuedFdb& QueuedFdb::operator=(QueuedFdb& source) noexcept//copy assignment
    {
        if(this != &source) {
            dbpool = source.dbpool;
        }
        return *this;
    }



    void runCommandInParallel(std::string query, std::vector<std::string> paramsarg,std::string myid, ParallelDb * pool, std::string dbName)
    {
        auto db = DbPool::singleton().getDb();
        if (dbName!="")
            db.setDb(dbName);
        jvar result = db.runCommand(query,paramsarg);
        if (!result["ok"])
            logerror(__FILE__,"row",__LINE__,"ParallelDB Run Command:",result["msg"].asString());
        pool->_addParallelQueryResult(myid,result);
    }
    void runQueryInParallel(std::string query, std::vector<MysqlInput> paramsarg,std::string myid, ParallelDb * pool, std::string dbName)
    {
        auto db = DbPool::singleton().getDb();
        if (dbName!="")
            db.setDb(dbName);
        jvar result = db.runParamQuery(query,paramsarg);
        if (!result["ok"])
            logerror(__FILE__,"row",__LINE__,"ParallelDB Run Query:",result["msg"].asString());
        pool->_addParallelQueryResult(myid,result);
    }
    void ParallelDb::setDb(std::string dbNameArg)
    {
        dbName = dbNameArg;
    }
    void ParallelDb::runCommand(std::string resultName,std::string query, std::vector<std::string> paramsarg)
    {
        parallelQueriesWorkers.push_back(std::thread(runCommandInParallel,query,paramsarg,resultName,this,dbName));
        threadIds++;
    }

    void ParallelDb::runParamQuery(std::string resultName,std::string query, std::vector<MysqlInput> paramsarg)
    {
        parallelQueriesWorkers.push_back(std::thread(runQueryInParallel,query,paramsarg,resultName,this,dbName));
        threadIds++;
    }
    void ParallelDb::runQuery(std::string resultName,std::string query)
    {
        runParamQuery(resultName,query,{});
    }
    void ParallelDb::_addParallelQueryResult(std::string id, jvar &result)
    {
        std::lock_guard<std::mutex> ulk(resultMutex);
        parallelQueriesResults[id] = result;
    }
    jvar ParallelDb::collectResults()
    {
        jvar result;
        for (size_t i = 0; i < threadIds; i++)
        {
            parallelQueriesWorkers[i].join();
        }
        threadIds=0;
        parallelQueriesWorkers.clear();
        return std::move(parallelQueriesResults);
    }