/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// $Id: OODatabaseImp.cpp 590734 2014-04-01 21:49:36Z gemmeren $
//====================================================================
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//
//====================================================================

/// Framework include files
#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DbClassLoader.h"
#include "StorageSvc/DbInstanceCount.h"
#include "StorageSvc/OODatabaseImp.h"

using namespace pool;

// Standard Constructor
OODatabaseImp::OODatabaseImp(void* ctxt, const DbType& typ)
: m_refCount(0), 
  m_type(typ),
  m_name(""),
  m_classLoader(0),
  m_context(ctxt)
{
  DbInstanceCount::increment(this);
  setClassLoader(new DbClassLoader());
}

// Standard destructor
OODatabaseImp::~OODatabaseImp()   {
  DbInstanceCount::decrement(this);
  releasePtr( m_classLoader );
}

// IInterface implementation: Decrease reference count
unsigned int OODatabaseImp::release()   {
  int cnt = --m_refCount;
  if ( cnt <= 0 )   {
    delete this;
  }
  return cnt;
}

// IInterface implementation: Query interface
DbStatus OODatabaseImp::queryInterface(const Guid& riid,void** ppvInterface)  {
  if ( riid == IOODatabase::interfaceID() )  {
    *ppvInterface = (IOODatabase*)this;
  }
  else   {
    return Error;
  }
  addRef();
  return Success;
}

// IOODatabase implementation: Initialize the Database
DbStatus OODatabaseImp::initialize(const std::string& nam)  {
  DbStatus status = Error;
  m_name   =  nam;
  m_name  += ".Database";
  status = Success;
  return status;
}

/// Set the class loader
void OODatabaseImp::setClassLoader(IClassLoader* loader) {
  releasePtr( m_classLoader );
  m_classLoader = loader;
}

/// Allow access to the class loader
IClassLoader* OODatabaseImp::classLoader() const  {
  return m_classLoader;
}
