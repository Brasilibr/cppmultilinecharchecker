#include "jvar.h"
#include "httplib.h"
#include "UWebStringUtils.h"
#include <malloc.h>
#include "memoryuse.h"
#include "datetime.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <map>
#include "ParallelLogger.hpp"

using namespace std::chrono_literals;

#define DEFAULT_DESIRED_IDLE_MEMORY 52428800 //50 mb
#define DEFAULT_DESIRED_RELEASE_THRESHOLD 67108864*2 //64 MB
#define DEFAULT_TIME_TO_CHECK_RELEASE_MEMORY 5 //5 seconds

void memoryFree();
void processOptions(const httplib::Request& req, httplib::Response& res);
void processGetRequests(const httplib::Request& req, httplib::Response& res);
void processPostRequests(const httplib::Request& req, httplib::Response& res);
void processPatchRequests(const httplib::Request& req, httplib::Response& res);
void processDeleteRequests(const httplib::Request& req, httplib::Response& res);



class Route{
private:
    std::vector<bool(*)(const httplib::Request& req, httplib::Response& res, jvar &ctx)> filters;
public:
    jvar(*processRequest)(const httplib::Request& req, httplib::Response& res, jvar &ctx);
    Route(jvar(*controllerarg)(const httplib::Request& req, httplib::Response& res, jvar &ctx))
    {
        processRequest = controllerarg;
    }
    Route * filter(bool(* funarg)(const httplib::Request& req, httplib::Response& res, jvar &ctx))
    {
        filters.push_back(funarg);
        return this;
    }
    std::vector<bool(*)(const httplib::Request& req, httplib::Response& res, jvar &ctx)> &getFilters()
    {
        return filters;
    }
};



