/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IClassHandler.h 601961 2014-06-16 14:49:09Z mnowak $
//
//  Package    : pool/StorageSvc (The pool framework)
//
//  @author      M.Frank
//
//====================================================================
#ifndef POOL_ICLASSHANDLER_H
#define POOL_ICLASSHANDLER_H
// $Header: /cvs/PF/pool/StorageSvc/StorageSvc/IClassHandler.h,v 1.14 2009/01/14 19:03:53 frankb Exp $

#include "DataModelRoot/RootType.h"

/*
 *   POOL namespace declaration
 */
namespace pool    {

  /** @class IClassHandler IClassHandler.h StorageSvc/IClassHandler.h
    *
    * Description:
    * Definition of the IClassHandler class.
    * <P> Class:      IClassHandler
    * <P> Purpose:    
    *  Object definition or a class, which handles special 
    *  object treatment of a given class during the object save
    *  and object load.
    *
    * @author  Markus Frank                                               
    * @version 1                                                         
    */
  class IClassHandler  {
  protected:
    /// Destructor (called only by sub-classes)
    virtual ~IClassHandler()   {     }
  public:
    /// Reflection class declaration
    typedef RootType TypeH;
    /// Release resource
    virtual void release() = 0;
    /// Access to reflection information (if avalible)
    virtual TypeH reflectionClass() const = 0;
    /// Access to the native storage technology dependent information
    virtual void* nativeClass() const = 0;
    /// Callback when this class is loaded
    virtual void onLoad(void* obj) = 0;
    /// Callback, when this class is saved
    virtual void onSave(void* obj) = 0;
  };
}      // End namespace pool
#endif // POOL_ICLASSHANDLER_H
