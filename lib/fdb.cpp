#include "fdb.h"
#include "ParallelLogger.hpp"

MysqlInput::~MysqlInput()
{
    deleteCurrent();
}
void MysqlInput::deleteCurrent()
{
    if (!isNull)
    {
        delete [] buffer;
        type=MYSQL_TYPE_NULL;
        buffer = NULL;
        size=0;
    }
}
template <class TType> void MysqlInput::startBuffer( TType val)
{
    isNull=false;
    size = sizeof(val);
    buffer = new char[size]; 
    memcpy(buffer,(const void*)&val,size); 
}
MysqlInput::MysqlInput(signed char val)
{
    startBuffer<signed char>(val);
    type = MYSQL_TYPE_TINY;
}
MysqlInput::MysqlInput(short int val)
{
    startBuffer<short int>(val);
    type = MYSQL_TYPE_SHORT;
}
MysqlInput::MysqlInput(int val)
{
    startBuffer<int>(val);
    type = MYSQL_TYPE_LONG;
}

MysqlInput::MysqlInput(long long int val) 
{
    startBuffer<long long int>(val);
    type = MYSQL_TYPE_LONGLONG;
}

MysqlInput::MysqlInput(size_t val)
{
    startBuffer<long long int>(val);
    type = MYSQL_TYPE_LONGLONG;
}

MysqlInput::MysqlInput(float val)
{
    startBuffer<float>(val);
    type = MYSQL_TYPE_FLOAT;
}
MysqlInput::MysqlInput(double val)
{
    startBuffer<double>(val);
    type = MYSQL_TYPE_DOUBLE;
}
MysqlInput::MysqlInput(TimeStamp valarg)
{
    MYSQL_TIME val = valarg.toMYSQLTime();
    startBuffer<MYSQL_TIME>(val);
    type = MYSQL_TYPE_TIMESTAMP;
}
MysqlInput::MysqlInput(DateTime valarg)
{
    MYSQL_TIME val = valarg.toMYSQLTime();
    startBuffer<MYSQL_TIME>(val);
    type = MYSQL_TYPE_DATETIME;
}
MysqlInput::MysqlInput(Date valarg)
{
    MYSQL_TIME val = valarg.toMYSQLTime();
    startBuffer<MYSQL_TIME>(val);
    type = MYSQL_TYPE_DATE;
}
MysqlInput::MysqlInput(Time valarg)
{
    MYSQL_TIME val = valarg.toMYSQLTime();
    startBuffer<MYSQL_TIME>(val);
    type = MYSQL_TYPE_TIME;
}
MysqlInput::MysqlInput(Mnull val)
{
    isNull = true;
    type=MYSQL_TYPE_NULL;
    buffer = NULL;
    size=0;
}
MysqlInput::MysqlInput(std::string valarg)
{
    isNull=false;
    char *val = (char*)valarg.c_str();
    size = valarg.size();
    buffer = new char[size];
    type = MYSQL_TYPE_STRING;
    memcpy(buffer,val,size);
}
MysqlInput::MysqlInput(const char val[])
{
    isNull=false;
    size = strlen(val);
    buffer = new char[size];
    type = MYSQL_TYPE_STRING;
    memcpy(buffer,val,size);
}
MysqlInput::MysqlInput(const MysqlInput& source)//copy
{
    isNull = source.isNull;
    type = source.type;
    size = source.size;
    buffer = new char[size];
    memcpy(buffer,source.buffer,size);
}
MysqlInput::MysqlInput(jvar source)//copy
{
    switch (source.getType())
    {
        case JBOOL:
            startBuffer<signed char>((bool)source.asInteger());
            type = MYSQL_TYPE_TINY;
        break;
        case JSTRING:
        {
            isNull=false;
            char *val = (char*)source.asString().c_str();
            size = source.size();
            buffer = new char[size];
            type = MYSQL_TYPE_STRING;
            memcpy(buffer,val,size);
        }
        break;
        case JNUMBER:
            startBuffer<double>(source.asDouble());
            type = MYSQL_TYPE_DOUBLE;
        break;
        case JLONGINTEGER:
            if (source<=32767)
            {
                startBuffer<short int>(((short int)source.asInteger()));
                type = MYSQL_TYPE_SHORT;
            } 
            else
                if (source<=2147483647)
                {
                    startBuffer<int>(((int)source.asInteger()));
                    type = MYSQL_TYPE_LONG;
                }
                else
                {
                    startBuffer<long long int>(source.asInteger());
                    type = MYSQL_TYPE_LONGLONG;
                }
        break;
        default:
            isNull = true;
            type=MYSQL_TYPE_NULL;
            buffer = NULL;
            size=0;
        break;
    } 
}
MysqlInput::MysqlInput(jvar &source)//copy
{
       switch (source.getType())
    {
        case JBOOL:
            startBuffer<signed char>((bool)source.asInteger());
            type = MYSQL_TYPE_TINY;
        break;
        case JSTRING:
        {
            isNull=false;
            char *val = (char*)source.asString().c_str();
            size = source.size();
            buffer = new char[size];
            type = MYSQL_TYPE_STRING;
            memcpy(buffer,val,size);
        }
        break;
        case JNUMBER:
            startBuffer<double>(source.asDouble());
            type = MYSQL_TYPE_DOUBLE;
        break;
        case JLONGINTEGER:
            if (source<=32767)
            {
                startBuffer<short int>(((short int)source.asInteger()));
                type = MYSQL_TYPE_SHORT;
            } 
            else
                if (source<=2147483647)
                {
                    startBuffer<int>(((int)source.asInteger()));
                    type = MYSQL_TYPE_LONG;
                }
                else
                {
                    startBuffer<long long int>(source.asInteger());
                    type = MYSQL_TYPE_LONGLONG;
                }
        break;
        default:
            isNull = true;
            type=MYSQL_TYPE_NULL;
            buffer = NULL;
            size=0;
        break;
    } 
}
MysqlInput::MysqlInput(MysqlInput&& source) noexcept//move
{
    deleteCurrent();
    isNull = source.isNull;
    source.isNull = true;
    type = source.type;
    size = source.size;
    buffer = source.buffer;
}



