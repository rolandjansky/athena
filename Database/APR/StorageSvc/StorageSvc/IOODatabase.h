/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IOODatabase.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  IOODatabase interface definition
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The pool framework)
//
//  Description: Interface of the Db persistenty service
//
//  @author      M.Frank
//
//====================================================================
#ifndef POOL_IOODATABASE_H
#define POOL_IOODATABASE_H
// $Header: /cvs/PF/pool/StorageSvc/StorageSvc/IOODatabase.h,v 1.18 2007/11/28 16:19:33 frankb Exp $

// Framework include files
#include "StorageSvc/pool.h"
#include "StorageSvc/DbType.h"

class Guid;

/*
 *  POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class IDbDomain;
  class IDbDatabase;
  class IDbContainer;

  /** @class IOODatabase interface definition.
    *
    *
    * <P> The IOODatabase interface allows to:
    * <UL>
    * <LI> Create Database sessions.
    * <LI> Create Database domains.
    * <LI> Create Database files.
    * <LI> Create Database containers.
    * <LI> Create, delete and release Database objects.
    * <LI> The Db conversion service also supports storage technology
    *      independent object linking.
    * </UL>
    *
    * @author  Markus Frank
    * @version 1.0
    */
  class IOODatabase   {
  protected:
    /// Destructor (called only by sub-classes)
    virtual ~IOODatabase()   {     }

  public:
    /// Retrieve interface ID
    static const Guid& interfaceID();

    /// Default empty constructor
    explicit IOODatabase() {}
    /// IInterface implementation: Query interfaces of Interface
    virtual DbStatus queryInterface(const Guid& riid, void** ppvUnkn) = 0;
    /** Increment the reference count of the  Interface instance.
        @return  Current reference count value.
    */
    virtual unsigned int addRef() = 0;
    /** Decrement reference count and release Interface instance when reference count=0.
        @return  Current reference count value.
    */
    virtual unsigned int release() = 0;
    /// Initialize the Database
    virtual DbStatus  initialize(const std::string& nam) = 0;
    /// Initialize the Database
    virtual DbStatus  finalize (void) = 0;
    /// Access to technology type
    virtual const DbType& type () const = 0;
    /// Name of the Database implementation
    virtual const std::string&  name () const = 0;
    /// Access to user context
    virtual void*  context()  const = 0;
    /// Create Database domain
    virtual IDbDomain* createDomain(void) = 0;
    /// Create Database
    virtual IDbDatabase* createDatabase(void) = 0;
    /// Create Database container
    virtual IDbContainer* createContainer(const DbType& typ) = 0;
  };
}       // End namespace pool
#endif  // POOL_IOODATABASE_H
