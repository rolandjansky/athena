/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
// $Id: IDbSession.h 726071 2016-02-25 09:23:05Z krasznaa $
//
//  Package    : pool/StorageSvc (The pool framework)
//
//  @author      M.Frank
//
//====================================================================
#ifndef POOL_IDBSESSION_H
#define POOL_IDBSESSION_H
// $Header: /cvs/PF/pool/StorageSvc/StorageSvc/IDbSession.h,v 1.13 2007/11/28 16:19:33 frankb Exp $

// Framework include files
#include "StorageSvc/pool.h"

/*
 *  POOL namespace declaration
 */
namespace pool    {

  /** @class class IDbSession IDbSession.h StorageSvc/IDbSession.h
    *
    * IDbSession interface
    *
    * Description:
    * Interface to the implementation specific part of a
    * Database session object
    *
    * @author  M.Frank
    * @version 1.0
    */
  class IDbSession    {
  protected:
    /// Destructor (called only by sub-classes)
    virtual ~IDbSession()   {     }

  public:
    /// Release (delete) object
    virtual void release() = 0;

    /// Open Database session
    /** @param mode  [IN]  Desired session access mode.
      *
      * @return DbStatus code indicating success or failure.
      */
    virtual DbStatus open(DbAccessMode mode) = 0;

    /// Close Database session
    /** @return DbStatus code indicating success or failure.      */
    virtual DbStatus close() = 0;
  };
}      // End namespace pool
#endif // POOL_IDBSESSION_H
