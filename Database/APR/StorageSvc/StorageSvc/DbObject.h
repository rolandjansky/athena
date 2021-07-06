/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  DbObject definition
//--------------------------------------------------------------------
//
//  Package    : (The POOL project)
//
//  Description: Generic persistable object
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBOBJECT_H
#define POOL_DBOBJECT_H 1

// Framework include files
#include "PersistentDataModel/Token.h"
#include "StorageSvc/pool.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/DbHandleBase.h"
#include "CxxUtils/checker_macros.h"

/*
 * POOL namespace declaration
 */
namespace pool  {
  // forward declarations
  class DbObjectAccessor;
  class DbTypeInfo;
  class DbContainer;
  typedef const class Shape *ShapeH;
}

#ifdef POOL_HAVE_BASE

namespace pool  {

  /** @class DbObject DbObject.h StorageSvc/DbObject.h
    *
    * Definition of the DbObject class (generic persistable object).
    *
    * - Class:      Object
    * - Purpose:    
    *  Basic object for storage of identifiable persistent data.
    *  The DbObject is able to keep track of persistent links 
    *  between objects.
    *
    *
    * @author  Markus Frank                                               
    * @version 1.0
    */
  class DbObject    {
  protected:
    /// Persistent objects only live with reference counting!
    virtual ~DbObject();
  public:
    /// operator new for purely heap based objects
    void* operator new(size_t size);
    /// Need to overload operator delete after special instantiation
    void operator delete(void* ptr);
    /// operator new for the creation of objects using clustering hint
    void* operator new(size_t siz, DbContainer& cntH);
    /// Exception enabled compilation needs this delete operator
    void operator delete(void *ptr, DbContainer& cntH);
    /// Standard destructor
    DbObject();
  };
}
#else

/// operator new for the creation of objects using clustering hint
void* operator new(size_t siz, pool::DbContainer& cntH, const pool::DbTypeInfo* typ);
/// operator new for the creation of objects using clustering hint
void* operator new(size_t siz, pool::DbContainer& cntH, const void *ptr, pool::ShapeH shape);
#endif


namespace pool {

  /** @class DbObjectHandle DbObject.h StorageSvc/DbObject.h
    *
    * Description:
    * Base class for object handles.
    *
    * @author  M.Frank
    * @version 1.0
    */
  template <class USER> class DbObjectHandle : public DbHandleBase<USER> {
  private:
    typedef DbHandleBase<USER> Base;
    /// Pointer conversion
    const USER* makePtr(void* p)                {  return (const USER*)(p); }
  protected:
    /// Attach object to handle with type: may be accessed by sub-class only
    template<typename T> void _set(const T* ptr, const DbType& typ) {
      _setObject(ptr);
      _setType(typ);
    }
  public:
    /// Set handle type
    void _setType(const DbType& type)     { Base::setType(type);       }
    /// Set object value
    template<class T> void _setObject(T* p)  {
      if ( 0 == Base::ptr() && 0 == p ) return;
      Base::m_ptr =  p;
    }
    void _setObject(const int /* null_obj */ )  { 
      if ( 0 == Base::ptr() ) return;
      Base::m_ptr = 0;
    }
  public:
    /// Standard destructor
    virtual ~DbObjectHandle()                 { _setObject(0);              }
    /// Standard Constructor
    DbObjectHandle()                          {                             }
    /// Constructor with storage type
    DbObjectHandle(const DbType& typ)         { _setType(typ);              }
    /// Constructor with object pointer
    DbObjectHandle(USER* p)                   { _setObject(p);              }
    /// Copy constructor
    DbObjectHandle(const DbObjectHandle<USER>& c) : DbHandleBase<USER>()
    { _set(c.ptr(),c.type());                                               }
    /// Generic assignment operator from base class
    operator const USER*() const                  { return Base::ptr();     }
    operator USER*()                              { return Base::ptr();     }
    /// Generic assignment operator
      DbObjectHandle<USER>& operator=(const int /* nuller */)
    { _setObject(0); return *this;                                          }
      DbObjectHandle<USER>& operator=(const DbObjectHandle& c)
        {
          if (&c != this)
            _set (c.ptr(), c.type());
          return *this;
        }
    /// Equality operator
    template <typename T> bool operator==(const DbHandleBase<T>& objH) const
    { return Base::type() == objH.type() && Base::ptr() == objH.ptr();      }
    /// Retrieve hosting container
    const DbContainer& containedIn() const;
    /// Access object oid
    const Token::OID_t& oid() const;
    /// Access object oid
    Token::OID_t& oid();
    /// Add persistent association entry
    DbStatus makeLink ATLAS_NOT_THREAD_SAFE (const Token* pToken, Token::OID_t& linkH) const;
  };


  template <class USER> class DbHandle : public DbObjectHandle< USER> {
    typedef DbObjectHandle< USER> Handle;
  public:
    /// Standard destructor
    virtual ~DbHandle()                           {                         }
    /// Default constructor
    DbHandle()                                    {                         }
    /// Constructor with storage type
    DbHandle(const DbType& typ)                   { Handle::_setType(typ);  }
    /// Constructor with object assignment
    DbHandle(USER* obj)                           { Handle::_setObject(obj);}
    /// Copy constructor
    template <typename T> DbHandle(const DbObjectHandle<T>& c)
      : DbObjectHandle<USER>()                    
    { Handle::_set(c.ptr(),c.type());                                       }
    /// Copy constructor
    DbHandle(const DbHandle<USER>& c) : DbObjectHandle<USER>()
    { Handle::_set(c.ptr(),c.type());                                       }
    /// Generic assignment operator
    DbHandle<USER>& operator=(DbHandle<USER>& c) {
      if ( this != &c ) Handle::_set(c.ptr(),c.type());
      return *this;
    }
    /// Generic assignment operator
    DbHandle<USER>& operator=(USER* obj) {
      if ( Handle::ptr() != obj ) Handle::_setObject(obj);
      return *this;
    }
    /// Generic assignment operator
    template <typename T> 
    DbHandle<USER>& operator=(DbObjectHandle<T>& c) {
      if ( Handle::ptr() != c.ptr() ) Handle::_set(c.ptr(), c.type());
      return *this;
    }
  };

  /// No void handles allowed: type must be polymorphic!
#ifndef POOL_HAVE_BASE
  template <> class DbHandle<DbObject>            {};
#endif
}

/*
 *
 *  Inline object handle implementation
 *
 */
#include "StorageSvc/DbObjectAccessor.h"

namespace pool {
#ifndef __no_inline

  /// Retrieve hosting container
  template <class T> inline
  const DbContainer& DbObjectHandle<T>::containedIn() const  
  { return DbObjectAccessor::containedIn(Base::ptr());                   }

  /// Access object oid
  template <class T> inline
  Token::OID_t& DbObjectHandle<T>::oid()
  { return DbObjectAccessor::objectOid(Base::ptr());                     }

  /// Access object oid
  template <class T> inline
  const Token::OID_t& DbObjectHandle<T>::oid() const  
  { return DbObjectAccessor::objectOid(Base::ptr());                     }

  /// Add persistent association entry
  template <class T> inline
  DbStatus DbObjectHandle<T>::makeLink ATLAS_NOT_THREAD_SAFE (const Token* pToken, Token::OID_t& linkH) const 
  { return DbObjectAccessor::makeObjectLink(Base::ptr(), pToken, linkH); }

#endif
}       // End namespace pool
#endif  // POOL_DBOBJECT_H
