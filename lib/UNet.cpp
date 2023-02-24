#include "./UNet.h"
#include <iostream>

jvar sendRequest(enum httpmethod method,std::string url,jvar body, jvar headersArg, bool parseJson)
{
    jvar url2 = url;
    jvar sep = url2.split("/");
    if (sep.size()==0)
        return sendRequest(method,url,"/",body,headersArg);
    
    std::string domain; 
    std::string path = "/";
    int startPath = 3;

    if (sep[ZERO].indexOf("http")==std::string::npos)
    {
        domain = sep[ZERO].asString();
        startPath=1;
    }else
        domain = (sep[ZERO] + "//" + sep[2]).asString();
    for (size_t i = startPath; i < sep.size(); i++)
        path += (sep[i] + "/").asString();
    
    
    if (path.size()>1)
        path = path.substr(0,path.length()-1);
    return sendRequest(method,domain,path,body,headersArg,parseJson);
}

jvar sendRequest(enum httpmethod method,std::string domain, std::string path,jvar body, jvar headersArg, bool parseJson)
{
    jvar result;
    httplib::Client cli(domain.c_str());
    cli.set_follow_location(true);
    cli.set_connection_timeout(20, 0);
    cli.set_read_timeout(20, 0); 
    cli.set_write_timeout(20, 0);
    

    httplib::Result res({nullptr, httplib::Error()});
    httplib::Headers headers;
    jvar headerEntries = headersArg.entries();
    for(auto &item : headerEntries)
        headers.emplace(item[ZERO].asString(),item[1].asString());
    switch (method)
    {
        case HGET:{ 
            std::string params="?";
            jvar paramEntries = body.entries();
            for(auto &item : paramEntries)
                params += urlEncode(item[ZERO]) + "=" + urlEncode(item[1]) = "&";
            
            params=params.substr(0,params.size()-1);
            res = cli.Get((path+params).c_str(),headers);
            break; 
        }
        case HPOST: res = cli.Post(path.c_str(),headers,body.asString(),"application/json"); break;
        case HPATCH: res = cli.Patch(path.c_str(),headers,body.asString(),"application/json"); break;
        case HDELETE: res = cli.Delete(path.c_str(),headers,body.asString(),"application/json"); break;
        default:
            return jo{
                "ok" << jv false,
                "msg" << jv "Unable to do request, invalid METHOD",
                "httpstatus" << jv 500
            };
    }
    if(res)
    {
        result["httpstatus"] = res->status;
        if (res->status==200)
            result["ok"]=true;
        else
            result["ok"]=false;

        if(parseJson)
            result["data"] = jvar::parseJson(res->body);
        else
            result["data"] = res->body;

        return result;
    }

    return jo{
        "ok" << jv false,
        "msg" << jv "Unable to do request",
        "httpstatus" << jv 500
    };
}


jvar sendRequest(jvar route,jvar body, jvar additionalHeaders)
{
    if (route==MNULL)
    {
        return jo{
            "ok" << jv false,
            "msg" << jv "Route is Undefined, check your routes file"
        };
    }
    jvar allHeaders =  jo{};
    allHeaders << route["headers"];
    allHeaders << additionalHeaders;
   
    enum httpmethod methodEn = HUNDEFINED;

    jvar method = route["method"].toLowerCase();
    if (method=="get")
        methodEn=HGET;
    if (method=="post")
        methodEn=HPOST;
    if (method=="patch")
        methodEn=HPATCH;
    if (method=="delete")
        methodEn=HDELETE;
    if (methodEn==HUNDEFINED)
        return jo{
            "ok" << jv false,
            "msg" << jv "Invalid Method, only get,post,patch,delete allowed",
            "methodPassed" << method
        };
    return sendRequest(methodEn,route["domain"].asString(),route["path"].asString(),body,allHeaders,route["parseJson"]);
}