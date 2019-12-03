/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbSessionObj.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbSessionObj object implementation
//--------------------------------------------------------------------
//
//  Package    : System (The POOL project)
//
//  Description: Generic data persistency
//
//  @author      M.Frank
//====================================================================
// Framework include files
#include "DbSessionObj.h"
#include "DbDomainObj.h"
#include "POOLCore/DbPrint.h"
#include "StorageSvc/IOODatabase.h"

#include "Gaudi/PluginService.h"

using namespace pool;

// Standard Constructor
DbSessionObj::DbSessionObj()
: Base("DbSession", pool::READ, POOL_StorageType)
{
}

// Standard Destructor
DbSessionObj::~DbSessionObj()  {
  clearEntries();
  for( auto& i : m_dbTypes )  releasePtr( i.second );
}

// Open session
DbStatus DbSessionObj::open()   {
  DbPrint log( "DbSession");
  log << DbPrintLvl::Info << "    Open     DbSession    " << DbPrint::endmsg;
  return Success;
}

// close session
DbStatus DbSessionObj::close()   {
  DbPrint log( "DbSession");
  log << DbPrintLvl::Info << "    Closed   DbSession    " << DbPrint::endmsg;
  return Success;
}

// Access different implementations
IOODatabase* DbSessionObj::db(const DbType& typ) {
   if( m_dbTypes[typ] == 0 ) {
      const std::string nam = typ.storageName();
      IOODatabase* imp = Gaudi::PluginService::Factory<IOODatabase*()>::create(nam).release();
      if( imp )  {
         m_dbTypes[typ] = imp;
      } else {
         DbPrint log( "DbSession");
         log << DbPrintLvl::Fatal << "Failed to load plugin for " << nam << " storage type" << DbPrint::endmsg;
      }
   }
   return m_dbTypes[typ] ;
}
