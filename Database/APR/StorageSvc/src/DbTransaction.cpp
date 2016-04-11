/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbTransaction.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbTransaction implementation
//--------------------------------------------------------------------
//
//  Package    : System (The POOL project)
//
//  Description: Generic data persistency
//
//  @author      M.Frank
//====================================================================

// Framework include files
#include "DbDomainObj.h"
#include "DbDatabaseObj.h"
#include "DbContainerObj.h"
#include "StorageSvc/DbTransaction.h"
#include "StorageSvc/DbInstanceCount.h"
#include "StorageSvc/Transaction.h"
using namespace pool;

static DbInstanceCount::Counter* s_count = 
  DbInstanceCount::getCounter(typeid(DbTransaction));

/// No copy allowed!
DbTransaction::DbTransaction(const DbTransaction& /* copy */ )  {    
  s_count->increment();
  m_state = Transaction::TRANSACT_START;
  m_type  = Transaction::TRANSACT_NONE;
  m_ptr.p = 0;
}

/// Standard constructor for domain level Transactions
DbTransaction::DbTransaction()   {
  s_count->increment();
  m_state = Transaction::TRANSACT_START;
  m_type  = Transaction::TRANSACT_NONE;
  m_ptr.p = 0;
}

/// Standard destructor
DbTransaction::~DbTransaction()    {
  s_count->decrement();
  switch(m_type)    {
  case Transaction::TRANSACT_DOM:    m_ptr.dom->release();    break;
  case Transaction::TRANSACT_DB:     m_ptr.db->release();     break;
  case Transaction::TRANSACT_CNT:    m_ptr.cnt->release();    break;
  case Transaction::TRANSACT_NONE:
  default:                                                    break;
  }
}

/// Validate tranaction for Domains
DbStatus DbTransaction::validate(DbDomainObj* domH)  {
  if ( domH )   {
    switch(m_type)    {
    case Transaction::TRANSACT_NONE:
      m_ptr.dom = domH;
      m_ptr.dom->addRef();
      m_state = Transaction::TRANSACT_START;
      m_type  = Transaction::TRANSACT_DOM;
      return Success;
    case Transaction::TRANSACT_CNT:   // Cannot use Container Transaction for domains   
    case Transaction::TRANSACT_DB:    // Cannot use Database Transaction for domains
      return Error;
    case Transaction::TRANSACT_DOM:
      return m_ptr.dom == domH ? Success : Error;
    }
  }
  return Error;
}

/// Validate tranaction for Databases
DbStatus DbTransaction::validate(DbDatabaseObj* dbH)  {
  if ( dbH )  {
    switch(m_type)    {
    case Transaction::TRANSACT_NONE:
      m_ptr.db = dbH;
      m_ptr.db->addRef();
      m_state = Transaction::TRANSACT_START;
      m_type  = Transaction::TRANSACT_DB;
      return Success;
    case Transaction::TRANSACT_CNT:    // Cannot use Container Transaction for Databases
      return Error;
    case Transaction::TRANSACT_DB:
      return dbH == m_ptr.db ? Success : Error;
    case Transaction::TRANSACT_DOM: 
      return dbH->domain().ptr() == m_ptr.dom 
        ? Success : Error;
    }
  }
  return Error;
}

/// Validate tranaction for Containers
DbStatus DbTransaction::validate(DbContainerObj* cntH)  {
  if ( cntH )   {
    switch(m_type)    {
    case Transaction::TRANSACT_NONE:
      m_ptr.cnt = cntH;
      m_ptr.cnt->addRef();
      m_state = Transaction::TRANSACT_START;
      m_type  = Transaction::TRANSACT_CNT;
      return Success;
    case Transaction::TRANSACT_CNT:    
      return cntH == m_ptr.cnt ? Success : Error;
    case Transaction::TRANSACT_DB:
      return cntH->database().ptr()==m_ptr.db
        ? Success : Error;
    case Transaction::TRANSACT_DOM:
      return cntH->database().containedIn().ptr() == m_ptr.dom
        ? Success : Error;
    }
  }
  return Error;
}

/// Check Transaction validity
bool DbTransaction::isValid()  const    {
  switch(m_type)    {
  case Transaction::TRANSACT_DOM:
  case Transaction::TRANSACT_DB:
  case Transaction::TRANSACT_CNT:
    return m_ptr.p != 0;
  case Transaction::TRANSACT_NONE:
  default:
    return false;
  }
}

/// Update Transaction type
DbTransaction& DbTransaction::set(Transaction::Action new_type) {
  m_state = new_type;
  return *this;
}