bool Fdb::connect()
{
    if (con)
    {
        mysql_close(con);
    }
    con = mysql_init(NULL);
    MYSQL * conRes;
    if (dbName == "")
        conRes = mysql_real_connect(con,host.c_str(),user.c_str(),pass.c_str(),NULL,port,NULL,CLIENT_MULTI_STATEMENTS);
    else
        conRes = mysql_real_connect(con,host.c_str(),user.c_str(),pass.c_str(),dbName.c_str(),port,NULL,CLIENT_MULTI_STATEMENTS);
    if (conRes==NULL)
    {
        //failed
        logerror(__FILE__,"row",__LINE__,"DB Connection to",user,"@",dbName,"-",host.c_str(),":",port,"Failed Error:",mysql_error(con));
        mysql_close(con);
        con=NULL;
        return false;
    }else
    {
        loginfo("DB Connection to",user,"@",dbName,"-",host.c_str(),":",port,"Succesful");
    }
    if (!mysql_set_character_set(con, charset.c_str()))
    {
        loginfo("DB New client character set",mysql_character_set_name(con));
    }
    bool reconnect = 1;
    mysql_options(con, MYSQL_OPT_RECONNECT, &reconnect);

    return true;
}
Fdb::Fdb(std::string hostarg, std::string userarg, std::string passarg,std::string dbNamearg, unsigned int portarg, std::string charsetarg)
{
    host=hostarg;
    user=userarg;
    pass=passarg;
    dbName=dbNamearg;
    port=portarg;
    charset = charsetarg;

    connect();
}
Fdb::Fdb()
{
    con=NULL;
}
Fdb::Fdb(Fdb && source)
{
    con=source.con;
    source.con=NULL;
}
Fdb::~Fdb()
{
    if (con)
    {
        std::cerr << "Mysql Connection Closed" << std::endl;
        mysql_close(con);
    }
}

