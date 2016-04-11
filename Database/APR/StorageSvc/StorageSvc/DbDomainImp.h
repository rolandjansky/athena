/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbDomainImp.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBDOMAINIMP_H
#define POOL_DBDOMAINIMP_H

/// Framework include files
#include "StorageSvc/IDbDomain.h"
#include "StorageSvc/DbImplementation.h"

/*
 * POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class DbSession;
  class IOODatabase;

  /** "Generic" domain implementation

      Description: Generic helper class to implement stuff common to all
      existing Databases. The base implementations can allways be overwritten.

      @author  M.Frank
      @version 1.0
  */
  class DbDomainImp 
  : public DbImplementation, 
    virtual public IDbDomain
  {
  protected:
    /// Standard Destructor
    virtual ~DbDomainImp();
  public:
    /// Constructor with initializing arguments
    DbDomainImp(IOODatabase* idb);
    /// Release instance (Abstract interfaces do not expose destructor!)
    virtual void release()                { delete this;                      }
    /// Close domain
    virtual DbStatus close()                { return Success;           }

    /// Open domain
    /** @param sessionH [IN]  Handle to valid session object
      *                       (validity ensured by upper levels).
      * @param nam      [IN]  Name of the domain to be opened.
      * @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus open(const DbSession&   sesH, 
                        const std::string& nam, 
                        DbAccessMode       mod  );

    /// Set domain specific options
    /** @param refOpt   [IN]  Reference to option object
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus setOption(const DbOption& refOpt);

    /// Access domain specific options
    /** @param refOpt   [IN]  Reference to option object
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus getOption(DbOption& refOpt) const;
  };
}      // End namespace pool
#endif // POOL_DBDOMAINIMP_H
