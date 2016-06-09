/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_CONDCONT_H
#define IOVSVC_CONDCONT_H 1

#include <iostream>
#include <set>
#include <vector>
#include <typeinfo>
#include <mutex>
#include <typeinfo>

#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"

#include "AthenaKernel/IOVEntryT.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/DataObjID.h"

namespace SG {
  class DataProxy;
  template<class T>
  class ReadCondHandle;
  template<class T>
  class WriteCondHandle;
}

class CondContBase {
public:
  CondContBase() {};
  virtual ~CondContBase(){};

  //  virtual void list() const { std::cout << "CCBase" << std::endl; }
  virtual void list(std::ostringstream&) const = 0;
  virtual int entries() const { return 0; }
  virtual bool valid( const EventIDBase& t) const = 0;
  virtual const DataObjID& id() const = 0;
  virtual bool insert(const EventIDRange& r, DataObject* obj) = 0;
  virtual bool insert(const EventIDRange& r, void* obj) = 0;

  virtual SG::DataProxy* proxy() const = 0;
  virtual void setProxy(SG::DataProxy*) = 0;

  virtual bool range(const EventIDBase&, EventIDRange&) const = 0;
  virtual std::vector<EventIDRange> ranges() const = 0;

private:
};

///////////////////////////////////////////////////////////////////////////

template <typename T>
class CondCont: public CondContBase {
public:

  friend SG::ReadCondHandle<T>;
  friend SG::WriteCondHandle<T>;

  CondCont(const DataObjID& id, SG::DataProxy* prx=nullptr):
    m_id(id), m_proxy(prx) {};
  ~CondCont();

  bool insert(const EventIDRange& r, DataObject* t);
  bool insert(const EventIDRange& r, void* t);
  bool insert(const EventIDRange& r, T* t);

  bool find(const EventIDBase& t, T*&) const;

  virtual void list(std::ostringstream&) const;
  virtual int entries() const;

  virtual bool valid( const EventIDBase& t ) const;

  virtual const DataObjID& id() const { return m_id; }

  virtual SG::DataProxy* proxy() const { return m_proxy; }
  virtual void setProxy(SG::DataProxy* prx) { m_proxy = prx; }

  virtual bool range(const EventIDBase&, EventIDRange&) const;
  virtual std::vector<EventIDRange> ranges() const;

private:

  bool findEntry(const EventIDBase& t, const IOVEntryT<T>*&) const;

  bool isRE(const EventIDBase&) const;
  bool isTS(const EventIDBase&) const;

  mutable std::mutex m_mut;

  typedef std::set<IOVEntryT<T>, typename IOVEntryT<T>::IOVEntryTStartCritereon > CondContSet;
  CondContSet m_condSet_clock, m_condSet_RE;

  DataObjID m_id;

  SG::DataProxy* m_proxy;

};

//
///////////////////////////////////////////////////////////////////////////
//

