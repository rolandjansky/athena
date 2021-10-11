/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
// $Id: IDbDatabase.h 726071 2016-02-25 09:23:05Z krasznaa $
//
//  Package    : pool/StorageSvc (The pool framework)
//
//  @author      M.Frank
//
//====================================================================
#ifndef POOL_IDBDATABASE_H
#define POOL_IDBDATABASE_H
// $Header: /cvs/PF/pool/StorageSvc/StorageSvc/IDbDatabase.h,v 1.18 2007/11/28 16:19:32 frankb Exp $

// Framework include files
#include "StorageSvc/pool.h"
#include "StorageSvc/Transaction.h"

/*
 *  POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class DbOption;
  class DbDomain;
  class DbDatabase;
  class DbContainer;

  /** @class IDbDatabase IDbDatabase.h StorageSvc/IDbDatabase.h
    *
    * IDbDatabase interface
    *
    * Description:
    * Interface to the implementation specific part of a Database object
    *
    * @author  M.Frank
    * @version 1.0
    */
  class IDbDatabase    {
  public:
    virtual ~IDbDatabase()   { }

    /// Access the size of the database: May be undefined for some technologies
    virtual long long int size()  const = 0;

    /// Set options
    /** @param refOpt   [IN]  Reference to option object.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus setOption(const DbOption& refOpt) = 0;

    /// Access options
    /** @param refOpt   [IN]  Reference to option object.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus getOption(DbOption& refOpt) const = 0;

    /// Close database access
    /** @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus close(DbAccessMode mode)  = 0;

    /// Open Database object
    /** @param domH     [IN]  Handle to valid domain object
      *                       (validity ensured by upper levels).
      * @param nam      [IN]  Name of the database to be opened.
      * @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus open(const DbDomain&     domH, 
                          const std::string&  nam, 
                          DbAccessMode        mode)  = 0;

    /// Callback after successful open of a database object
    /** @param dbH      [IN]  Handle to valid database object
      * @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus onOpen(DbDatabase& dbH, 
                            DbAccessMode      mode) = 0;

    /// Re-open database with changing access permissions
    /** @param mode     [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.  
      */
    virtual DbStatus reopen(DbAccessMode mode) = 0;

    /// Execute Database Transaction action
    /** @param   action     [IN]  action to perform
      * @return Status code indicating success or failure.
      */
    virtual DbStatus transAct(Transaction::Action action) = 0;
 };
}      // End namespace pool
#endif // POOL_IDBDATABASE_H
