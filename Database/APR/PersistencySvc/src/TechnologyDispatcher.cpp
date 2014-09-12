/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TechnologyDispatcher.h"
#include "MicroSessionManager.h"
#include "StorageSvc/DbType.h"
#include "PersistencySvc/PersistencySvcException.h"

pool::PersistencySvc::TechnologyDispatcher::TechnologyDispatcher( pool::PersistencySvc::DatabaseRegistry& registry,
                                                                  pool::ITransaction& transaction ):
  m_registry( registry ),
  m_transaction( transaction ),
  m_technologyTypes(),
  m_technologyNames()
{}


pool::PersistencySvc::TechnologyDispatcher::~TechnologyDispatcher()
{
  for ( std::map< long, pool::PersistencySvc::MicroSessionManager* >::iterator iManager = m_technologyTypes.begin();
        iManager != m_technologyTypes.end(); ++iManager ) {
    delete iManager->second;
  }
}


pool::PersistencySvc::MicroSessionManager&
pool::PersistencySvc::TechnologyDispatcher::microSessionManager( long technology )
{
  pool::DbType dbType( technology );
  long majorType = dbType.majorType();
  std::map< long, pool::PersistencySvc::MicroSessionManager* >::iterator iManager = m_technologyTypes.find( majorType );
  if ( iManager != m_technologyTypes.end() ) {
    return *(iManager->second);
  }

  // Technology does not exist. Create the new session.
  pool::PersistencySvc::MicroSessionManager* mgr = new pool::PersistencySvc::MicroSessionManager( m_registry,
                                                                                                  m_transaction,
                                                                                                  majorType );
  m_technologyTypes.insert( std::make_pair( majorType, mgr ) );
  pool::DbType dbTypeMajor( majorType );
  m_technologyNames.insert( std::make_pair( dbTypeMajor.storageName(), mgr ) );
  return *mgr;
}


const pool::PersistencySvc::MicroSessionManager&
pool::PersistencySvc::TechnologyDispatcher::microSessionManager( long technology ) const
{
  pool::DbType dbType( technology );
  long majorType = dbType.majorType();
  std::map< long, pool::PersistencySvc::MicroSessionManager* >::const_iterator iManager = m_technologyTypes.find( majorType );
  if ( iManager == m_technologyTypes.end()  ){
    // Technology does not exist. Throw an exception !
    throw pool::PersistencySvcException( "Unregistered technology",
                                         "PersistencySvc::TechnologyDispatcher::microSessionManager");
  }
  return *(iManager->second);
}


pool::PersistencySvc::MicroSessionManager&
pool::PersistencySvc::TechnologyDispatcher::microSessionManager( const std::string& technology )
{
  std::map< std::string, pool::PersistencySvc::MicroSessionManager* >::iterator iManager = m_technologyNames.find( technology );
  if ( iManager != m_technologyNames.end() ) {
    return *(iManager->second);
  }
  else {
    return this->microSessionManager( pool::DbType::getType( technology ).type() );
  }
}

const pool::PersistencySvc::MicroSessionManager&
pool::PersistencySvc::TechnologyDispatcher::microSessionManager( const std::string& technology ) const
{
  std::map< std::string, pool::PersistencySvc::MicroSessionManager* >::const_iterator iManager = m_technologyNames.find( technology );
  if ( iManager != m_technologyNames.end() ) {
    return *(iManager->second);
  }
  else {
    return this->microSessionManager( pool::DbType::getType( technology ).type() );
  }
}

void
pool::PersistencySvc::TechnologyDispatcher::disconnectAll()
{
  for ( std::map< long, pool::PersistencySvc::MicroSessionManager* >::iterator iManager = m_technologyTypes.begin();
        iManager != m_technologyTypes.end(); ++iManager ) {
    iManager->second->disconnectAll();
  }
}
