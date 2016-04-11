/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbSessionObj.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbSessionObj object definition
//--------------------------------------------------------------------
//
//  Package    : System (The POOL project)
//
//  Description: Generic data persistency
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBSESSIONOBJ_H
#define POOL_DBSESSIONOBJ_H 1

// Framework includes
#include "StorageSvc/DbAccessObj.h"

/*
 *   POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class DbDomainObj;

  /** Db objects: DbSessionObj

      Description:
      Implementation independent part of a Database session object.

      There is a ring of protection around the object. The object can
      only be accessed through its handle, the technology dependent 
      code and the DbDatabaseObj object hosting the container.
      This should ensure proper reference counting and inhibit non
      existing references flying around.

      @author  M.Frank
      @version 1.0
  */
  class DbSessionObj : public DbAccessObj<DbType, DbDomainObj >   {
  private:
    /// Known Implementation types
    void*         m_dbTypes;
    /// Unser context
    void*         m_ctxt;
  public:
    /// Standard constructor
    DbSessionObj();
    /// Standard destructor
    virtual ~DbSessionObj();
    /// Allow access to the Database implementation
    IOODatabase* db(const DbType& typ)  const;
    /// Open session
    DbStatus open(void* context);
    /// Close Database session
    DbStatus close();
    /// Inject new Database implementation
    DbStatus addDb(IOODatabase* db_impl)   const;
  };
}       // End namespace pool
#endif  // POOL_DBSESSIONOBJ_H
