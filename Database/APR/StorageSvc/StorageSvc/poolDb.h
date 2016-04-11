/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: poolDb.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//
//  Package    : StorageSvc (The pool framework)
//
//  Description: Management of the object Databases
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_POOLDB_H
#define POOL_POOLDB_H 1

#include "StorageSvc/pool.h"

namespace poolDb   {
  using pool::releasePtr;
  using pool::deletePtr;
  using pool::debugBreak;
  using pool::DbInstanceCount;
  using pool::accessMode;
  using pool::NOT_OPEN;
  using pool::READ;
  using pool::UPDATE;
  using pool::CREATE;
  using pool::RECREATE;
  using pool::Success;
  using pool::Warning;
  using pool::Error;
  using pool::ConnTimeout;
}

#endif  // POOL_POOLDB_H
