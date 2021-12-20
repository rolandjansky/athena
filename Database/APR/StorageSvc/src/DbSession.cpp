/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
#include "CxxUtils/checker_macros.h"

using namespace pool;

/// Object constructor
DbSession::DbSession(DbSessionObj* pSession) {
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
void DbSession::switchPtr(DbSessionObj* obj) {
  if (   obj ) obj->addRef();
  if ( m_ptr ) {
    if (m_ptr->release() == 0) m_ptr = 0;
  }
  m_ptr = obj;
  if ( m_ptr )   {
    setType(m_ptr->type());
  }
}

/// Open session by handle
DbStatus DbSession::open()  {
  if ( 0 == m_ptr )   {
    switchPtr(new DbSessionObj());
  }
  return (isValid()) ? m_ptr->open() : Error;
}

/// Close session by handle
DbStatus DbSession::close() {
  if ( m_ptr )       {
    m_ptr->clearEntries();
    m_ptr->close();
  }
  switchPtr(0);
  return Success;
}

/// Add domain to session
DbStatus DbSession::add(DbDomainObj* dom)   {
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
DbStatus DbSession::remove(const DbDomainObj* dom) {
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
DbDomainObj* DbSession::find(const DbType& tech_type) {
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
IOODatabase* DbSession::db(const DbType& typ) {
  return isValid() ? m_ptr->db(typ) : 0;
}

