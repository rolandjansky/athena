/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCSystemTools.h 457829 2011-09-08 13:13:25Z mnowak $
#ifndef POOL_FCSYSTEMTOOLS_H
#define POOL_FCSYSTEMTOOLS_H
#include <string>

namespace pool{
  
  class IFileCatalog;
  /** @class FCSystemTools  FCSystemTools.h FileCatalog/FCSystemTools.h
      FileCatalog Utility methods
      @author Zhen Xie
  */
  class FCSystemTools{
  public:
    /** Read an environment variable.  */
    static const char* GetEnv(const char* key);
    /** Check if a file exists.  */
    static bool FileExists(const char* filename);
    /** select and load FC implementation using plugin manager according to contact string*/
    //static IFileCatalog* createCatalog(const std::string& uriprefix);
    static std::string itostr(const int i);
  };
} // ns pool
#endif
