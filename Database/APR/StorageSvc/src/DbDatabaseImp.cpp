/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbDatabaseImp.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//
//====================================================================

/// Framework include files
#include "StorageSvc/DbDatabaseImp.h"
#include "StorageSvc/DbInstanceCount.h"
using namespace pool;

static DbInstanceCount::Counter* s_count = DbInstanceCount::getCounter(typeid(DbDatabaseImp));

/// Standard Constructor
DbDatabaseImp::DbDatabaseImp(IOODatabase* idb)  : DbImplementation(idb)
{  s_count->increment();                                                }

/// Standard Destructor
DbDatabaseImp::~DbDatabaseImp()
{  s_count->decrement();                                                }

/// Callback after successful open of a database object
DbStatus DbDatabaseImp::onOpen(const DbDatabase& /* dbH  */,DbAccessMode /* mode */)
{  return Success;                                                      }

/// Re-open database with changing access permissions
DbStatus DbDatabaseImp::reopen(DbAccessMode /* mode */)
{  return Error;                                                        }

/// Access options
DbStatus DbDatabaseImp::getOption(DbOption& /* opt */) const
{  return Error;                                                        }

/// Set options
DbStatus DbDatabaseImp::setOption(const DbOption& /* opt */)
{  return Success;                                                      }

/// Execute Database Transaction action
DbStatus DbDatabaseImp::transAct(Transaction::Action /*action*/)
{  return Success;                                                      }
