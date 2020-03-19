/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_METACONT_H
#define ATHENAKERNEL_METACONT_H

#include <sstream>
#include <map>
#include <vector>
#include <typeinfo>
#include <mutex>

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/SourceID.h"
#include "AthenaKernel/DataBucketTraitFwd.h"
#include "AthenaKernel/MetaContDataBucket.h"

namespace SG {
  template<class T> class ReadMetaHandle;
}

class MetaContBase {
 public:
  typedef SG::SourceID SourceID;
  MetaContBase() {};
  virtual ~MetaContBase(){};

  virtual bool insert(const SourceID& sid, void* obj) = 0;

  virtual int entries() const { return 0; }
  virtual bool valid(const SourceID& sid) const = 0;

  virtual std::vector<SourceID> sources() const = 0;

  virtual void list(std::ostringstream& stream) const = 0;

 private:
};

///////////////////////////////////////////////////////////////////////////

template <typename T>
class MetaCont: public MetaContBase {
 public:
  typedef T Payload_t;
  friend SG::ReadMetaHandle<T>;

  MetaCont() {};
  ~MetaCont();

  // Virtual functions
  virtual bool insert(const SourceID& sid, void* obj) override;

  virtual int entries() const override;
  virtual bool valid(const SourceID& sid) const override;

  virtual std::vector<SourceID> sources() const override;

  virtual void list(std::ostringstream& stream) const override;

  // Non-virtual functions
  bool insert(const SourceID& sid, T* t);
  bool find(const SourceID& sid, T*& t) const;
  const T* get(const SourceID& sid) const;

 private:

  mutable std::mutex m_mut;

  typedef std::map<SourceID,T*> MetaContSet;
  MetaContSet m_metaSet;
};


namespace SG {


  /**
   * @brief Metafunction to find the proper @c DataBucket class for @c T.
   *
   * Specialize this for @c MetaCont.
   * See AthenaKernel/StorableConversions.h for an explanation.
   */
  template <class T, class U>
    struct DataBucketTrait<MetaCont<T>, U>
    {
      typedef MetaContDataBucket<U> type;
      static void init() {}
    };

} // namespace SG

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
MetaCont<T>::~MetaCont<T>() {
  for (auto t : m_metaSet) {
    delete t.second;
  }
  m_metaSet.clear();
}    

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::insert(const SourceID& sid, void* obj) {
  T* t = static_cast<T*>(obj);
  return insert(sid, t);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::insert(const SourceID& sid, T* t) {
  std::lock_guard<std::mutex> lock(m_mut);
  return m_metaSet.insert(std::make_pair(sid,t)).second;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::valid(const SourceID& sid) const {
  std::lock_guard<std::mutex> lock(m_mut);
  return m_metaSet.find(sid)!=m_metaSet.end();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
bool MetaCont<T>::find(const SourceID& sid, T*& t) const {
  std::lock_guard<std::mutex> lock(m_mut);

  typename MetaContSet::const_iterator itr = m_metaSet.find(sid);
  if (itr != m_metaSet.end()) {
    t=itr->second;
    return true;
  }

  return false;
}

    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
const T* MetaCont<T>::get(const SourceID& sid) const {
  std::lock_guard<std::mutex> lock(m_mut);

  typename MetaContSet::const_iterator itr = m_metaSet.find(sid);
  if (itr != m_metaSet.end()) {
    return itr->second;
  }

  return nullptr;
}

    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <typename T>
void MetaCont<T>::list(std::ostringstream& stream) const {
  std::lock_guard<std::mutex> lock(m_mut);
  // To Do: perhaps extend this output?
  stream << "MetaCont with size : [" << m_metaSet.size() << "]" << std::endl;
  for(const auto& mapel : m_metaSet) {
    stream << "... Key : " << mapel.first << std::endl;
  }
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

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( MetaContBase , 34480469 , 1 )


/// Declare a metadata container along with its CLID:
//    METACONT_DEF(TYPE, CLID);
//
#define METACONT_DEF(T, CLID) \
  CLASS_DEF( MetaCont<T>, CLID, 1 )             \
  SG_BASE( MetaCont<T>, MetaContBase )

#endif

