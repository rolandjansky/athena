/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_METACONT_H
#define IOVSVC_METACONT_H 1

#include <iostream>
#include <set>
#include <vector>
#include <typeinfo>
#include <mutex>
#include <typeinfo>

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/DataObjID.h"

//#include "PersistentDataModel/Guid.h"

namespace SG {
  class DataProxy;
  template<class T>
  class ReadMetaHandle;
  template<class T>
  class WriteMetaHandle;
}

class MetaContBase {
//typedef Guid SourceID;
public:
  typedef std::string SourceID;
  MetaContBase() {};
  virtual ~MetaContBase(){};

  //  virtual void list() const { std::cout << "CCBase" << std::endl; }
  virtual void list(std::ostringstream&) const = 0;
  virtual int entries() const { return 0; }
  virtual bool valid( const SourceID& t) const = 0;
  virtual const DataObjID& id() const = 0;
  virtual bool insert(const SourceID& sid, DataObject* obj) = 0;
  virtual bool insert(const SourceID& sid, void* obj) = 0;

  virtual SG::DataProxy* proxy() const = 0;
  virtual void setProxy(SG::DataProxy*) = 0;

  virtual std::vector<SourceID> sources() const = 0;

private:
};

///////////////////////////////////////////////////////////////////////////

template <typename T>
class MetaCont: public MetaContBase {
public:

  friend SG::ReadMetaHandle<T>;
  friend SG::WriteMetaHandle<T>;

  MetaCont(const DataObjID& id, SG::DataProxy* prx=nullptr):
    m_id(id), m_proxy(prx) {};
  ~MetaCont();

  bool insert(const SourceID& r, DataObject* t);
  bool insert(const SourceID& r, void* t);
  bool insert(const SourceID& r, T* t);

  bool find(const SourceID& t, T*&) const;

  virtual void list(std::ostringstream&) const;
  virtual int entries() const;

  virtual bool valid( const SourceID& t ) const;

  virtual const DataObjID& id() const { return m_id; }

  virtual SG::DataProxy* proxy() const { return m_proxy; }
  virtual void setProxy(SG::DataProxy* prx) { m_proxy = prx; }

  virtual std::vector<SourceID> sources() const;

private:

  //bool findEntry(const EventIDBase& t, const IOVEntryT<T>*&) const;

  mutable std::mutex m_mut;

  //typedef std::set<IOVEntryT<T>, typename IOVEntryT<T>::IOVEntryTStartCritereon > MetaContSet;
  typedef std::map<SourceID,T*> MetaContSet;
  MetaContSet m_metaSet;

  DataObjID m_id;

  SG::DataProxy* m_proxy;

};

//
///////////////////////////////////////////////////////////////////////////
//

template <typename T>
MetaCont<T>::~MetaCont<T>() {
  //  std::cout << "cleaning up - " << typeid( T ).name() << std::endl;
  
  for (auto itr=m_metaSet.begin(); itr != m_metaSet.end(); ++itr) {
    //delete itr->objPtr();
    delete itr->second;
  }
  m_metaSet.clear();
}    

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::insert(const SourceID& r, void* obj) {
  T* t = static_cast<T*>(obj);

  return insert(r, t);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::insert(const SourceID& r, DataObject* obj) {
  T* t = dynamic_cast<T*>( obj );

  if (t == 0) {
    std::cerr << "MetaCont<>T unable to dcast from DataObject to " 
              << typeid(T).name()
              << std::endl;
    return false;
  }

  return insert(r, t);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::insert(const SourceID& r, T* t) {

  std::lock_guard<std::mutex> lock(m_mut);

  return m_metaSet.insert(std::make_pair(r,t)).second;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::valid(const SourceID& it) const {
  typename MetaContSet::const_iterator itr;
  
  std::lock_guard<std::mutex> lock(m_mut);

  return m_metaSet.find(it)!=m_metaSet.end();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::find(const SourceID& it, T*& t) const {
  typename MetaContSet::const_iterator itr;
  
  std::lock_guard<std::mutex> lock(m_mut);

  //std::cerr << "MetaCont find SID " << it << std::endl;

  itr = m_metaSet.find(it);
  if (itr != m_metaSet.end()) {
    t=itr->second;
    return true;
  }
/*
  else {
    t=0;
    typename MetaContSet::const_iterator it=m_metaSet.begin();
    for (; it != m_metaSet.end(); ++it) {
      std::cerr << "Container has SID=" << it->first << std::endl;
    }
  }
*/
  return false;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
template <typename T>
bool MetaCont<T>::findEntry(const EventIDBase& it, const IOVEntryT<T>*& t) const {
  typename MetaContSet::const_iterator itr;
  
  std::lock_guard<std::mutex> lock(m_mut);

  // if EventID is BOTH, we need to look in both sets
  if (it.isRunEvent()) {
    itr = m_condSet_RE.begin();
    for (; itr != m_condSet_RE.end(); ++itr) {
      if ( itr->range().isInRange( it ) ) {
        t = &(*itr);
        return true;
      }
    }
  }
  if (it.isTimeStamp()) {
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
*/
    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
void MetaCont<T>::list(std::ostringstream& ost) const {
  std::lock_guard<std::mutex> lock(m_mut);

  ost << "id: " << m_id << "  proxy: " << m_proxy << std::endl;

  ost << "Meta: [" << m_metaSet.size() << "]" << std::endl;
  //auto itr = m_metaSet.begin();
  //for (; itr != m_metaSet.end(); ++itr) {
  //  ost << *(itr->second) << std::endl;
  //}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
int MetaCont<T>::entries() const {
  std::lock_guard<std::mutex> lock(m_mut);
  return m_metaSet.size();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
std::vector<MetaContBase::SourceID> 
MetaCont<T>::sources() const {
  std::lock_guard<std::mutex> lock(m_mut);

  std::vector<MetaContBase::SourceID> r;
  for (auto ent : m_metaSet) {
    r.push_back(ent.first);
  }

  return r;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( MetaContBase , 34480469 , 1 )
//#include "SGTools/BaseInfo.h"
//SG_BASE( MetaCont<Cont>, MetaContBase )

#endif

