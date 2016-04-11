/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbObjectGuard.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbObjectGuard handle implementation
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================

#include "StorageSvc/DbObjectGuard.h"
using namespace pool;

/// Destructor
DbObjectGuard::~DbObjectGuard()   {
}

/// Standard constructor
DbObjectGuard::DbObjectGuard(int type, int siz) 
: m_size(siz),
  m_container(type),
  m_oid(INVALID,INVALID) { 
}

/// Standard constructor
DbObjectGuard::DbObjectGuard(const DbContainer& cntH, int siz)
: m_size(siz),
  m_container(cntH),
  m_oid(INVALID,INVALID) { 
}
