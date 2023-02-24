#ifndef UNET_UTILS_H
#define UNET_UTILS_H

#include "./httplib.h"
#include "./jvar.h"
#include "./UWebStringUtils.h"


jvar sendRequest(enum httpmethod method,std::string domain, std::string path, jvar body=MNULL, jvar headersArg=MNULL,bool parseJson=true);
jvar sendRequest(enum httpmethod method,std::string url, jvar body=MNULL, jvar headersArg=MNULL,bool parseJson=true);
jvar sendRequest(jvar route,jvar body=MNULL, jvar additionalHeaders=MNULL);

#endif
