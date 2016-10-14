/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCSystemTools.h 778462 2016-10-14 12:29:52Z mnowak $
#ifndef POOL_FCSYSTEMTOOLS_H
#define POOL_FCSYSTEMTOOLS_H

#include "GaudiKernel/IMessageSvc.h"

namespace pool{
  
  /** @class FCSystemTools  FCSystemTools.h FileCatalog/FCSystemTools.h
      FileCatalog Utility methods
      @author Zhen Xie
  */
  class FCSystemTools
  {
  public:
    /** Read an environment variable.  */
    static const char* GetEnv(const char* key);
    static std::string GetEnvStr(const std::string& key);
    
   /** Check if a file exists.  */
    static bool FileExists(const char* filename);
  
    static std::string itostr(const int i);

    // get base component of the FC connection string
    static std::string FCBasename(const std::string& cat);
    
    // return the output level for PFC
    static MSG::Level GetOutputLvl();

    // set PFC output level (env setting still has priority)
    static MSG::Level SetOutputLvl(MSG::Level new_lvl);

    // read the PFC_MSGLEVEL env setting
    static MSG::Level GetOutputLvlFromEnv();
  };
} // ns pool
#endif
