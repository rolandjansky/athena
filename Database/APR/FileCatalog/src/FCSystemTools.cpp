/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileCatalog/FCSystemTools.h"
#include <sys/stat.h>
#include <cstdlib>
#ifndef POOL_URIPARSER_H
#include "FileCatalog/URIParser.h"
#endif
#include <sstream>
#include <libgen.h>
#include <string.h>

namespace pool
{

   const char* FCSystemTools::GetEnv(const char* key){
      return getenv(key);
   }
  
   std::string FCSystemTools::GetEnvStr(const std::string& key) {
      const char *var = GetEnv( key.c_str() );
      return var? std::string(var) : std::string();
   }
   
   bool FCSystemTools::FileExists(const char* filename){
      struct stat fs;
      if(stat(filename, &fs) != 0) {
         return false;
      }else{
         return true;
      }
   }

   std::string FCSystemTools::FCBasename(const std::string& path) {
      size_t dl_pos = path.find_last_of("/\\");
      if( dl_pos == std::string::npos )
         // delimiter not found
         return path;
      return path.substr(dl_pos+1);
   }
   
   std::string FCSystemTools::itostr(const int i){
      std::ostringstream ost;
      ost<<i;
      return ost.str();
   }


   static MSG::Level FCOutputLevel = MSG::NIL;
   
   MSG::Level FCSystemTools::SetOutputLvl(MSG::Level new_ol) {
      MSG::Level old_ol = FCOutputLevel;
      FCOutputLevel = new_ol;
      return old_ol;
   }

   
   MSG::Level FCSystemTools::GetOutputLvl()
   {
      // Priority to the ENV setting so debugging is easy
      MSG::Level ol = GetOutputLvlFromEnv();
      if( ol != MSG::NIL )
         return ol;
      if( FCOutputLevel!= MSG::NIL )
         return FCOutputLevel;
      return MSG::WARNING;    // This is the current default!
   }

   
   MSG::Level FCSystemTools::GetOutputLvlFromEnv()
   {
      if( getenv( "PFC_MSGLEVEL" ) )
      {
         // Check only the first char of the environment variable
         switch ( *getenv( "PFC_MSGLEVEL" ) )
         {
            // NOTE:  NIL is used as 'no setting'
          case '0':
          case 'n':
          case 'N': return MSG::NIL;
	
          case '1':
          case 'v':
          case 'V': return MSG::VERBOSE;
	
          case '2':
          case 'd':
          case 'D': return MSG::DEBUG;
	
          case '3':
          case 'i':
          case 'I': return MSG::INFO;
	
          case '4':
          case 'w':
          case 'W': return MSG::WARNING;
	
          case '5':
          case 'e':
          case 'E': return MSG::ERROR;
	
          case '6':
          case 'f':
          case 'F': return MSG::FATAL;
	
          case '7':
          case 'a':
          case 'A': return MSG::ALWAYS;
	
          default: break;
         }
      }
      return MSG::NIL;
   }


} // ns pool


