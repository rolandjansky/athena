/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbAccessObj.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbAccessObj Base class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBACCESSOBJ_H
#define POOL_DBACCESSOBJ_H 1

// Framework include files
#include "StorageSvc/pool.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/IOODatabase.h"

// STL include files
#include <map>
#include <utility>

// #define _DEBUG_REFCOUNTS
#ifdef _DEBUG_REFCOUNTS
#  include<iostream>
#endif

/* 
 *  POOL namespace declaration
 */
namespace pool    {

  /** @class DbAccessObj DbAccessObj.h StorageSvc/DbAccessObj.h
    *
    * Description:
    *
    * Templated base class for "abstarct" container objects.
    * These abstarct containers are: DnSessions, DbDomainObjs,
    * DbDatabaseObjs and DbContainerObjs.
    *
    * @author  M.Frank
    * @version 1.0
    */
  template <class KEY, class TYPE> class DbAccessObj {
  public:
    /// Type definitions
    typedef DbAccessObj< KEY , TYPE >     Base;
    typedef std::map< KEY, TYPE* >        Keys;
    typedef typename Keys::iterator       iterator;
    typedef typename Keys::const_iterator const_iterator;
  private:
    /// Reference counter
    mutable int           m_refCount;
    /// Access mode
    DbAccessMode          m_mode;
    /// Name of the instance
    std::string           m_name;
    /// Database type
    DbType                m_type;
    /// Key entry buffer
    Keys                  m_keys;
    /// Pointer to specific pool implementation
    IOODatabase*          m_pool;

  public:
    /// Constructor with initializing arguments
    DbAccessObj(const std::string& n, DbAccessMode m, const DbType& t, IOODatabase* s=0)
    : m_refCount(0), m_mode(m), m_name(n), m_type(t), m_pool(s)
    { if( m_pool )  m_pool->addRef(); }
    /// Standard destructor
    virtual ~DbAccessObj()            {      releasePtr( m_pool );    }
    /// Access the instance name
    const std::string& name()  const  {      return m_name;           }
    /// Access the instance name
    void setName(const std::string& n){      m_name = n;              }
    /// Access mode
    DbAccessMode mode()   const       {      return m_mode;           }
    /// Set Access mode
    void setMode(DbAccessMode m)      {      m_mode = m;              }
    // Inline functions
    const DbType& type()  const       {      return m_type;           }
    /// Allow access to the Database implementation
    IOODatabase* db()         const   {      return m_pool;           }
    /// Access reference counter
    int refCount()   const            {      return m_refCount;       }
    /// Add reference count
    int addRef() const  {
#ifdef _DEBUG_REFCOUNTS
      std::cout << typeid(*this).name() 
                << "  " 
                << m_name 
                << " Refcount:" 
                << m_refCount+1  
                << std::endl;
#endif
      return ++m_refCount;
    }
    /// Remove reference count
    int release()  const   {
      int count = --m_refCount;
#ifdef _DEBUG_REFCOUNTS
      std::cout << typeid(*this).name() 
                << "  " 
                << m_name 
                << " Refcount:" 
                << count  
                << std::endl;
#endif
      if ( m_refCount == 0 )    {
        delete this;
      }
      return count;
    }
    /// Object size
    size_t size()  const    {
      return m_keys.size();
    }
    /// Object cleanup: remove all entries
    DbStatus clearEntries()   {
      Keys k = m_keys;
      for(iterator j = k.begin(); j != k.end(); ++j )  {
        (*j).second->release();
      }
      m_keys.clear();
      return Success;
    }
    /// Find object by key (CONST)
    const TYPE* find(const KEY& key)  const   {
      const_iterator i = m_keys.find(key);
      return (m_keys.end() == i) ? 0 : (*i).second;
    }
    /// Find object by key
    TYPE* find(const KEY& key)    {
      iterator i = m_keys.find(key);
      return (m_keys.end() == i) ? 0 : (*i).second;
    }
    /// Add entry to container
    DbStatus add(const KEY& key, TYPE* val)   {
      iterator i = m_keys.find(key);
      if ( m_keys.end() == i )    {
        m_keys.insert(std::make_pair(key, val));
        val->addRef();
        return Success;
      }
      return Error;
    }
    /// Remove entry from container
    DbStatus remove(const TYPE* val)    {
      for (iterator j = m_keys.begin(); j != m_keys.end(); ++j )    {
        if ( (*j).second == val ) {
          TYPE* p = (*j).second;
          m_keys.erase(j);
          p->release();
          return Success;
        }
      }
      return Error;
    }
    iterator begin()              {      return m_keys.begin();   }
    const_iterator begin()  const {      return m_keys.begin();   }
    iterator end()                {      return m_keys.end();     }
    const_iterator end()  const   {      return m_keys.end();     }
  };
}      // End namespace pool
#endif // POOL_DBACCESSOBJ_H
