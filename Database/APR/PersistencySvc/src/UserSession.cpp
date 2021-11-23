/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "UserSession.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "GlobalTransaction.h"
#include "DatabaseRegistry.h"
#include "TechnologyDispatcher.h"
#include "UserDatabase.h"
#include "DatabaseHandler.h"
#include "MicroSessionManager.h"

pool::PersistencySvc::UserSession::UserSession( pool::IFileCatalog& fileCatalog ):
  m_policy( 0 ),
  m_catalog( &fileCatalog ),
  m_registry( 0 ),
  m_transaction( 0 ),
  m_technologyDispatcher( 0 )
{
  m_policy = new pool::DatabaseConnectionPolicy;
  m_registry = new pool::PersistencySvc::DatabaseRegistry();
  m_transaction = new pool::PersistencySvc::GlobalTransaction( *m_registry );
  m_technologyDispatcher = new pool::PersistencySvc::TechnologyDispatcher( *m_registry, *m_transaction );
}

pool::PersistencySvc::UserSession::~UserSession()
{
  delete m_technologyDispatcher;
  delete m_transaction;
  delete m_registry;
  delete m_policy;
}

pool::PersistencySvc::DatabaseRegistry&
pool::PersistencySvc::UserSession::registry()
{
  return *m_registry;
}

pool::PersistencySvc::TechnologyDispatcher&
pool::PersistencySvc::UserSession::technologyDispatcher()
{
  return *m_technologyDispatcher;
}

void
pool::PersistencySvc::UserSession::setDefaultConnectionPolicy( const pool::DatabaseConnectionPolicy& policy )
{
  *m_policy = policy;
}

const pool::DatabaseConnectionPolicy&
pool::PersistencySvc::UserSession::defaultConnectionPolicy() const
{
  return *m_policy;
}

void
pool::PersistencySvc::UserSession::disconnectAll()
{
  m_technologyDispatcher->disconnectAll();
}
      
pool::ITransaction&
pool::PersistencySvc::UserSession::transaction()
{
  return static_cast<pool::ITransaction&>( *m_transaction );
}

const pool::ITransaction&
pool::PersistencySvc::UserSession::transaction() const
{
  return static_cast<const pool::ITransaction&>( *m_transaction );
}

std::vector< std::string >
pool::PersistencySvc::UserSession::connectedDatabases() const
{
  std::vector< std::string > result;
  // Make sure we call const methods.
  const DatabaseRegistry* registry = m_registry;
  for ( pool::PersistencySvc::DatabaseRegistry::const_iterator iDb = registry->begin();
        iDb != registry->end(); ++iDb ) {
    result.push_back( (*iDb)->fid() );
  }
  return result;
}

pool::IDatabase*
pool::PersistencySvc::UserSession::databaseHandle( const std::string& dbName,
                                                   DatabaseSpecification::NameType dbNameType )
{
  if ( m_transaction->isActive() ) {
    return new pool::PersistencySvc::UserDatabase( *m_technologyDispatcher,
                                                   *m_policy,
                                                   *m_catalog,
                                                   *m_transaction,
                                                   *m_registry,
                                                   dbName,
                                                   dbNameType );
  }
  return 0;
}

pool::ITransaction&
pool::PersistencySvc::UserSession::globalTransaction()
{
  return static_cast< pool::ITransaction& >( *m_transaction );
}

pool::IFileCatalog&
pool::PersistencySvc::UserSession::fileCatalog()
{
  return *m_catalog;
}

void
pool::PersistencySvc::UserSession::setFileCatalog(pool::IFileCatalog& catalog)
{
  m_catalog = &catalog;
}

const pool::ITechnologySpecificAttributes&
pool::PersistencySvc::UserSession::technologySpecificAttributes( long technology ) const
{
  // Make sure we call the const version of microSesssionManager().
  const TechnologyDispatcher* disp = m_technologyDispatcher;
  return static_cast< const pool::ITechnologySpecificAttributes& >( disp->microSessionManager( technology ) );
}

pool::ITechnologySpecificAttributes&
pool::PersistencySvc::UserSession::technologySpecificAttributes( long technology )
{
  return static_cast< pool::ITechnologySpecificAttributes& >( m_technologyDispatcher->microSessionManager( technology ) );
}
