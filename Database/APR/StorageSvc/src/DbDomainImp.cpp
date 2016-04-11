/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbDomainImp.cpp 726071 2016-02-25 09:23:05Z krasznaa $
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
