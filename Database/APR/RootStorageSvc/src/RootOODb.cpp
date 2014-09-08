/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//        Root Database factories implementation
//--------------------------------------------------------------------
//
//        Package    : POOLCore/RootDb (The POOL project)
//
//        Author     : M.Frank
//====================================================================
#include "RootOODb.h"
#include "RootDatabase.h"
#include "RootDomain.h"
#include "RootClassLoader.h"
#include "RootKeyContainer.h"
#include "RootTreeContainer.h"
#include "StorageSvc/DbInstanceCount.h"

#include <memory>

using namespace pool;

static RootClassLoader* _classLoader()  {
  static std::auto_ptr<RootClassLoader> ldr;
  if ( 0 == ldr.get() )  {
    ldr = std::auto_ptr<RootClassLoader>(new RootClassLoader);
  }
  return ldr.get();
}

#ifndef HAVE_GAUDI_PLUGINSVC
//namespace pool {  void* createLoader()  {    return _classLoader();  }    }
#endif

/// Standard Constructor
RootOODb::RootOODb(void* ctxt) : OODatabaseImp(ctxt, ROOT_StorageType)  {
  DbInstanceCount::increment(this);
  setClassLoader(_classLoader());
}

/// Standard Constructor
RootOODb::RootOODb(void* ctxt, DbType typ) : OODatabaseImp(ctxt, typ)   {
  DbInstanceCount::increment(this);
  setClassLoader(_classLoader());
}

/// Standard Destructor
RootOODb::~RootOODb()  {
  DbInstanceCount::decrement(this);
}

/// Create Root Domain object
IDbDomain* RootOODb::createDomain()  {
  return new RootDomain(this);        
}

/// Create Root Database object (TFile)
IDbDatabase* RootOODb::createDatabase()  {
  return new RootDatabase(this);         
}

/// Create Root Container object
IDbContainer* RootOODb::createContainer(const DbType& typ) { 
  if ( typ.match(ROOTKEY_StorageType) )  {
    return new RootKeyContainer(this);
  }
  else if ( typ.match(ROOTTREE_StorageType) )    {
    return new RootTreeContainer(this);
  }
  else if ( typ.match(ROOT_StorageType) )    {
    return new RootTreeContainer(this);
  }
  return 0;
}
