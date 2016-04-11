/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// $Id: OODatabaseImp.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//
//====================================================================

/// Framework include files
#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DbInstanceCount.h"
#include "StorageSvc/OODatabaseImp.h"

using namespace pool;

// Standard Constructor
OODatabaseImp::OODatabaseImp(void* ctxt, const DbType& typ)
: m_refCount(0), 
  m_type(typ),
  m_name(""),
  m_context(ctxt)
{
  DbInstanceCount::increment(this);
}

// Standard destructor
OODatabaseImp::~OODatabaseImp()   {
  DbInstanceCount::decrement(this);
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

