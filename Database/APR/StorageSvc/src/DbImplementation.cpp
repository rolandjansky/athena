/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbImplementation.cpp 458019 2011-09-09 10:11:57Z mnowak $
//====================================================================
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//
//====================================================================

/// Framework include files
#include "StorageSvc/pool.h"
#include "StorageSvc/IOODatabase.h"
#include "StorageSvc/DbInstanceCount.h"
#include "StorageSvc/DbImplementation.h"
using namespace pool;

static DbInstanceCount::Counter* s_count = 
  DbInstanceCount::getCounter(typeid(DbImplementation));

// Standard Constructor
DbImplementation::DbImplementation(IOODatabase* idb) : m_ooDB(idb)  {
  s_count->increment();
  if ( m_ooDB   )  {
    m_ooDB->addRef();
  }
}

// Standard destructor
DbImplementation::~DbImplementation()   {
  s_count->decrement();
  releasePtr( m_ooDB );
}

/// Access to the class loader
IClassLoader* DbImplementation::loader() const {
  return db() ? db()->classLoader() : 0; 
}