size_t bufferMemoryMultiplierByType(enum_field_types mytype)
{
    switch(mytype)
    {
        case MYSQL_TYPE_GEOMETRY:
        case MYSQL_TYPE_ENUM:
        case MYSQL_TYPE_SET:
        case MYSQL_TYPE_BIT:
        case MYSQL_TYPE_VARCHAR:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_JSON:
            return 4;
        default:
            return 1;
    }
    return 1;
}
jvar Fdb::bindToJvar(MYSQL_BIND *source)
{
    jvar ret;
    if ((*(source->is_null)))
        return ret;
    switch (source->buffer_type)
    {
        case MYSQL_TYPE_NEWDECIMAL:
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_DOUBLE:
            return ret = (*(double*)source->buffer);
        case MYSQL_TYPE_FLOAT: 
            return ret = (double)(*(float*)source->buffer); 
        case MYSQL_TYPE_TINY:
            return ret = (*(char*)source->buffer);
        case MYSQL_TYPE_SHORT:  
            return ret = (*(short int*)source->buffer);
        case MYSQL_TYPE_LONG:
            return ret = (*(int*)source->buffer);
        case MYSQL_TYPE_NULL:  
            return ret; 
        case MYSQL_TYPE_TIMESTAMP:
        {
            MYSQL_TIME *time = (MYSQL_TIME *)source->buffer;
            TimeStamp ts(*time);
            return ret = (long long int)ts.getStamp();
        }
        case MYSQL_TYPE_LONGLONG:
            return ret = (*(long long int*)source->buffer);
        case MYSQL_TYPE_INT24:
            return ret = (*(long long int*)source->buffer);
        case MYSQL_TYPE_DATE:   
        {
            MYSQL_TIME *time = (MYSQL_TIME *)source->buffer;
            Date dt(*time);
            return ret = dt.toString();
        }
        case MYSQL_TYPE_TIME:
        {
            MYSQL_TIME *time = (MYSQL_TIME *)source->buffer;
            Time dt(*time);
            return ret = dt.toString();
        }
        case MYSQL_TYPE_DATETIME: 
        {
            MYSQL_TIME *time = (MYSQL_TIME *)source->buffer;
            DateTime dt(*time);
            return ret = dt.toString();
        }
        case MYSQL_TYPE_YEAR:
            return ret = (*(int*)source->buffer);
        case MYSQL_TYPE_NEWDATE: 
        {
            MYSQL_TIME *time = (MYSQL_TIME *)source->buffer;
            Date dt(*time);
            return ret = dt.toString();
        }
        case MYSQL_TYPE_GEOMETRY:
        case MYSQL_TYPE_ENUM:
        case MYSQL_TYPE_SET:
        case MYSQL_TYPE_BIT:
        case MYSQL_TYPE_VARCHAR:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
            return ret = std::string((char*)source->buffer);
        case MYSQL_TYPE_JSON:
            ret.parseJson((char*)source->buffer);
            return ret;
        default:
            return ret;
    }
}


jvar Fdb::runCommand(std::string query, std::vector<std::string> paramsarg)
{
    jvar queryEdit = query;
    for (auto &value : paramsarg)
    {
        queryEdit = queryEdit.replace("?",value);
    }
    mysql_query(con, queryEdit.asString().c_str());
    jvar result = mysql_error(con);
    if (result=="")
        result = jo{
            "ok" << jv true,
            "msg" << jv ""
        };
    else
        result = jo{
            "ok" << jv false,
            "msg" << result
        };
    return result;
}


jvar Fdb::setDb(std::string dbNameArg)
{
    if (dbName==dbNameArg)
        return jo{"ok" << jv true, "msg" << jv""};
    jvar result = runCommand("USE ?", {dbNameArg});
    if (result["ok"])
        dbName = dbNameArg;
    return result;
}

jvar Fdb::runQuery(std::string query)
{
    return runParamQuery(query, {});
}

