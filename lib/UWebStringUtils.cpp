#include "UWebStringUtils.h"
#include <iostream>

std::string replaceTemplateVariables(jvar &variables, std::string templateString)
{
    jvar result = templateString;
    jvar paramList = variables.entries();
    for(jvar &pair : paramList)
    {
        jvar key = pair[ZERO];
        key = "{"+key+"}";
        jvar value = pair[1];
        result = result.replaceAll(key,value);
    }
    return result;
}

std::string urlEncode(std::string str){
    std::string new_str = "";
    char c;
    int ic;
    const char* chars = str.c_str();
    char bufHex[10];
    int len = str.length();

    for(int i=0;i<len;i++){
        c = chars[i];
        ic = c;
        // uncomment this if you want to encode spaces with +
        if (c==' ') new_str += '+';   
        else if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') new_str += c;
        else {
            sprintf(bufHex,"%X",c);
            if(ic < 16) 
                new_str += "%0"; 
            else
                new_str += "%";
            new_str += bufHex;
        }
    }
    return new_str;
 }

std::string urlDecode(std::string str){
    std::string ret;
    char ch;
    int i, len = str.length();
    unsigned int ii;

    for (i=0; i < len; i++){
        if(str[i] != '%'){
            if(str[i] == '+')
                ret += ' ';
            else
                ret += str[i];
        }else{
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
    }
    return ret;
}