/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbObjectSet.h 458019 2011-09-09 10:11:57Z mnowak $
//====================================================================
//  DbDomainObj object definition
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBDOMAINOBJ_H
#define POOL_DBDOMAINOBJ_H 1

#include "StorageSvc/pool.h"
#include <set>

/*
 *  POOL namespace declaration
 */
namespace pool    {

  // POOL forward declarations
  class DbTypeInfo;
  class DbObjectSetBase;

  class DbObjectSetBase  {
  protected:
    typedef void (*dtor_t)(void*);
    
    /// Object destructor
    dtor_t                m_destructor;
    /// Object type
    const std::type_info& m_type;
    /// Set of objects
    std::set<void*>       m_objects;

  public:
    /// Constructor
    DbObjectSetBase(dtor_t dtor, const std::type_info& type);
    /// Standard destructor
    virtual ~DbObjectSetBase();
    /// Printout
    void printOut();
    /// Remove single object
    void removeObject(void* ptr);
  };

  /**@class DbObjectSet DbObjectSet.h src/DbObjectSet.h

      Description:
      Set to hold objects for memory tracing.

      @author  M.Frank
      @version 1.0
  */
  template <class T> class DbObjectSet : public DbObjectSetBase  {
  private:
    static void __delete__(void* p) {
      T* q=(T*)p;
      if ( q ) delete q;
    }

  public:
    /// Constructor
    DbObjectSet();
    /// Constructor with destroy function
    DbObjectSet(dtor_t);
    /// Standard destructor
    virtual ~DbObjectSet();
    /// Add object to set
    void add(T* pObj);
    /// Remove single object from set
    void remove(T* pObj);
  };


  /// Constructor
  template <class T> inline 
  DbObjectSet<T>::DbObjectSet() 
  : DbObjectSetBase(__delete__, typeid(T)) {  }

  /// Constructor
  template <class T> inline 
  DbObjectSet<T>::DbObjectSet(dtor_t dt) 
  : DbObjectSetBase(dt, typeid(T)) {  }

  /// Standard destructor
  template<class T> inline DbObjectSet<T>::~DbObjectSet() { }

  /// Add object to set
  template<class T> inline void DbObjectSet<T>::add(T* pObj)  {
    m_objects.insert(pObj);
  }
  /// Remove single object from set
  template<class T> inline void DbObjectSet<T>::remove(T* pObj)  {
    removeObject(pObj);
  }
}      // End namespace pool
#endif // POOL_DBDOMAINOBJ_H
