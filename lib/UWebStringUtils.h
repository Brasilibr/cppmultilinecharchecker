

#ifndef fStringUtils_h
#define fStringUtils_h
#include <string>
#include "jvar.h"

std::string urlDecode(std::string str);
std::string urlEncode(std::string str);
std::string replaceTemplateVariables(jvar &variables, std::string templateString);

#endif