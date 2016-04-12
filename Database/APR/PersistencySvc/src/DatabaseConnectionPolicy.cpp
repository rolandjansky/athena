/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "CoralBase/MessageStream.h"

bool
pool::DatabaseConnectionPolicy::setWriteModeForExisting( pool::DatabaseConnectionPolicy::Mode mode )
{
  if ( mode == pool::DatabaseConnectionPolicy::CREATE ) {
    coral::MessageStream log( "PersistencySvc::DatabaseConnectionPolicy" );
    log << coral::Error << "CREATE is not allowed as an option in setWriteModeForExisting()" 
        << coral::MessageStream::endmsg;
  }
  else if ( mode == pool::DatabaseConnectionPolicy::READ ) {
    coral::MessageStream  log( "PersistencySvc::DatabaseConnectionPolicy" );
    log << coral::Error << "READ is not allowed as an option in setWriteModeForExisting()" 
        << coral::MessageStream::endmsg;
  }
  else {
    if ( mode == pool::DatabaseConnectionPolicy::RAISE_ERROR ||
         mode == pool::DatabaseConnectionPolicy::UPDATE ||
         mode == pool::DatabaseConnectionPolicy::OVERWRITE ) {
      m_writeModeForExisting = mode;
      return true;
    }
    else {
      coral::MessageStream log( "PersistencySvc::DatabaseConnectionPolicy" );
      log << coral::Error << "Unrecognizable option specified in setWriteModeForExisting()" 
          << coral::MessageStream::endmsg;
    }
  }
  return false;
}

bool
pool::DatabaseConnectionPolicy::setWriteModeForNonExisting( pool::DatabaseConnectionPolicy::Mode mode )
{
  if ( mode ==  pool::DatabaseConnectionPolicy::READ ) {
    coral::MessageStream log( "PersistencySvc::DatabaseConnectionPolicy" );
    log << coral::Error << "READ is not allowed as an option in setWriteModeForNonExisting()" 
        << coral::MessageStream::endmsg;
  }
  else if ( mode == pool::DatabaseConnectionPolicy::OVERWRITE ) {
    coral::MessageStream log( "PersistencySvc::DatabaseConnectionPolicy" );
    log << coral::Error << "OVERWRITE is not allowed as an option in setWriteModeForExisting()" 
        << coral::MessageStream::endmsg;
  }
  else {
    if ( mode == pool::DatabaseConnectionPolicy::RAISE_ERROR ||
         mode == pool::DatabaseConnectionPolicy::CREATE ||
         mode == pool::DatabaseConnectionPolicy::UPDATE ) {
      m_writeModeForNonExisting = mode;
      return true;
    }
    else {
      coral::MessageStream log( "PersistencySvc::DatabaseConnectionPolicy" );
      log << coral::Error << "Unrecognizable option specified in setWriteModeForNonExisting()"
          << coral::MessageStream::endmsg;
    }
  }
  return false;
}

bool
pool::DatabaseConnectionPolicy::setReadMode( pool::DatabaseConnectionPolicy::Mode mode )
{
  if ( mode == pool::DatabaseConnectionPolicy::CREATE ) {
    coral::MessageStream log( "PersistencySvc::DatabaseConnectionPolicy" );
    log << coral::Error << "CREATE is not allowed as an option in setReadMode()" << coral::MessageStream::endmsg;
  }
  else if ( mode == pool::DatabaseConnectionPolicy::OVERWRITE ) {
    coral::MessageStream log( "PersistencySvc::DatabaseConnectionPolicy" );
    log << coral::Error << "OVERWRITE is not allowed as an option in setReadMode()" << coral::MessageStream::endmsg;
  }
  else if ( mode == pool::DatabaseConnectionPolicy::RAISE_ERROR ) {
    coral::MessageStream log( "PersistencySvc::DatabaseConnectionPolicy" );
    log << coral::Error << "RAISE_ERROR is not allowed as an option in setReadMode()" << coral::MessageStream::endmsg;
  }
  else {
    if ( mode == pool::DatabaseConnectionPolicy::READ ||
         mode == pool::DatabaseConnectionPolicy::UPDATE ) {
      m_readMode = mode;
      return true;
    }
    else {
      coral::MessageStream log( "PersistencySvc::DatabaseConnectionPolicy" );
      log << coral::Error << "Unrecognizable option specified in setReadMode()" << coral::MessageStream::endmsg;
    }
  }
  return false;
}