template <typename T>
CondCont<T>::~CondCont<T>() {
  //  std::cout << "cleaning up - " << typeid( T ).name() << std::endl;
  
  for (auto itr=m_condSet_clock.begin(); itr != m_condSet_clock.end(); ++itr) {
    delete itr->objPtr();
  }
  m_condSet_clock.clear();
  for (auto itr=m_condSet_RE.begin(); itr != m_condSet_RE.end(); ++itr) {
    delete itr->objPtr();
  }
  m_condSet_RE.clear();
}    

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool CondCont<T>::insert(const EventIDRange& r, void* obj) {
  T* t = static_cast<T*>(obj);

  return insert(r, t);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool CondCont<T>::insert(const EventIDRange& r, DataObject* obj) {
  T* t = dynamic_cast<T*>( obj );

  if (t == 0) {
    std::cerr << "CondCont<>T unable to dcast from DataObject to " 
              << typeid(*t).name()
              << std::endl;
    return false;
  }

  return insert(r, t);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool CondCont<T>::insert(const EventIDRange& r, T* t) {

  std::lock_guard<std::mutex> lock(m_mut);

  if (r.start().isRunEvent() && r.stop().isRunEvent()) {
    m_condSet_RE.emplace( IOVEntryT<T>(t, r) );
  } else if (r.start().isTimeStamp() && r.stop().isTimeStamp()) {
    m_condSet_clock.emplace( IOVEntryT<T>(t,r) );
  } else {
    std::cerr << "CondCont<T>::insert error: EventIDRange " << r 
              << " is neither fully RunEvent nor TimeStamp" 
              << std::endl;
    return false;
  }

  return true;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool CondCont<T>::valid(const EventIDBase& it) const {
  typename CondContSet::const_iterator itr;
  
  std::lock_guard<std::mutex> lock(m_mut);

  if (it.isRunEvent()) {
    itr = m_condSet_RE.begin();
    for (; itr != m_condSet_RE.end(); ++itr) {
      // std::cout << "  -> " << itr->range() << " : " << *(itr->objPtr()) 
      //           << std::endl;
      if ( itr->range().isInRange( it ) ) {
        return true;
      }
    }
  } else if (it.isTimeStamp()) {
    itr = m_condSet_clock.begin();
    for (; itr != m_condSet_clock.end(); ++itr) {
      if ( itr->range().isInRange( it ) ) {
        return true;
      }
    }
  }
        
  return false;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool CondCont<T>::find(const EventIDBase& it, T*& t) const {
  //  typename std::set<IOVEntryT<T>, IOVEntryTComp<T> >::const_iterator itr;
  typename CondContSet::const_iterator itr;
  
  std::lock_guard<std::mutex> lock(m_mut);

  if (it.isRunEvent()) {
    itr = m_condSet_RE.begin();
    for (; itr != m_condSet_RE.end(); ++itr) {
      // std::cout << "  -> " << itr->range() << " : " << *(itr->objPtr()) 
      //           << std::endl;
      if ( itr->range().isInRange( it ) ) {
        t = itr->objPtr();
        return true;
      }
    }
  } else if (it.isTimeStamp()) {
    itr = m_condSet_clock.begin();
    for (; itr != m_condSet_clock.end(); ++itr) {
      if ( itr->range().isInRange( it ) ) {
        t = itr->objPtr();
        return true;
      }
    }
  }

  return false;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool CondCont<T>::findEntry(const EventIDBase& it, const IOVEntryT<T>*& t) const {
  typename CondContSet::const_iterator itr;
  
  std::lock_guard<std::mutex> lock(m_mut);

  if (it.isRunEvent()) {
    itr = m_condSet_RE.begin();
    for (; itr != m_condSet_RE.end(); ++itr) {
      if ( itr->range().isInRange( it ) ) {
        t = &(*itr);
        return true;
      }
    }
  } else if (it.isTimeStamp()) {
    itr = m_condSet_clock.begin();
    for (; itr != m_condSet_clock.end(); ++itr) {
      if ( itr->range().isInRange( it ) ) {
        t = &(*itr);
        return true;
      }
    }
  }

  return false;
}
    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
void CondCont<T>::list(std::ostringstream& ost) const {
  std::lock_guard<std::mutex> lock(m_mut);

  ost << "id: " << m_id << "  proxy: " << m_proxy << std::endl;

  ost << "clock: [" << m_condSet_clock.size() << "]" << std::endl;
  //  std::set<IOVEntryT<T>, IOVEntryT<T>::IOVEntryTStartCritereon >::const_iterator itr = m_condSet_clock.begin();
  typename CondContSet::const_iterator itr = m_condSet_clock.begin();
  for (; itr != m_condSet_clock.end(); ++itr) {
    ost << *itr << std::endl;
  }
  ost << "RE: [" << m_condSet_RE.size() << "]" << std::endl;
  itr = m_condSet_RE.begin();
  for (; itr != m_condSet_RE.end(); ++itr) {
    ost << *itr << std::endl;
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
int CondCont<T>::entries() const {
  std::lock_guard<std::mutex> lock(m_mut);
  return m_condSet_RE.size() + m_condSet_clock.size();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
std::vector<EventIDRange> 
CondCont<T>::ranges() const {
  std::lock_guard<std::mutex> lock(m_mut);

  std::vector<EventIDRange> r;
  for (auto ent : m_condSet_RE) {
    r.push_back( ent.range() );
  }
  for (auto ent : m_condSet_clock) {
    r.push_back( ent.range() );
  }

  return r;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool
CondCont<T>::range(const EventIDBase& now, EventIDRange& r) const {
  std::lock_guard<std::mutex> lock(m_mut);

  typename CondContSet::const_iterator itr;
  if (now.isRunEvent()) {
    itr = m_condSet_RE.begin();
    for (; itr != m_condSet_RE.end(); ++itr) {
      if ( itr->range().isInRange( now ) ) {
        r = itr->range();
        return true;
      }
    }
  } else if (now.isTimeStamp()) {
    itr = m_condSet_clock.begin();
    for (; itr != m_condSet_clock.end(); ++itr) {
      if ( itr->range().isInRange( now ) ) {
        r = itr->range();
        return true;
      }
    }
  }
  
  return false;
}


// #include "SGTools/CLASS_DEF.h"
// CLASS_DEF( CondContBase , 34480459 , 1 )
// #include "SGTools/BaseInfo.h"
// // SG_BASE( CondCont<Cont>, CondContBase )

#endif

