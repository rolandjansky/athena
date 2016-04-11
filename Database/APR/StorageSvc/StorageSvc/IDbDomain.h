/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
// $Id: IDbDomain.h 726071 2016-02-25 09:23:05Z krasznaa $
//
//  Package    : pool/StorageSvc (The pool framework)
//
//  @author      M.Frank
//
//====================================================================
#ifndef POOL_IDBDOMAIN_H
#define POOL_IDBDOMAIN_H
// $Header: /cvs/PF/pool/StorageSvc/StorageSvc/IDbDomain.h,v 1.12 2007/11/28 16:19:32 frankb Exp $

// Framework include files
#include "StorageSvc/pool.h"

/*
 *  POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class DbSession;
  class DbOption;
  class DbType;

  /** @class IDbDomain IDbDomain.h StorageSvc/IDbDomain.h
    * 
    * IDbDomain interface
    *
    * Description:
    * Interface to the implementation specific part of a 
    * Database domain object.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class IDbDomain   {
  protected:
    /// Destructor (called only by sub-classes)
    virtual ~IDbDomain()   {     }

  public:
    /// Release implementation object. Object may no longer be used after call
    virtual void release()  = 0;

    /// Check for existence of a domain within a session
    /** @param nam  [IN] name of the database in question
      *
      * @return boolean value indicating the accessability of the database.
      */
    virtual bool existsDbase(const std::string& nam) = 0;

    /// Close domain
    /** @return DbStatus code indicating success or failure.      */
    virtual DbStatus close() = 0;

    /// Open domain
    /** @param sessionH [IN]  Handle to valid session object
      *                       (validity ensured by upper levels).
      * @param nam      [IN]  Name of the domain to be opened.
      * @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus open(const DbSession&    sessionH, 
                        const std::string&  nam, 
                        DbAccessMode        mode) = 0;

    /// Set domain specific options
    /** @param refOpt   [IN]  Reference to option object
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus setOption(const DbOption& refOpt) = 0;

    /// Access domain specific options
    /** @param refOpt   [IN]  Reference to option object
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus getOption(DbOption& refOpt) const = 0;
  };
}      // End namespace pool
#endif // POOL_IDBDOMAIN_H
