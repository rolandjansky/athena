/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbIter.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbIter definition
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  Description: Generic persistable object iterator
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBITER_H
#define POOL_DBITER_H

// Framework include files
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbContainer.h"

// STL include files
#include <memory>

/*
 *  POOL namespace declaration
 */
namespace pool  {

  /** @class DbIter<T> DbIter.h StorageSvc/DbIter.h
    *
    * Definition of the DbIter class (generic persistable object iterator).
    *
    * <P> Class:      DbIter
    * <P> Purpose:    
    * Object definition to access objects within a container.
    *
    * @author  Markus Frank                                               
    * @version 1                                                         
    */
  template <class T> class DbIter   {
    /// Handle to container object
    DbContainer       m_cnt;
    /// Handle to current object
    DbHandle<T>       m_obj;
    /// Current cursor for the loop
    Token             m_token;
    /// Object type used for this scan
    const DbTypeInfo* m_type;
    /// Data access type (Default to READ)
    DbAccessMode      m_mode;

  private:
    /// Copy constructor
    DbIter(const DbIter& c) : m_obj(c.m_obj), m_token(c.m_token), m_type(c.m_type),
      m_mode(c.m_mode)                        {                           }
    /// Equality operator
    bool operator==(const DbIter<T>& iter)   const 
    {    return m_obj.ptr() == iter.m_obj.ptr();                          }
    /// Assignment operator
    DbIter& operator=(const DbIter& copy)     {
      m_cnt     = copy.m_cnt;
      m_obj     = copy.m_obj;
      m_mode    = copy.m_mode;
      m_token   = copy.m_token;
      m_type    = copy.m_type;
      return *this;
    }
  public:
    /// Constructor with initializing arguments
    DbIter(DbAccessMode mode=pool::READ) : m_token(), m_type(0), m_mode(mode) {        }
    /// Standard destructor
    virtual ~DbIter()                   {                                     }
    /// Access to container handler
    const DbContainer& container() const{    return m_cnt;                    }
    /// Check validity
    bool isValid()  const               {    return m_obj.isValid();          }
    /// Dereference operator
    DbHandle<T>& operator->()           {    return m_obj;                    }
    /// Dereference operator
    DbHandle<T>& operator*()            {    return m_obj;                    }
    /// Access to current object handle
    DbHandle<T>& handle()               {    return m_obj;                    }
    /// Access to current object handle (CONST)
    const DbHandle<T>& handle() const   {    return m_obj;                    }
    /// Dereference operator
    T* object()  const                  {    return m_obj.ptr();              }
    /// Access to cursor token
    const Token* token() const          {    return &m_token;                 }
    /// Scan the container
    DbStatus scan(const DbContainer& cntH, const DbTypeInfo* typ);
    /// Retrieve next element
    DbStatus next();
  };

  /// No void objects allowed on the user side!
  template <> class DbIter<DbObject>   {};

} // End namespace pool


/// Scan the container
template <class T> inline 
pool::DbStatus pool::DbIter<T>::scan(const DbContainer& cntH, 
                                     const DbTypeInfo* typ) 
{
  if ( cntH.isValid() )   {
    m_type   = typ;
    m_cnt    = cntH;
  }
  m_obj = DbHandle<T>(cntH.type());

  return cntH.isValid()? Success : Error;
}

        
/// Retrieve next element of iteration
template <class T> inline pool::DbStatus pool::DbIter<T>::next()
{
  DbObjectHandle<DbObject> objH(m_obj.ptr());
  objH._setType(m_obj.type());
  m_token.oid().second++;
  if ( m_cnt.loadNext(objH, m_token.oid(), m_type).isSuccess() ) {
    m_obj._setObject(static_cast<T*>(objH.ptr()));
    return Success;
  }
  return Error;
}

#endif  // POOL_DBITER_H
