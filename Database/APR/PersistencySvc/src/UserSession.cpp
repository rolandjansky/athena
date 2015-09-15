/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "UserSession.h"
#include "PersistencySvcConfiguration.h"
#include "GlobalTransaction.h"
#include "DatabaseRegistry.h"
#include "TechnologyDispatcher.h"
#include "UserDatabase.h"
#include "DatabaseHandler.h"
#include "MicroSessionManager.h"

pool::PersistencySvc::UserSession::UserSession( pool::IFileCatalog& fileCatalog ):
  m_configuration( 0 ),
  m_registry( 0 ),
  m_transaction( 0 ),
  m_technologyDispatcher( 0 )
{
  m_configuration = new pool::PersistencySvc::PersistencySvcConfiguration;
  m_registry = new pool::PersistencySvc::DatabaseRegistry( *m_configuration );
  m_transaction = new pool::PersistencySvc::GlobalTransaction( *m_registry );
  m_technologyDispatcher = new pool::PersistencySvc::TechnologyDispatcher( *m_registry, *m_transaction );
  m_configuration->setFileCatalog( fileCatalog );
}

pool::PersistencySvc::UserSession::~UserSession()
{
  delete m_technologyDispatcher;
  delete m_transaction;
  delete m_registry;
  delete m_configuration;
}

pool::PersistencySvc::PersistencySvcConfiguration&
pool::PersistencySvc::UserSession::configuration()
{
  return *m_configuration;
}

const pool::PersistencySvc::PersistencySvcConfiguration&
pool::PersistencySvc::UserSession::configuration() const
{
  return *m_configuration;
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
  m_configuration->setDatabaseConnectionPolicy( policy );
}

const pool::DatabaseConnectionPolicy&
pool::PersistencySvc::UserSession::defaultConnectionPolicy() const
{
  return m_configuration->databaseConnectionPolicy();
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
  for ( pool::PersistencySvc::DatabaseRegistry::const_iterator iDb = m_registry->begin();
        iDb != m_registry->end(); ++iDb ) {
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
                                                   *m_configuration,
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
  return m_configuration->fileCatalog();
}

const pool::ITechnologySpecificAttributes&
pool::PersistencySvc::UserSession::technologySpecificAttributes( long technology ) const
{
  return static_cast< const pool::ITechnologySpecificAttributes& >( m_technologyDispatcher->microSessionManager( technology ) );
}

pool::ITechnologySpecificAttributes&
pool::PersistencySvc::UserSession::technologySpecificAttributes( long technology )
{
  return static_cast< pool::ITechnologySpecificAttributes& >( m_technologyDispatcher->microSessionManager( technology ) );
}
