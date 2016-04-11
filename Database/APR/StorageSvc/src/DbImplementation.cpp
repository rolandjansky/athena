/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbImplementation.cpp 726071 2016-02-25 09:23:05Z krasznaa $
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

