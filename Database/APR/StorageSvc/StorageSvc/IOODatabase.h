/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
    * <LI> Create Database domains.
    * <LI> Create Database files.
    * <LI> Create Database containers.
    * <LI> The Db conversion service also supports storage technology
    *      independent object linking.
    * </UL>
    *
    * @author  Markus Frank
    * @version 1.0
    */
   class IOODatabase : public RefCounter  {
   public:  
    virtual ~IOODatabase() {}

   public:
    /// Name of the Database implementation
    virtual const std::string&  name () const = 0;

    /// Create Database domain
    virtual IDbDomain* createDomain(void) = 0;
    /// Create Database
    virtual IDbDatabase* createDatabase(void) = 0;
    /// Create Database container
    virtual IDbContainer* createContainer(const DbType& typ) = 0;

    ///  delete object if ref count drops to 0
    void release() { if( RefCounter::subRef() <= 0 ) delete this; }
  };
}       // End namespace pool
#endif  // POOL_IOODATABASE_H
