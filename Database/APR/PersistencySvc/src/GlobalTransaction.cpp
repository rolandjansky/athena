/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GlobalTransaction.h"
#include "DatabaseHandler.h"
#include "CoralBase/MessageStream.h"

pool::PersistencySvc::GlobalTransaction::GlobalTransaction( pool::PersistencySvc::DatabaseRegistry& registry ):
  m_type( pool::ITransaction::UNDEFINED ),
  m_databases( registry )
{}


pool::PersistencySvc::GlobalTransaction::~GlobalTransaction()
{
  if ( this->isActive() ) {
    this->rollback();
  }
}


bool
pool::PersistencySvc::GlobalTransaction::start( pool::ITransaction::Type type )
{
  if ( this->isActive() || type == pool::ITransaction::UNDEFINED ) return false;
  unsigned int successfullDatabases = 0;
  bool result = false;
  for ( pool::PersistencySvc::DatabaseRegistry::iterator iDb = m_databases.begin();
        iDb != m_databases.end(); ++iDb ) {
    if ( ! (*iDb)->startTransaction() ) {
      coral::MessageStream log( "PersistencySvc::GlobalTransaction" );
      log << coral::Error << "Could not start a transaction for the database with" << coral::MessageStream::endmsg
          << "FID = " << (*iDb)->fid() << coral::MessageStream::endmsg
          << "PFN = " << (*iDb)->pfn() << coral::MessageStream::endmsg;
      break;
    }
    ++successfullDatabases;
  }

  if ( successfullDatabases == m_databases.size() ) {
    result = true;
  }
  else { // at least one database failed. Rollback the ones we started
    pool::PersistencySvc::DatabaseRegistry::iterator iDb = m_databases.begin();
    while ( successfullDatabases > 0 ) {
      (*iDb)->rollBackTransaction();
      --successfullDatabases;
      ++iDb;
    }
  }

  if ( result ) m_type = type;
  return result;
}


bool
pool::PersistencySvc::GlobalTransaction::commit()
{
  if ( this->isActive() ) {
    bool OK = true;
    for ( pool::PersistencySvc::DatabaseRegistry::iterator iDb = m_databases.begin();
          iDb != m_databases.end(); ++iDb ) {
      bool bCommit = (*iDb)->commitTransaction(); // This has to be replaced with a two phase commit
      if ( ! bCommit ) {
        coral::MessageStream log( "PersistencySvc::GlobalTransaction" );
        log << coral::Error << "Could not commit the transaction for the database with" << coral::MessageStream::endmsg
            << "FID = " << (*iDb)->fid() << coral::MessageStream::endmsg
            << "PFN = " << (*iDb)->pfn() << coral::MessageStream::endmsg;
      }
      OK = OK && bCommit;
    }
    m_type = pool::ITransaction::UNDEFINED;
    return OK;
  }
  return false;
}


bool
pool::PersistencySvc::GlobalTransaction::commitAndHold()
{
  if ( this->isActive() ) {
    bool OK = true;
    for ( pool::PersistencySvc::DatabaseRegistry::iterator iDb = m_databases.begin();
          iDb != m_databases.end(); ++iDb ) {
      bool bCommit = (*iDb)->commitAndHoldTransaction(); // This has to be replaced with a two phase commit
      if ( ! bCommit ) {
        coral::MessageStream log( "PersistencySvc::GlobalTransaction" );
        log << coral::Error << "Could not commit and hold the transaction for the database with" << coral::MessageStream::endmsg
            << "FID = " << (*iDb)->fid() << coral::MessageStream::endmsg
            << "PFN = " << (*iDb)->pfn() << coral::MessageStream::endmsg;
      }
      OK = OK && bCommit;
    }
    return OK;
  }
  return false;
}


void
pool::PersistencySvc::GlobalTransaction::rollback()
{
  if ( this->isActive() ) {
    for ( pool::PersistencySvc::DatabaseRegistry::iterator iDb = m_databases.begin();
          iDb != m_databases.end(); ++iDb ) {
      (*iDb)->rollBackTransaction();
    }
    m_type = pool::ITransaction::UNDEFINED;
  }
}


bool
pool::PersistencySvc::GlobalTransaction::isActive() const
{
  if ( m_type == pool::ITransaction::UNDEFINED ) return false;
  return true;
}


void
pool::PersistencySvc::GlobalTransaction::update()
{
  if ( m_type == pool::ITransaction::READ ) {
    m_type = pool::ITransaction::UPDATE;
  }
}


pool::ITransaction::Type
pool::PersistencySvc::GlobalTransaction::type() const
{
  return m_type;
}
