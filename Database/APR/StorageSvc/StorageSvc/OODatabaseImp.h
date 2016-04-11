/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OODatabaseImp.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_OODATABASEIMP_H
#define POOL_OODATABASEIMP_H 1

/// Framework include files
#include "StorageSvc/IOODatabase.h"

/*
 *   POOL namespace declaration
 */
namespace pool    {

  /** @class OODatabaseImp OODatabaseImp.h StorageSvc/OODatabaseImp.h
    *
    * Description:
    *
    * "Generic" OODatabase implementation
    *
    * Generic helper class to implement stuff common to all
    * existing Databases. The base implementations can allways be overwritten.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class OODatabaseImp : public IOODatabase  {

  protected:
    /// Reference counter
    int                 m_refCount;
    /// Type the implementation belongs to
    DbType              m_type;
    /// Object name
    std::string         m_name;
    /// Pointer to user context
    void*               m_context;

  protected:
    /// Constructor with initializing arguments
    OODatabaseImp(void* ctxt, const DbType& typ);
    /// Standard Destructor
    virtual ~OODatabaseImp();
  public:
    /// IInterface implementation: Increase reference count
    virtual unsigned int addRef()            { return ++m_refCount;    }
    /// IInterface implementation: Decrease reference count
    virtual unsigned int release();
    /// IInterface implementation: Query interface
    virtual DbStatus queryInterface(const Guid& riid, void** ppv);
    /// IOODatabase implementation: Initialize the Database
    virtual DbStatus initialize (const std::string& name);
    /// IOODatabase implementation: Finalize the Database
    virtual DbStatus finalize(void)             { return Success; }
    /// IOODatabase implementation: Access to OODatabase type
    virtual const DbType& type()  const       { return m_type;          }
    /// IOODatabase implementation: Object's name
    virtual const std::string& name()  const  { return m_name;          }
    /// IOODatabase implementation: Access to user context
    virtual void*  context()  const           { return m_context;       }
  };
}      // End namespace pool
#endif // POOL_OODATABASEIMP_H
