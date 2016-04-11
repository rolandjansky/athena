/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbHandleBase.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  Pool Handle Base class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBHANDLEBASE_H
#define POOL_DBHANDLEBASE_H 1

// Framework include files
#include "StorageSvc/DbType.h"

/*
 *  POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  template <class T> class DbHandleBase;

  /** @class DbHandleBase DbHandleBase.h StorageSvc/DbHandleBase.h
    *
    * Description:
    * Definition of the generic database object handle.
    *
    * @author  M.Frank
    * @version 1.0
    */
  template <class T> class DbHandleBase   {
  protected:
    /// Data type definition
    typedef  T                       _DataType;
    /// Data type definition
    typedef  DbHandleBase<_DataType> Base;
    /// Data member: Object pointer. Sub-classes need access on re-assignment
    mutable  T*                      m_ptr;
    /// Data member: Technology type. Sub-classes need access on re-assignment
    mutable  DbType                  m_type;

    /// Standard destructor
    ~DbHandleBase() {  m_type.check();                  }
    /// Standard constructor
    DbHandleBase() : m_ptr(0), m_type(0)  {             }
    /// Set handle type
    void setType(const DbType& typ) const { m_type=typ; }
    /// Set data pointer
    void setPtr(T* ptr) const             { m_ptr=ptr;  }

  public:
    /// Dereference operator
    T* operator->() const         { return m_ptr;       }
    /// Validity check through operator NOT
    bool operator!() const        { return !isValid();  }
    /// Access to underlying object
    T* ptr() const                { return m_ptr;       }
    /// Validity check (Objy like)
    bool isValid() const          { return 0 != m_ptr;  }
    /// Inquire storage type of the handle
    const DbType& type() const    { return m_type;      }
  };
}       // End namespace pool
#endif  // POOL_DBHANDLEBASE_H