class MyApp{
    private:
        std::vector<bool(*)(const httplib::Request& req, httplib::Response& res, jvar &ctx)> filters;
        static size_t desiredIdleMemory;
        static size_t desiredReleaseThreshold;
        static double timeToReleaseMemory;
        static std::mutex memoryControllerMutex;
    public:
    std::map<std::string,Route> getRoutes;
    std::map<std::string,Route> patchRoutes;
    std::map<std::string,Route> postRoutes;
    std::map<std::string,Route> deleteRoutes;
    httplib::Server svr;
    jvar config;
    static double getTimeToReleaseMemory()
    {
        std::lock_guard<std::mutex> ulk(memoryControllerMutex);
        return timeToReleaseMemory;
    }
    static size_t getDesiredReleaseThreshold()
    {
        std::lock_guard<std::mutex> ulk(memoryControllerMutex);
        return desiredReleaseThreshold;
    }
    static size_t getDesiredIdleMemory()
    {
        std::lock_guard<std::mutex> ulk(memoryControllerMutex);
        return desiredIdleMemory;
    }
    /*time in seconds*/
    static void setTimeToReleaseMemory(double seconds)
    {
        std::lock_guard<std::mutex> ulk(memoryControllerMutex);
        timeToReleaseMemory=seconds;
    }
    /*mem value in bytes*/
    static void setDesiredReleaseThreshold(size_t bytes)
    {
        std::lock_guard<std::mutex> ulk(memoryControllerMutex);
        desiredReleaseThreshold=bytes;
    }
    /*mem value in bytes*/
    static void setDesiredIdleMemory(size_t bytes)
    {
        std::lock_guard<std::mutex> ulk(memoryControllerMutex);
        desiredIdleMemory=bytes;
    }
    std::vector<bool(*)(const httplib::Request& req, httplib::Response& res, jvar &ctx)> &getFilters()
    {
        return filters;
    }
    MyApp * filter(bool(* funarg)(const httplib::Request& req, httplib::Response& res, jvar &ctx))
    {
        filters.push_back(funarg);
        return this;
    }
    void setConfig(jvar & configarg)
    {
        config = configarg;
    }
    Route * addRoute(std::string path,enum httpmethod method, jvar(*controllerarg)(const httplib::Request& req, httplib::Response& res, jvar &ctx))
    {
        Route r(controllerarg);
        switch(method)
        {
            case HGET:
                return &getRoutes.insert(std::pair<std::string, Route>(path,r)).first->second;
            break;
            case HPOST:
                return &postRoutes.insert(std::pair<std::string, Route>(path,r)).first->second;
            break;
            case HPATCH:
                return &patchRoutes.insert(std::pair<std::string, Route>(path,r)).first->second;
            break;
            case HDELETE:
                return &deleteRoutes.insert(std::pair<std::string, Route>(path,r)).first->second;
            break;
            default:
                logwarn("Server: Tried to add route with unsupported method");
        }
        return NULL;
    }
    void start(jvar &config, bool &actionBodyExists)
    {
        if (actionBodyExists)
            return runAsAction(config);
        setConfig(config);
        std::thread t1(memoryFree);
        t1.detach();
        svr.Options(R"(.*)", processOptions); // always accept requests from everywhere, filtering is done later
        svr.Get(R"(.*)", processGetRequests);
        svr.Post(R"(.*)", processPostRequests);
        svr.Patch(R"(.*)", processPatchRequests);
        svr.Delete(R"(.*)", processDeleteRequests);
        loginfo("Starting to listen at port",config["port"].asInteger());
        if (!svr.listen("localhost", config["port"].asInteger()))
        {
            logerror(__FILE__,"row",__LINE__,"Failed to listen at port",config["port"].asInteger(),"is it in use ?");
        }
        svr.stop(); 
    }
    void runAsAction(jvar &actionRequestBody)
    {
        setConfig(actionRequestBody);
        httplib::Request req;
        httplib::Response res;
        req.method = actionRequestBody["__ow_method"].asString();
        std::multimap<std::string, std::string> params;
        for(jvar &field : actionRequestBody["__ow_headers"].entries())
        {
            std::string fieldName = field[ZERO].asString();
            if (fieldName=="x-forwarded-uri")
            {
                jvar &fieldVal = field[1];
                req.path = fieldVal.substr(0,fieldVal.indexOf("?")).asString();
            }
            req.set_header(fieldName.c_str(),field[1].asString());
        }
        for(jvar &field : actionRequestBody.entries())
        {
            std::string fieldName = field[ZERO].asString();
            if (fieldName.find("__ow")==std::string::npos)
                params.emplace(fieldName,field[1].asString());
        }
        req.params = params;
        std::transform(req.method.begin(), req.method.end(), req.method.begin(),
        [](unsigned char c){ return std::tolower(c); });
        if (req.method=="options")
            processOptions(req,res);
        if (req.method=="get")
            processGetRequests(req,res);
        if (req.method=="post")
            processPostRequests(req,res);
        if (req.method=="patch")
            processPatchRequests(req,res);
        if (req.method=="delete")
            processDeleteRequests(req,res);
        
         jvar resultHeaders = ja{};
        for (auto it = res.headers.begin(); it != res.headers.end(); ++it)
        {
            std::string headerName = it->first;
            std::transform(headerName.begin(), headerName.end(), headerName.begin(),
            [](unsigned char c){ return std::tolower(c); });

            resultHeaders[headerName] = it->second;
        }

        resultHeaders["details"] = actionRequestBody;
        
        jvar result =  jo{
            "statusCode" << jv res.status,
            "body" << jv res.body,
            "headers" << resultHeaders
        };
        loginfo("{\"Action Output\":",result,"}");
        
    }
};

MyApp app;

size_t MyApp::desiredIdleMemory= DEFAULT_DESIRED_IDLE_MEMORY ;
size_t MyApp::desiredReleaseThreshold= DEFAULT_DESIRED_RELEASE_THRESHOLD ;
double MyApp::timeToReleaseMemory= DEFAULT_TIME_TO_CHECK_RELEASE_MEMORY ;
std::mutex MyApp::memoryControllerMutex;
/*handles freeing memory to OS when app goes above threshold in bursts*/
void memoryFree()
{
    while(1)
    {
        auto currentMemory = getCurrentRSS();
        size_t desiredMemory = MyApp::getDesiredIdleMemory();
        size_t threshold =  MyApp::getDesiredReleaseThreshold();
        if (currentMemory > threshold)
        {
            malloc_trim(desiredMemory);
            loginfo("Idle Memory,(",currentMemory/1048576,"mb ) above desired threshold(",desiredMemory/1048576,"mb ) Trying to give Memory Back to OS");
            
        }
        std::this_thread::sleep_for(std::chrono::duration<double>(MyApp::getTimeToReleaseMemory()));
    }
}

