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

namespace pool{
  const char* FCSystemTools::GetEnv(const char* key){
    return getenv(key);
  }
  
  bool FCSystemTools::FileExists(const char* filename){
    struct stat fs;
    if(stat(filename, &fs) != 0) {
      return false;
    }else{
      return true;
    }
  }
  std::string FCSystemTools::itostr(const int i){
    std::ostringstream ost;
    ost<<i;
    return ost.str();
  }

} // ns pool
