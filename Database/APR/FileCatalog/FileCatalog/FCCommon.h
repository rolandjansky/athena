/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_FCCOMMON_H
#define POOL_FCCOMMON_H
#include <string>
#include "FileCatalog/FCEntry.h"
namespace pool{
  namespace FileCatalog{
    typedef std::string FileID;
    enum CommitMode{REFRESH,ONHOLD};
    enum FileOpenMode{READ,WRITE,UPDATE};
    ///File Access Pattern
    enum FileAccessPattern{SEQUENTIAL,RANDOM,PRANDOM};
    ///MetaData update mode: allow deleting existing redundant field or not
    enum MetaUpdateMode{NO_DELETE, DELETE_REDUNDANT};
  }
}
#endif
