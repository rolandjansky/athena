/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_SYSTEMTOOLS_H
#define POOL_SYSTEMTOOLS_H

#include "GaudiKernel/IMessageSvc.h"

namespace pool{
  
  /** @class SystemTools SystemTools.h POOLCore/SystemTools.h
      Misc utility methods
  */
  class SystemTools
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

    // read the PFC_MSGLEVEL env setting
    static MSG::Level GetOutputLvlFromEnv();

    // initialize the Gaudi framework for standalone executables
    static bool initGaudi();
  };
} // ns pool
#endif
