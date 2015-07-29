/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IClassLoader.h 590734 2014-04-01 21:49:36Z gemmeren $
//
//  Package    : pool/StorageSvc (The pool framework)
//
//  @author      M.Frank
//
//====================================================================
#ifndef POOL_ICLASSLOADER_H
#define POOL_ICLASSLOADER_H
// $Header: /cvs/PF/pool/StorageSvc/StorageSvc/IClassLoader.h,v 1.19 2008/05/13 14:39:31 witoldp Exp $

// Framework include files
#include "StorageSvc/pool.h"

// C++ include files
#include <vector>
#include <string>

/*
 *   POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class IClassHandler;

  /** @class IClassLoader IClassLoader.h StorageSvc/IClassLoader.h
    *
    * Definition of the IClassLoader class.
    * 
    * Description:
    *  Object definition or a class, which handles the dynamic loading
    *  of reflection information for accessing persistent storage.
    *
    * @author  Markus Frank                                               
    * @version 1                                                         
    */
  class IClassLoader  {
  protected:
    /// Destructor (called only by sub-classes)
    virtual ~IClassLoader()   {     }

  public:
    /// Release resource
    virtual void release() = 0;

    /// Access to the class handler by it's LCG class name
    /** @param name    [IN]  Class name as it is known to the LCG dictionary.
      * @param force   [IN]  Flag to force the class loading.
      * @return Pair with the pointers to the LCG and native class description.
      */
    virtual IClassHandler* handler( const std::string& name,
                                    bool force_load=true) = 0;

    /// Access to the class handler by it's native class name
    /** @param name    [IN]  Class name as it is known to the native dictionary.
      * @param force   [IN]  Flag to force the class loading.
      * @return Pair with the pointers to the LCG and native class description.
      */
    virtual IClassHandler* nativeHandler( const std::string& name,
                                          bool force_load=true) = 0;
    /// Set a user handler in place to deal with special class treatment
    /** @param lcg_name [IN]  Class name as it is known to the LCG dictionary.
      * @param nat_name [IN]  Class name as it is known to the native dictionary.
      * @param hndl     [IN]  Pointer to class handler
      * @return Status code indicating success or failure.
      */
    virtual DbStatus setHandler(      const std::string& lcg_name, 
                                      const std::string& nat_name, 
                                      IClassHandler* hndl) = 0;

    /// Remove a user handler
    /** @param name    [IN]  Class name as it is known to the LCG dictionary.
      * @return Status code indicating success or failure.
      */
    virtual DbStatus removeHandler(   const std::string& cl_name) = 0;
  };
}      // End namespace pool
#endif // POOL_ICLASSLOADER_H
