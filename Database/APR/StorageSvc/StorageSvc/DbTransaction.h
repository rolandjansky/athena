/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbTransaction.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbTransaction definition
//--------------------------------------------------------------------
//
//  Package    : (The POOL project)
//
//  Description: Generic persistable BLOB object
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBTransaction_H
#define POOL_DBTransaction_H 1

// Framework include files
#include "StorageSvc/DbStatus.h"
#include "StorageSvc/Transaction.h"

/*
 *   POOL namespace declaration
 */
namespace pool  {

  // Forward declarations
  class DbDomainObj;
  class DbDatabaseObj;
  class DbContainerObj;

  /** Definition of the Transaction class

    - Purpose:    
      Stored the context for Database Transactions.

    @author  Markus Frank                                               
    @version 1.0
   */
  class DbTransaction    {
    friend class DbDomain;
    friend class DbDatabase;
    friend class DbContainer;
  private:
    /// Transaction type
    Transaction::Action m_state;
    /// Transaction context
    Transaction::Type   m_type;
    /// Internal storage
    union Pointers  {
      DbDomainObj*    dom;
      DbDatabaseObj*  db;
      DbContainerObj* cnt;
      char*           p;
    };
    Pointers        m_ptr;
  private:
    /// No copy allowed!
    DbTransaction(const DbTransaction& copy);
    /// Validate tranaction for Domains
    DbStatus validate(DbDomainObj* domH);
    /// Validate tranaction for Databases
    DbStatus validate(DbDatabaseObj* dbH);
    /// Validate tranaction for Containers
    DbStatus validate(DbContainerObj* cntH);
  public:
    /// Standard constructor
    DbTransaction();
    /// Standard destructor
    virtual ~DbTransaction();
    /// Check Transaction validity
    bool isValid()  const;
    /// Update Transaction type
    DbTransaction& set(Transaction::Action new_state);
    /// Access Transaction type
    Transaction::Action state() const   {     return m_state;   }
    /// Access Transaction context type
    Transaction::Type type() const      {     return m_type;    }
  };
}       // End namespace pool
#endif  // POOL_DBTransaction_H
