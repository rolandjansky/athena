/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  DbObjectGuard definition
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  Description: Guard containing object persistency information
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBOBJECTGUARD_H
#define POOL_DBOBJECTGUARD_H 1

// Framework include files
#include "PersistentDataModel/Token.h"
#include "StorageSvc/DbContainer.h"

/*
 * POOL namespace declaration
 */
namespace pool    {

  /** @class DbObjectGuard DbObjectGuard.h StorageSvc/DbObjectGuard.h 
    *
    *  Description:
    *  class containing persistency information for managing 
    *  basic persistable objects.
    *
    *  @author  M.Frank
    *  @version 1.0
    */
  class DbObjectGuard   {
  private:
    /// Size of memory chunk
    size_t                        m_size;
    /// Reference to persistent container
    DbContainer                   m_container;
    /// OID
    Token::OID_t                  m_oid;
   public:
    /// Destructor
    virtual ~DbObjectGuard();
    /// Standard constructor
    DbObjectGuard(int type = 0, int siz = 0);
    /// Copy constructor
    DbObjectGuard(const DbContainer& cntH, int siz = 0);
    /// Access memory chunk size
    size_t size()   const             { return m_size;            }
    /// Access oid
    Token::OID_t& oid()               { return m_oid;             }
    const Token::OID_t& oid() const   { return m_oid;             }
    /// Access Storage Type
    const DbType& type()  const       { return m_container.type();}
    /// Accessor: get object container
    DbContainer& container()          { return m_container;       }
    const DbContainer& container() const { return m_container;       }
  };
}       // End namespace pool
#endif  // POOL_DBOBJECTGUARD_H