void processOptions(const httplib::Request& req, httplib::Response& res) 
{
    jvar ctx;
    ctx["filter"] = jo{
        "msg" << jv ""
    };
    ctx["config"] = app.config;
    auto &appFilters = app.getFilters();
    bool result;
    for(auto &filter : appFilters)
    {
        result = filter(req,res, ctx);
        if (!result)
        {
            res.set_content(ctx["filter"],"application/json");
            res.status = ctx["filter"]["httpstatus"];
            return;
        }
    }
    res.status = 200;
    res.set_content("{\"s\":true}","application/json");
}
//std::transform(data.begin(), data.end(), data.begin(),
//    [](unsigned char c){ return std::tolower(c); });

void processRequests(const httplib::Request& req, httplib::Response& res, Route & r) 
{
    mtimer t1;
    TimeUtil::startTimer(t1);
    MDC::clear();
    
    auto &filters = r.getFilters();
    auto &appFilters = app.getFilters();
    jvar ctx;
    ctx["filter"] = jo{
        "msg" << jv ""
    };
    ctx["config"] = app.config;
    for (auto it = req.headers.begin(); it != req.headers.end(); ++it)
    {
        std::string headerName = it->first;
        std::transform(headerName.begin(), headerName.end(), headerName.begin(),
        [](unsigned char c){ return std::tolower(c); });

        ctx["requestHeaders"][headerName] = it->second;
    }
    for (auto it = req.params.begin(); it != req.params.end(); ++it)
        ctx["params"][it->first] = urlDecode(it->second);
    MDC::put("path",req.path);
    MDC::put("method",req.method);
    MDC::put("body",req.body);
    //MDC::put("headers",ctx["requestHeaders"]);
    MDC::put("params",ctx["params"]);
    loginfo("Request Received");
    bool result;
    for(auto &filter : appFilters)
    {
        result = filter(req,res, ctx);
        if (!result)
        {
            res.set_content(ctx["filter"],"application/json");
            res.status = ctx["filter"]["httpstatus"];
            return;
        }
    }
    for(auto &filter : filters)
    {
        result = filter(req,res, ctx);
        if (!result)
        {
            res.set_content(ctx["filter"],"application/json");
            res.status = ctx["filter"]["httpstatus"];
            return;
        }
    }
    loginfo("Past Filter, Calling Controller");
    res.status = 200; 
    jvar output = std::move(r.processRequest(req, res, ctx));
    jvar ctxStatus = ctx["status"];
    if (!output["httpstatus"].isNull())
        res.status = output["httpstatus"];
    if (ctxStatus)
        res.status = ctxStatus;
    if (ctx["responseHeaders"]["Content-Type"])
        res.set_content(output,ctx["responseHeaders"]["Content-Type"].asString().c_str());
    else
        res.set_content(output,"application/json"); 
    TimeUtil::finishTimer(t1);
}

void processGetRequests(const httplib::Request& req, httplib::Response& res) 
{
    try{
        Route &r = app.getRoutes.at(req.path);
        processRequests(req,res,r);
    }catch(std::out_of_range e)
    {
        jvar json404;
        json404["msg"] = "404: API Path not found";
        res.set_content(json404.toJson(),"application/json");
        res.status = 404;
    }
}

void processPostRequests(const httplib::Request& req, httplib::Response& res) 
{
    try{
        Route &r = app.postRoutes.at(req.path);
        processRequests(req,res,r);
    }catch(std::out_of_range e)
    {
        jvar json404;
        json404["msg"] = "404: API Path not found";
        res.set_content(json404.toJson(),"application/json");
        res.status = 404;
    }
}

void processPatchRequests(const httplib::Request& req, httplib::Response& res) 
{
    try{
        Route &r = app.patchRoutes.at(req.path);
        processRequests(req,res,r);
    }catch(std::out_of_range e)
    {
        jvar json404;
        json404["msg"] = "404: API Path not found";
        res.set_content(json404.toJson(),"application/json");
        res.status = 404;
    }
}

void processDeleteRequests(const httplib::Request& req, httplib::Response& res) 
{
    try{
        Route &r = app.deleteRoutes.at(req.path);
        processRequests(req,res,r);
    }catch(std::out_of_range e)
    {
        jvar json404;
        json404["msg"] = "404: API Path not found";
        res.set_content(json404.toJson(),"application/json");
        res.status = 404;
    }
}