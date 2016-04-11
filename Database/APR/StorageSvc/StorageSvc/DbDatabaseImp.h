/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbDatabaseImp.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBDATABASEIMP_H
#define POOL_DBDATABASEIMP_H 1

/// Framework include files
#include "StorageSvc/IDbDatabase.h"
#include "StorageSvc/DbImplementation.h"

/*
 * POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class DbDatabase;
  class DbContainer;
  class IOODatabase;

  /** @class DbDatabaseImp DbDatabaseImp.h StorageSvc/DbDatabaseImp.h
    *
    * Description:
    * "Generic" Database implementation
    *
    * Generic helper class to implement stuff common to all
    * existing Databases. The base implementations can allways 
    * be overwritten.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbDatabaseImp : public DbImplementation, 
                        virtual public IDbDatabase
  {
  protected:
    /// Standard Destructor
    virtual ~DbDatabaseImp();

  public:

    /// Constructor with initializing arguments
    /** @param idb      [IN]  Pointer to implementation object
      *
      * @return Reference to initialized object
      */
    DbDatabaseImp(IOODatabase* idb);

    /// Release instance (Abstract interfaces do not expose destructor!)
    virtual void release()             { delete this;         }

    /// Access the size of the database: May be undefined for some technologies
    virtual long long int size()  const   { return -1;           }

    /// Callback after successful open of a database object
    /** @param dbH      [IN]  Handle to valid database object
      * @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus onOpen(const DbDatabase& dbH, DbAccessMode mode);

    /// Re-open database with changing access permissions
    /** @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus reopen(DbAccessMode mode);

    /// Find container in Database: to be overridden
    /** @param handle   [IN]  Handle to invalid container object.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus find(DbContainer& handle);

    /// Access options
    /** @param opt      [IN]  Reference to option object.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus getOption(DbOption& opt) const;

    /// Set options
    /** @param opt      [IN]  Reference to option object.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus setOption(const DbOption& opt);

    /// Start/Commit/Rollback Database Transaction
    /** @param refTransaction [IN/OUT]  Transaction context.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus transAct(DbTransaction& refTransaction);
  };
}      // End namespace pool
#endif // POOL_DBDATABASEIMP_H
