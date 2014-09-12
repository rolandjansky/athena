/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  Generic class handler definition
//--------------------------------------------------------------------
//
//  Package    : POOLCore/StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_STORAGESVC_DBCLASSHANDLER_H
#define POOL_STORAGESVC_DBCLASSHANDLER_H

#include "StorageSvc/IClassHandler.h"

/*
 * pool namespace 
 */
namespace pool  {
  // Forward declarations
  class IOODatabase;

  /** class DbClassHandler, DbClassHandler.h, POOLCore/DbClassHandler.h
    *
    * Description:
    * The class handler is called on loading and saving objects.
    * An overridden implementation allows to trigger callbacks
    * to users.
    *
    * @author  M.Frank
    * @date    1/8/2002
    * @version 1.0
    */
  class DbClassHandler : virtual public IClassHandler  {
  private:
    /// Reference to reflection class
    TypeH                 m_class;
    /// Reference to native class
    void*                 m_native;

  public:
    /// Constructor with initializing arguments
    DbClassHandler(const TypeH& cl, void* nat)
      : m_class(cl), m_native(nat)        {                    }
    /// Standard destructor
    virtual ~DbClassHandler()             {                    }
    /// IClassHandler implementation: Release resource
    virtual void release()                { delete this;       }
    /// Access to reflection information (if avalible)
    virtual TypeH reflectionClass() const { return m_class;    }
    /// Access to the native storage technology dependent information
    virtual void* nativeClass() const     { return m_native;   }
    /// Callback when this class is loaded
    virtual void onLoad(void* /* obj */)  {                    }
    /// Callback, when this class is saved
    virtual void onSave(void* /* obj */)  {                    }
  };
}      // End namespace pool
#endif // POOL_STORAGESVC_DBCLASSHANDLER_H
