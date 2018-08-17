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
  public:
    virtual ~IDbDomain()   { }
    
    /// Check for existence of a domain within a session
    /** @param nam  [IN] name of the database in question
      *
      * @return boolean value indicating the accessability of the database.
      */
    virtual bool existsDbase(const std::string& nam) = 0;

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
