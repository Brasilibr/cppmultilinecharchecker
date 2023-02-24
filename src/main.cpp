#include "../lib/ParallelLogger.hpp"
#include "main.h"
#include <fstream>
#include "../lib/fjwt.h"
#include <chrono>
#include <unistd.h>
#include <filesystem>


bool stringContainsMultiByteChar(jvar str, std::string fileName)
{
  int pos=0;
  bool insideString=false;
  bool justOpened=false;
  bool insideSingleQuote=false;
  bool justOpenedSingleQuote=false;
  int charsInside=0;
  int col=0;
  int row=1;
  bool skippingLine=false;
  bool skippingLineLarge=false;

  int lastSafeChar = str.size()-2;
  for(size_t index=0;index<str.size();index++)
  {
    if(index < lastSafeChar && str.charAt(index)=='/' && str.charAt(index+1)=='/' && insideString==false && insideSingleQuote==false)
    {
      skippingLine=true;
      //loginfo("Comment Line at ","row",row,"col",col);
    }
    if(index < lastSafeChar && str.charAt(index)=='/' && str.charAt(index+1)=='*'  && insideString==false && insideSingleQuote==false)
    {
      skippingLineLarge=true;
      //loginfo("Comment Line at ","row",row,"col",col);
    }
    if(str.charAt(index)=='\n')
    {
      row+=1;
      col=0;
      if (skippingLine)
        skippingLine=false;
    }
    if (skippingLine)
      continue;

    if(index < lastSafeChar && str.charAt(index)=='*' && str.charAt(index+1)=='/')
    {
      skippingLineLarge=false;
    }
    if (skippingLineLarge)
      continue;

    if (str.charAt(index)=='"' && insideString==false && insideSingleQuote==false)
    {
      insideString=true;
      //loginfo("inside string ","row",row,"col",col);
      justOpened=true;
    }
    if (index>0)
    {
      if (str.charAt(index)=='"' && insideString==true && justOpened==false)
      {
        if (
        ((str.charAt(index-1)=='\\' && str.charAt(index-2)=='\\') || (str.charAt(index-1)!='\\')) && 
        (!(str.charAt(index-1)=='\\' && str.charAt(index-2)=='\\' && str.charAt(index-3)=='\\') || 
        (str.charAt(index-1)=='\\' && str.charAt(index-2)=='\\' && str.charAt(index-3)=='\\' && str.charAt(index-4)=='\\'))
        )
        {
        insideString=false;
        //loginfo("outside string ","row",row,"col",col);
        }
      }
    }
    justOpened=false;
    col+=1;
    if (insideString)
    {
      //loginfo("inside string ","row",row,"col",col);
      continue;
    }

    if (str.charAt(index)=='\'' && insideSingleQuote==false)
    {
      //loginfo("opening single quote area");
      //loginfo("inside quote ","row",row,"col",col);
      insideSingleQuote=true;
      justOpenedSingleQuote=true;
    }


    if (index > 0 && str.charAt(index)=='\'' && insideSingleQuote==true && justOpenedSingleQuote==false)
    {
      if (
        ((str.charAt(index-1)=='\\' && str.charAt(index-2)=='\\') || (str.charAt(index-1)!='\\')) && 
        (!(str.charAt(index-1)=='\\' && str.charAt(index-2)=='\\' && str.charAt(index-3)=='\\') || 
        (str.charAt(index-1)=='\\' && str.charAt(index-2)=='\\' && str.charAt(index-3)=='\\' && str.charAt(index-4)=='\\'))
        )
      {
      insideSingleQuote=false;
      charsInside=0;
      //loginfo("outside quote ","row",row,"col",col);
      }
      
      //loginfo("closing single quote area");
    }
    
    if (insideSingleQuote && justOpenedSingleQuote==false)
    {
      charsInside+=1;
    }
    if (charsInside>4)
    {
      logerror("multibyte character detected at", fileName,"row",row,"col",col);
      return true;
    }
    justOpenedSingleQuote=false;
  }
  return false;
}

bool validateFolder(std::string path)
{
  for (const auto & entry : std::filesystem::directory_iterator(path))
  {
    std::error_code ec;
    jvar name = entry.path().c_str();
    if (std::filesystem::is_directory(entry.path(), ec))
    { 
        // Process a directory.
        jvar fileName = "";
        int pos =0;
        pos = name.lastIndexOf("/");
        //std::cout << "POS::" << pos << std::endl;
        if (pos==std::string::npos)
          pos = 0;
        else
         pos = pos+1;
        fileName = name.substr(pos,name.size());
        if (fileName=="." || fileName ==".." || fileName.charAt(0)=='.' || name.indexOf("/obj")!=std::string::npos)
          continue;
        std::cout << "Opening Dir: " << entry.path() << std::endl;
        if (validateFolder(entry.path())==false)
          return false;
    }else
    {
      if (name.indexOf(".c")!=std::string::npos)
      {
        //std::cout << "Validating: " << name << std::endl;
        std::ifstream t3(name.asString()); std::string str3((std::istreambuf_iterator<char>(t3)),std::istreambuf_iterator<char>());
        bool res = stringContainsMultiByteChar(str3,name);
        if (res==true)
        {
          //logerror("File Contains Multi Byte");
          return false;
        }
      }
      if (name.indexOf(".h")!=std::string::npos)
      {
        std::ifstream t4(name.asString()); std::string str4((std::istreambuf_iterator<char>(t4)),std::istreambuf_iterator<char>());
        bool res = stringContainsMultiByteChar(str4,name);
        if (res==true)
        {
          //logerror("File Contains Multi Byte");
          return false;
        }
      }
    }
  }
  return true;      
}

int main( int argc, char *argv[ ] )
{
  MDC::put("mainthread:",true);
  std::string folder = argv[1];
  std::cout << "Folder:" << folder << std::endl;
  bool res = validateFolder(folder);
  //jvar teste;
  //teste['testeeeeee'] = true;

  closeLogger();
  return res;
}