MYSQL_STMT * Fdb::startParamQuery(std::string query, std::vector<MysqlInput> paramsarg, jvar &retorno, int tries)
{
    //std::cerr << "Try(" << tries << ")" << std::endl;
    if (tries>3)
        return NULL;
    if (con==NULL)
    {
        connect();
        if (con==NULL)
        {
            logerror(__FILE__,"row",__LINE__,"DB Err, not connected to db");
            return NULL;
        }
    }
    MYSQL_STMT * stmt;
    stmt = mysql_stmt_init (con);
    if (NULL == stmt)
    {
        logerror(__FILE__,"row",__LINE__,"DB Err, stmt Init",mysql_error(con));
        return NULL;
    }
    if (mysql_stmt_prepare (stmt, query.c_str(), query.size()))
    {
        mysql_stmt_close (stmt);
        std::string errorMsg =  std::string(mysql_stmt_error(stmt));
        logerror(__FILE__,"row",__LINE__,"DB Err, stmt prepare statement",errorMsg,"query:",query);
        if (errorMsg.find("server has gone away") != std::string::npos)
        {//try to reconnect and start stmt
            mysql_close(con);
            con=NULL;
            connect();
            return startParamQuery(query, paramsarg, retorno, tries+1);
        }
        return NULL;
    }
    retorno["msg"] = "";
    return stmt;
}
jvar Fdb::runParamQuery(std::string query, std::vector<MysqlInput> paramsarg)
{
    MDC::put("lastQuery:",query);
    jvar retorno = jo();
    retorno["data"] = ja();
    retorno["msg"] = "";
    retorno["changes"] = 0;
    retorno["ok"] = false;
    MYSQL_STMT * stmt = startParamQuery(query,paramsarg,retorno,0);
    if (stmt==NULL)
    {
        return retorno;
    }

    size_t param_count= mysql_stmt_param_count(stmt);
    size_t paramsSize = paramsarg.size();
    if (param_count!=paramsSize)
    {
        mysql_stmt_close (stmt);
        retorno["msg"] = "Erro: Sql Param Requested/Suplied Mismatch " + std::string( std::to_string(param_count) + "/" + std::to_string(paramsSize) );
        logerror(__FILE__,"row",__LINE__,"DB err2 Sql Param Requested/Suplied Mismatch (",param_count,"/",paramsSize,")");
        return retorno;
    }
    MYSQL_BIND *bparams;
    if (paramsSize>0)
    {
        bparams = new MYSQL_BIND [paramsSize];
        memset(bparams, 0, sizeof(MYSQL_BIND)*paramsSize);
        for(size_t i=0; i<paramsSize; i++)
        {
            bparams[i].buffer_type = paramsarg[i].getType();
            bparams[i].buffer = paramsarg[i].getBuffer();
            bparams[i].is_null = (my_bool*)paramsarg[i].getIsNullAddress();
            bparams[i].buffer_length = paramsarg[i].getSize();
            //std::cerr << bparams[i].buffer_type << "|" << bparams[i].buffer << "|"  << "|" << bparams[i].buffer_length << std::endl;
        }
        if (mysql_stmt_bind_param (stmt, bparams))
        {
            mysql_stmt_close (stmt);
            delete[] bparams;
            logerror(__FILE__,"row",__LINE__,"DB bind param:",mysql_stmt_error(stmt));
            return retorno;
        }
    }
    else
    {
        bparams = new MYSQL_BIND [1];
        memset(bparams,0,sizeof(MYSQL_BIND) );
    }
    
    //https://dev.mysql.com/doc/c-api/5.7/en/mysql-stmt-fetch.html
    if (mysql_stmt_execute (stmt))
    {
            mysql_stmt_close (stmt);
            delete[] bparams;
            logerror(__FILE__,"row",__LINE__,"DB stmt execute:",mysql_stmt_error(stmt));
            return retorno;
    }

    //Get meta params regarding query return table
    
    MYSQL_RES *prepare_meta_result = mysql_stmt_result_metadata(stmt);
    if (!prepare_meta_result)
    {
        std::string error( mysql_stmt_error(stmt) );
        if (error=="")
        {
            retorno["ok"]=true;
            retorno["changes"] = mysql_stmt_affected_rows(stmt);
            retorno["msg"] = "Changes: " + retorno["changes"];
        }
        else
        {
            logerror(__FILE__,"row",__LINE__,"DB err5 metadata:",mysql_stmt_error(stmt));
        }
            mysql_stmt_close (stmt);
            delete[] bparams;
            return retorno;
    }
    
    size_t column_count = mysql_num_fields(prepare_meta_result);
    if (column_count<1)
        column_count = 1;
    MYSQL_BIND *bresult = new MYSQL_BIND [column_count];
    std::string *fieldNames = new std::string[column_count];
    unsigned long *sizes = new unsigned long[column_count];
    my_bool *isnulls = new my_bool[column_count];
    my_bool *errors = new my_bool[column_count];
    memset(sizes,0,sizeof(unsigned long) * column_count);
    memset(isnulls,0,sizeof(my_bool) * column_count);
    memset(errors,0,sizeof(my_bool) * column_count);

    MYSQL_FIELD *field;
    size_t pos=0;
    size_t bufferLength=0;
    size_t mysqlTimeMemoryBugFixPaddingBytes = 16;
    while(field = mysql_fetch_field(prepare_meta_result)) 
    {
        //std::cerr <<  field->name << "-t:" << field->type << " s:" << field->length <<" | ";
        //if (field->length>0)
        bufferLength = field->length *bufferMemoryMultiplierByType(field->type) + mysqlTimeMemoryBugFixPaddingBytes;
        bresult[pos].buffer =  new char[bufferLength];
        memset(bresult[pos].buffer,0,bufferLength);
        bresult[pos].buffer_type = field->type;
        bresult[pos].buffer_length = bufferLength;
        bresult[pos].is_null = &isnulls[pos];
        bresult[pos].length = &sizes[pos];
        bresult[pos].error = &errors[pos];
        fieldNames[pos] = field->name;
        pos++;
    }
    //std::cerr << std::endl;
    mysql_free_result(prepare_meta_result);
    
    if (mysql_stmt_store_result(stmt))
    {
        mysql_stmt_close (stmt);
        for(size_t i=0; i<column_count; i++)
        {
            if (bresult[i].buffer_length>0)
                delete[] (char*)bresult[i].buffer;
        }
        delete[] bresult;
        delete[] bparams;
        delete[] fieldNames;
        delete[] isnulls;
        delete[] errors;
        delete[] sizes;
        logerror(__FILE__,"row",__LINE__,"DB err6 store result:",mysql_stmt_error(stmt));
        return retorno;
    }
    
    
    if (mysql_stmt_bind_result(stmt, bresult))
    {
        mysql_stmt_close (stmt);
        for(size_t i=0; i<column_count; i++)
        {
            if (bresult[i].buffer_length>0)
                delete[] (char*)bresult[i].buffer;
        }
        delete[] bresult;
        delete[] bparams;
        delete[] fieldNames;
        delete[] isnulls;
        delete[] errors;
        delete[] sizes;
        logerror(__FILE__,"row",__LINE__,"DB err7 bind result:",mysql_stmt_error(stmt));
        return retorno;
    }
    size_t row_count = 0;
    size_t ret=0;
    
    while (1)
    {
        ret = mysql_stmt_fetch(stmt);
        if (ret == 1 || ret == MYSQL_NO_DATA)
            break;

        if (ret==MYSQL_DATA_TRUNCATED)
            logwarn("DB data truncated!");
            
        jo row = jo();
        row.reserve(column_count);//reserve memory space for all column itens
        for(size_t i=0; i<column_count; i++)
        {
            row[fieldNames[i]] = bindToJvar(&bresult[i]);
            memset(bresult[i].buffer,0,bresult[i].buffer_length);//limpa buffer para o proxima result
        }
        retorno["data"][row_count] = row;
        row_count++;
    }

    for(size_t i=0; i<column_count; i++)
    {
        if (bresult[i].buffer_length>0)
            delete[] (char*)bresult[i].buffer;
    }
    delete[] bresult;
    delete[] bparams;
    delete[] fieldNames;
    delete[] isnulls;
    delete[] errors;
    delete[] sizes; 
    
    if (mysql_stmt_free_result(stmt))
        logerror(__FILE__,"row",__LINE__,"DB stmt Free Err:",mysql_stmt_error(stmt));
    
    retorno["ok"]=true;
    mysql_stmt_close (stmt);
    MDC::put("lastQuery:",MNULL);
    return retorno;
}