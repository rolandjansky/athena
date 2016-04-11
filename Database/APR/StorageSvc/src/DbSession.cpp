/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbSession.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbSessionObj handle implementation
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  Description: Generic data persistency
//
//  @author      M.Frank
//====================================================================

#include "StorageSvc/DbSession.h"
#include "DbDomainObj.h"
#include "DbSessionObj.h"

using namespace pool;

/// Object constructor
DbSession::DbSession(const DbSessionObj* pSession) {
  setType(POOL_StorageType);
  switchPtr(pSession);
}

/// Constructor
DbSession::DbSession() {
  setType(POOL_StorageType);
}

/// Copy constructor
DbSession::DbSession(const DbSession& copy)
: DbHandleBase<DbSessionObj>()
{
  setType(POOL_StorageType);
  switchPtr(copy.m_ptr);
}

/// Copy constructor
DbSession::~DbSession() {
  switchPtr(0);
}

/// Attach session to handle
void DbSession::switchPtr(const DbSessionObj* obj) const {
  if (   obj ) obj->addRef();
  if ( m_ptr ) {
    if (m_ptr->release() == 0) m_ptr = 0;
  }
  m_ptr = const_cast<DbSessionObj*>(obj);
  if ( m_ptr )   {
    setType(m_ptr->type());
  }
}

/// Open session by handle
DbStatus DbSession::open(void* context)  {
  if ( 0 == m_ptr )   {
    switchPtr(new DbSessionObj());
  }
  return (isValid()) ? m_ptr->open(context) : Error;
}

/// Close session by handle
DbStatus DbSession::close()   const   {
  if ( m_ptr )       {
    m_ptr->clearEntries();
    m_ptr->close();
  }
  switchPtr(0);
  return Success;
}

/// Add domain to session
DbStatus DbSession::add(DbDomainObj* dom)  const   {
  DbStatus sc = Error;
  if ( isValid() && dom )    {
    sc = m_ptr->add(dom->type().type(), dom);
    if ( sc.isSuccess() )  {
      m_ptr->addRef();
    }
  }
  return sc;
}

/// Remove domain from current session
DbStatus DbSession::remove(const DbDomainObj* dom)  const {
  DbStatus sc = Error;
  if ( isValid() && dom )    {
    sc = m_ptr->remove(dom);
    if ( isValid() && sc.isSuccess() )  {
      if (m_ptr->release() == 0) m_ptr = 0;
    }
  }
  return sc;
}

/// Add reference count to object if present
int DbSession::refCount() const  {
  return isValid() ? m_ptr->refCount() : int(INVALID);
}

/// Find domain in session
DbDomainObj* DbSession::find(const DbType& tech_type)  const {
  DbDomainObj* dom = 0;
  if ( isValid() )  {
    dom = m_ptr->find(tech_type);
    if ( 0 == dom )   {
      dom = m_ptr->find(tech_type.type());
    }
  }
  return dom;
}

/// Allow access to the Database implementation
IOODatabase* DbSession::db(const DbType& typ) const {
  return isValid() ? m_ptr->db(typ) : 0;
}

/// Inject new Database implementation
DbStatus DbSession::addDb(IOODatabase* db_impl) const {
  return (isValid() && db_impl) ? m_ptr->addDb(db_impl) : Error;
}
