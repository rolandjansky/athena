/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: PersistencySvc.cpp 739662 2016-04-12 11:55:10Z krasznaa $

// includes
#include "PersistencySvc.h"
#include "UserSession.h"
#include "PersistencySvcConfiguration.h"
#include "UserDatabase.h"
#include "DatabaseHandler.h"
#include "PersistentDataModel/Token.h"
#include "PersistencySvc/DatabaseSpecification.h"
#include "PersistencySvc/ITransaction.h"

#include "PersistentDataModel/Placement.h"

pool::PersistencySvc::PersistencySvc::PersistencySvc( pool::IFileCatalog& fileCatalog ):
  m_session( new pool::PersistencySvc::UserSession( fileCatalog ) )
{}

pool::PersistencySvc::PersistencySvc::~PersistencySvc()
{
  delete m_session; m_session = 0;
}

void
pool::PersistencySvc::PersistencySvc::setFileCatalog( pool::IFileCatalog& catalog )
{
  m_session->configuration().setFileCatalog( catalog );
}

pool::IFileCatalog&
pool::PersistencySvc::PersistencySvc::fileCatalog()
{
  return m_session->configuration().fileCatalog();
}

pool::IConfiguration&
pool::PersistencySvc::PersistencySvc::configuration(){
  return m_session->configuration();
}

void*
pool::PersistencySvc::PersistencySvc::readObject( const Token& token, void* object )
{
  void* result( (void*)0 );
  if ( m_session->transaction().isActive() ) {
    UserDatabase db( m_session->technologyDispatcher(),
                     m_session->configuration(),
                     m_session->transaction(),
                     m_session->registry(),
                     token.dbID().toString(),
                     pool::DatabaseSpecification::FID );
    if ( db.openMode() == pool::IDatabase::CLOSED ) {
      db.setTechnology( token.technology() );
      db.connectForRead();
    }
    result = db.databaseHandler().readObject( token, object );
  }
  return result;
}

Token*
pool::PersistencySvc::PersistencySvc::registerForWrite( const Placement& place,
                                                        const void* object,
                                                        const RootType& type )
{
  if ( ! m_session->transaction().isActive() ||
       m_session->transaction().type() != pool::ITransaction::UPDATE ) return 0;
  UserDatabase db( m_session->technologyDispatcher(),
                   m_session->configuration(),
                   m_session->transaction(),
                   m_session->registry(),
                   place.fileName(),
                   pool::DatabaseSpecification::PFN );
  pool::IDatabase::OpenMode openMode = db.openMode();
  switch ( openMode ) {
  case pool::IDatabase::CLOSED:
    db.setTechnology( place.technology() );
    db.connectForWrite();
    break;
  case pool::IDatabase::READ:
    db.revertMode( pool::IDatabase::UPDATE );
    break;
  default:
    break;
  };
  return db.databaseHandler().writeObject( place.containerName(),
                                           place.technology(),
                                           object,
                                           type );
}

bool
pool::PersistencySvc::PersistencySvc::destroyObject( const Token& token )
{
  if ( ! m_session->transaction().isActive() ||
       m_session->transaction().type() != pool::ITransaction::UPDATE ) return false;
  UserDatabase db( m_session->technologyDispatcher(),
                   m_session->configuration(),
                   m_session->transaction(),
                   m_session->registry(),
                   token.dbID().toString(),
                   pool::DatabaseSpecification::FID );
  pool::DatabaseConnectionPolicy policy; // Keep the default behaviour.
  pool::IDatabase::OpenMode openMode = db.openMode();
  switch ( openMode ) {
  case pool::IDatabase::CLOSED:
    db.setTechnology( token.technology() );
    db.connectForWrite( policy );
    break;
  case pool::IDatabase::READ:
    db.revertMode( pool::IDatabase::UPDATE );
    break;
  default:
    break;
  };
  return db.databaseHandler().destroyObject( token );
}

std::string
pool::PersistencySvc::PersistencySvc::getContName( const Token& token )
{
    UserDatabase db( m_session->technologyDispatcher(),
                     m_session->configuration(),
                     m_session->transaction(),
                     m_session->registry(),
                     token.dbID().toString(),
                     pool::DatabaseSpecification::FID );
    return db.databaseHandler().getContName( token );
}

pool::ISession&
pool::PersistencySvc::PersistencySvc::session()
{
  return static_cast< pool::ISession& >( *m_session );
}
