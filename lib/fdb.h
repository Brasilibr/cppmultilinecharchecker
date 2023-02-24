
#ifndef fdb_h
#define fdb_h


#include <mysql.h>
#include <iostream>
#include "jvar.h"
#include "datetime.h" 

#define my_bool bool
//test
class MysqlInput
{
private:
    enum_field_types type;
    my_bool isNull=true;
    size_t size;
    char *buffer;
public:
    enum_field_types getType(){return type;};
    my_bool *getIsNullAddress(){return &isNull;};
    size_t getSize(){return size;};
    char * getBuffer(){return buffer;};
    ~MysqlInput();
    void deleteCurrent();
    template <class TType> void startBuffer( TType val);
    MysqlInput(signed char val);
    MysqlInput(short int val);
    MysqlInput(int val);
    MysqlInput(long long int val);
    MysqlInput(size_t val);
    MysqlInput(float val);
    MysqlInput(double val);
    MysqlInput(TimeStamp valarg);
    MysqlInput(DateTime valarg);
    MysqlInput(Date valarg);
    MysqlInput(Time valarg);
    MysqlInput(Mnull val);
    MysqlInput(std::string valarg);
    MysqlInput(const char val[]);
    MysqlInput(const MysqlInput& source);
    MysqlInput(MysqlInput&& source)  noexcept;//move
    MysqlInput(jvar source);
    MysqlInput(jvar &source);
};

class Fdb{
private:
    MYSQL *con=NULL;
    jvar bindToJvar(MYSQL_BIND *source);
    std::string host="";
    std::string user="";
    std::string pass="";
    std::string dbName="";
    unsigned int port=0;
    std::string charset="";
    MYSQL_STMT * startParamQuery(std::string query, std::vector<MysqlInput> paramsarg, jvar &retorno, int tries);
public:
    //escrever reconnect
    bool connect();
    Fdb();
    Fdb(Fdb && source);//move
    Fdb(std::string host, std::string user, std::string pass,std::string dbName, unsigned int port, std::string charset);
    ~Fdb();
    jvar setDb(std::string dbNameArg);
    jvar runQuery(std::string query);
    jvar runParamQuery(std::string query, std::vector<MysqlInput> paramsarg);
    jvar runCommand(std::string query, std::vector<std::string>  paramsarg);

};

#endif

