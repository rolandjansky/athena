/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdio>
#include "DatabaseRegistry.h"
#include "DatabaseHandler.h"

pool::PersistencySvc::DatabaseRegistry::DatabaseRegistry()
{}


pool::PersistencySvc::DatabaseRegistry::~DatabaseRegistry()
{}

std::set< pool::PersistencySvc::DatabaseHandler* >::iterator
pool::PersistencySvc::DatabaseRegistry::registerDbHandler( pool::PersistencySvc::DatabaseHandler* dbHandler )
{
  std::set< pool::PersistencySvc::DatabaseHandler* >::iterator iDatabase = m_databases.find( dbHandler );
  if ( iDatabase == m_databases.end() ) {
    iDatabase = m_databases.insert( dbHandler ).first;
    m_fidToDb.insert( std::make_pair( dbHandler->fid(), dbHandler ) );
    const pool::DbDatabase::Redirections& redirections = dbHandler->redirections();
    for (pool::DbDatabase::Redirections::const_iterator iter = redirections.begin(), iterEnd = redirections.end(); iter != iterEnd; ++iter) {
        m_fidToDb.insert( std::make_pair( iter->first, dbHandler ) );
    }
    m_pfnToDb.insert( std::make_pair( dbHandler->pfn(), dbHandler ) );
    m_fidToLfns.insert( std::make_pair( dbHandler->fid(),
                                        std::set< std::string >() ) );
  }
  return iDatabase;
}

void
pool::PersistencySvc::DatabaseRegistry::registerDatabaseHandler( pool::PersistencySvc::DatabaseHandler* dbHandler )
{
  this->registerDbHandler( dbHandler );
}


void
pool::PersistencySvc::DatabaseRegistry::registerDatabaseHandler( pool::PersistencySvc::DatabaseHandler* dbHandler,
                                                                 const std::string& lfn )
{
  std::set< pool::PersistencySvc::DatabaseHandler* >::iterator iDatabase = this->registerDbHandler( dbHandler );
  const std::string& fid = (*iDatabase)->fid();
  m_fidToLfns.find( fid )->second.insert( lfn );
  m_lfnToDb.insert ( std::make_pair( lfn, *iDatabase ) );
}


void
pool::PersistencySvc::DatabaseRegistry::deregisterDatabaseHandler( pool::PersistencySvc::DatabaseHandler* dbHandler )
{
  std::set< pool::PersistencySvc::DatabaseHandler* >::iterator idb = m_databases.find( dbHandler );
  if ( idb != m_databases.end() ) {
    m_fidToDb.erase( (*idb)->fid() );
    const pool::DbDatabase::Redirections& redirections = dbHandler->redirections();
    for (pool::DbDatabase::Redirections::const_iterator iter = redirections.begin(), iterEnd = redirections.end(); iter != iterEnd; ++iter) {
        m_fidToDb.erase( iter->first );
    }
    m_pfnToDb.erase( (*idb)->pfn() );

    std::map< std::string, std::set< std::string > >::iterator iLfnSet = m_fidToLfns.find( (*idb)->fid() );
    if ( iLfnSet != m_fidToLfns.end() ) {
      for ( std::set< std::string >::iterator iLfn = iLfnSet->second.begin();
            iLfn != iLfnSet->second.end(); ++iLfn ) {
        m_lfnToDb.erase( *iLfn );
      }
      m_fidToLfns.erase( iLfnSet );
    }

    m_databases.erase( idb );
  }
}
