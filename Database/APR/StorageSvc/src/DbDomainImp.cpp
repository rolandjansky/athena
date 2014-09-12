/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbDomainImp.cpp 590734 2014-04-01 21:49:36Z gemmeren $
//====================================================================
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//
//====================================================================

/// Framework include files
#include "StorageSvc/DbDomainImp.h"
#include "StorageSvc/DbInstanceCount.h"

using namespace pool;

/// Standard Constructor
DbDomainImp::DbDomainImp(IOODatabase* idb) : DbImplementation(idb)
{  DbInstanceCount::increment(this);                                    }

/// Standard Destructor
DbDomainImp::~DbDomainImp()
{  DbInstanceCount::decrement(this);                                    }

/// Open domain
DbStatus DbDomainImp::open( const DbSession&   /* sesH  */, 
                            const std::string& /* nam   */, 
                            DbAccessMode       /* mod   */)
{
  return Success;
}

/// Set domain specific options
DbStatus DbDomainImp::setOption(const DbOption&  /* refOpt */)
{  return Error;                                                        }

/// Access domain specific options
DbStatus DbDomainImp::getOption(DbOption&        /* refOpt */) const
{  return Error;                                                        }
