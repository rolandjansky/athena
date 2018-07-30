/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: PersistencySvc.cpp 739662 2016-04-12 11:55:10Z krasznaa $

// includes
#include "PersistencySvc.h"
#include "UserSession.h"
#include "UserDatabase.h"
#include "DatabaseHandler.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "PersistencySvc/DatabaseSpecification.h"
#include "PersistencySvc/ITransaction.h"
#include "FileCatalog/IFileCatalog.h"

#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/Placement.h"

pool::PersistencySvc::PersistencySvc::PersistencySvc( pool::IFileCatalog& fileCatalog ):
  m_session( new pool::PersistencySvc::UserSession( fileCatalog ) )
{}

pool::PersistencySvc::PersistencySvc::~PersistencySvc()
{
  delete m_session; m_session = 0;
}

/*
void
pool::PersistencySvc::PersistencySvc::setFileCatalog( pool::IFileCatalog& catalog )
{
  m_session->setFileCatalog( catalog );
}

pool::IFileCatalog&
pool::PersistencySvc::PersistencySvc::fileCatalog()
{
  return m_session->fileCatalog();
}
*/

void*
pool::PersistencySvc::PersistencySvc::readObject( const Token& token, void* object )
{
  void* result( (void*)0 );
  if ( m_session->transaction().isActive() ) {
    UserDatabase db( m_session->technologyDispatcher(),
                     m_session->defaultConnectionPolicy(),
                     m_session->fileCatalog(),
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
                   m_session->defaultConnectionPolicy(),
                   m_session->fileCatalog(),
                   m_session->transaction(),
                   m_session->registry(),
                   place.fileName(),
                   pool::DatabaseSpecification::PFN );
  if ( db.openMode() == pool::IDatabase::CLOSED ) {
    db.setTechnology( place.technology() );
    db.connectForWrite();
  }
  return db.databaseHandler().writeObject( place.containerName(),
                                           place.technology(),
                                           object,
                                           type );
}

pool::ISession&
pool::PersistencySvc::PersistencySvc::session()
{
  return static_cast< pool::ISession& >( *m_session );
}
