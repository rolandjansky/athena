/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "POOLCore/SystemTools.h"

#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"

#include "POOLCore/DbPrint.h"

#include <sys/stat.h>
#include <cstdlib>
#include <sstream>
#include <libgen.h>
#include <string.h>

using namespace pool;

namespace pool
{
   const char* SystemTools::GetEnv(const char* key){
      return getenv(key);
   }
  
   std::string SystemTools::GetEnvStr(const std::string& key) {
      const char *var = GetEnv( key.c_str() );
      return var? std::string(var) : std::string();
   }
   
   bool SystemTools::FileExists(const char* filename){
      struct stat fs;
      if(stat(filename, &fs) != 0) {
         return false;
      }else{
         return true;
      }
   }

   std::string SystemTools::FCBasename(const std::string& path) {
      size_t dl_pos = path.find_last_of("/\\");
      if( dl_pos == std::string::npos )
         // delimiter not found
         return path;
      return path.substr(dl_pos+1);
   }
   
   std::string SystemTools::itostr(const int i){
      std::ostringstream ost;
      ost<<i;
      return ost.str();
   }


   static MSG::Level OutputLevel = MSG::NIL;
   
   MSG::Level SystemTools::SetOutputLvl(MSG::Level new_ol) {
      MSG::Level old_ol = OutputLevel;
      OutputLevel = new_ol;
      return old_ol;
   }

   
   MSG::Level SystemTools::GetOutputLvl()
   {
      // Priority to the ENV setting so debugging is easy
      static MSG::Level ol = GetOutputLvlFromEnv();
      if( ol != MSG::NIL )
         return ol;
      // Next programatically set level
      if( OutputLevel!= MSG::NIL )
         return OutputLevel;
      // Last the default
      return MSG::WARNING;
   }

   
   MSG::Level SystemTools::GetOutputLvlFromEnv()
   {
      const char *msg_var = getenv( "POOL_OUTMSG_LEVEL" );
      // Check only the first char of the environment variable
      if( msg_var ) switch( *msg_var ) {
            // NOTE:  NIL is used as 'no setting'            
          case 0  :
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
   return MSG::NIL; 
   }

   
   bool SystemTools::initGaudi()   {
      // Create an instance of an application manager
      IInterface* iface = Gaudi::createApplicationMgr();
      if( !iface ) {
         DbPrint log("APR.initGaudi");
         log << DbPrintLvl::Warning << "Gaudi framework failed to initialize" << endmsg;
         return false;
      }
      SmartIF<ISvcManager> svcMgr(iface);
      SmartIF<IAppMgrUI> appMgr(iface);
      SmartIF<IProperty> propMgr(iface);
      SmartIF<ISvcLocator> svcLoc(iface);

      propMgr->setProperty( "JobOptionsType", "NONE" ) .ignore();
      // prevents some output from the AppMgr
      propMgr->setProperty( "OutputLeveL", "5" ) .ignore();
      // this prevents AppMgr "welcome" output
      propMgr->setProperty( "AppName", "" ) .ignore();

      if( !appMgr->configure().isSuccess() || !appMgr->initialize().isSuccess() ) {
         std::cerr << "Gaudi ApplicationMgr failed to initialize" << std::endl;
         return false;
      }
      return true;
   }

} // ns pool


