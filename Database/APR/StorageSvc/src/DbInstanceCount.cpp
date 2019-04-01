/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbInstanceCount.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  DbInstanceCount implementation
//--------------------------------------------------------------------
//
//  Package    : System (The POOL project)
//
//  Description: Allow for internal object tracing
//
//  @author      M.Frank
//====================================================================

#include "PersistentDataModel/Token.h"
#include "StorageSvc/DbInstanceCount.h"
#include "StorageSvc/DbObjectSet.h"
#include "StorageSvc/DbTypeInfo.h"
#include <iostream>
#include <iomanip>
#include <memory>
#include <cstdio>
#include <map>

using namespace pool;
using namespace std;
namespace pool  {
  const string typeName(const type_info& typ);
  void delete_DbObjectSetBase(void*) {  
  }
}

// Do not clutter global namespace
namespace {
  typedef DbInstanceCount::Counter      COUNT;
  typedef map<const type_info*, COUNT*> TypeCounter;
  typedef map<const string*,    COUNT*> StringCounter;
  typedef DbObjectSet<DbObjectSetBase>  ContCounter;

  bool s_trace_instances = false;
  bool s_trace_init = false;
  
  bool trace_instances()
  {
    if (!s_trace_init)
      s_trace_instances = (0 != ::getenv("POOL_TRACE")) || (0 != ::getenv("POOLDB_TRACE"));
    return s_trace_instances;
  }

  ContCounter& contCounter() {
    static ContCounter       s_cntCounts(delete_DbObjectSetBase);
    return s_cntCounts;
  }
  static auto_ptr<TypeCounter>       s_typCounts(new TypeCounter());
  static auto_ptr<StringCounter>     s_strCounts(new StringCounter());
  static DbInstanceCount::Counter s_nullCount;
  static DbInstanceCount::Counter s_thisCount;
  static DbInstanceCount          s_counter;
  inline TypeCounter& types()      {    return *(s_typCounts.get());  }
  inline StringCounter& strings()  {    return *(s_strCounts.get());  }
}

DbObjectSetBase::DbObjectSetBase(dtor_t dt, const type_info& t) 
: m_destructor(dt), m_type(t)  {
  if ( m_destructor != delete_DbObjectSetBase ) {
    contCounter().add(this);
  }
}

DbObjectSetBase::~DbObjectSetBase()   {    
  set<void*>::iterator i=m_objects.begin();
  printOut();
  for( ; i != m_objects.end(); i = m_objects.begin() )  {
    void* obj = *i;
    m_objects.erase(i);
    if ( m_destructor )  (*m_destructor)(obj);
  }
  if ( m_destructor != delete_DbObjectSetBase ) {
    contCounter().remove(this);
  }
}

void DbObjectSetBase::printOut()  {
  if ( trace_instances() && m_type != typeid(DbObjectSetBase) )  {
    cout << "Removal on unload of " << m_objects.size() << " instances of type:" 
              << typeName(m_type) << endl;       
  }
}

void DbObjectSetBase::removeObject(void* pObj)  {
  set<void*>::iterator i=m_objects.find(pObj);
  if ( i != m_objects.end() )  {
    m_objects.erase(i);
  }
}

/// Standard Constructor
DbInstanceCount::DbInstanceCount() {
  s_thisCount.increment();
}

/// Standard destructor
DbInstanceCount::~DbInstanceCount() {
  s_thisCount.decrement();
  if ( 0 == s_thisCount.value() )   {
    StringCounter::iterator i;
    TypeCounter::iterator   j;
    dump(trace_instances() ? ALL : NONE);
    for(i=s_strCounts->begin(); i != s_strCounts->end(); ++i)
      delete (*i).second;
    for(j=s_typCounts->begin(); j != s_typCounts->end(); ++j)
      delete (*j).second;
    s_strCounts->clear();
    s_typCounts->clear();
  }
}

/// Check if tracing is enabled.
bool DbInstanceCount::doTrace()    {
  return trace_instances();
}

/// Enable/Disable tracing
void DbInstanceCount::doTracing(bool value)    {
  s_trace_instances = value;
  s_trace_init = true;
}

/// Access counter object for local caching on optimizations
DbInstanceCount::Counter* DbInstanceCount::getCounter(const type_info& typ)  {
  Counter* cnt = trace_instances() ? types()[&typ] : &s_nullCount;
  return (0!=cnt) ? cnt : types()[&typ] = new Counter();
}

/// Access counter object for local caching on optimizations
DbInstanceCount::Counter* DbInstanceCount::getCounter(const string& typ)  {
  Counter* cnt = trace_instances() ? strings()[&typ] : &s_nullCount;
  return (0!=cnt) ? cnt : strings()[&typ] = new Counter();
}

/// Force dump of counters
void DbInstanceCount::dump(int typ)  {
  bool need_footer = false;
  if ( (typ & STRING) && s_strCounts.get() )    {
    if ( s_strCounts->begin() != s_strCounts->end() )   {
      StringCounter::const_iterator i;
      cout << "+--------------------------------------------------------------+" << endl;
      cout << "| I n s t a n c e   c o u n t e r s   b y    N A M E           |" << endl;
      cout << "+----------+-------+-------------------------------------------+" << endl;
      cout << "|   Total  | Diff  |      Type identifier                      |" << endl;
      cout << "+----------+-------+-------------------------------------------+" << endl;
      for(i=s_strCounts->begin(); i != s_strCounts->end(); ++i) {
        cout << "|" << setw(10) << (*i).second->total()
                  << "|" << setw(7)  << (*i).second->value()
                  << "|" << (*i).first << endl;       
      }
      need_footer = true;
    }
  }
  if ( (typ & TYPEINFO) && s_typCounts.get() )   {
    if ( s_typCounts->begin() != s_typCounts->end() )  {
      TypeCounter::const_iterator i;
      cout << "+--------------------------------------------------------------+" << endl;
      cout << "| I n s t a n c e   c o u n t e r s   b y    T Y P E I N F O   |" << endl;
      cout << "+----------+-------+-------------------------------------------+" << endl;
      cout << "|   Total  | Diff  |      Type identifier                      |" << endl;
      cout << "+----------+-------+-------------------------------------------+" << endl;
      for(i=s_typCounts->begin(); i != s_typCounts->end(); ++i) {
        cout << "|" << setw(10) << (*i).second->total()
                  << "|" << setw(7)  << (*i).second->value()
                  << "|" << typeName(*((*i).first)) << endl;       
      }
      need_footer = true;
    } 
  }
  if ( need_footer )  {
    cout << "|" << setw(10) << "<Unknown>"
         << "|" << setw(7)  << Token::numInstances()
              << "|" << "Token" << endl;       
    cout << "+----------+-------+-------------------------------------------+" << endl;
  }
}
