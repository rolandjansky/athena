/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_IDENTIFIABLECVALUEONTAINER_H
#define EVENTCONTAINERS_IDENTIFIABLEVALUECONTAINER_H

#include "EventContainers/IdentifiableValueCache.h"

template<class T, T EMPTYVALUE>
class IdentifiableValueContainer{

public:

   static constexpr T emptyValue() {
      return EMPTYVALUE;
   }

   IdentifiableValueContainer(const IdentifiableValueContainer<T,EMPTYVALUE>&) = delete;

   ~IdentifiableValueContainer()  { if(m_own) delete m_cache; }
   IdentifiableValueContainer(size_t maxSize) : m_mask(maxSize, false), m_own(true)
   {
       m_cache = new IdentifiableValueCache<T, EMPTYVALUE>(maxSize);
   }

   IdentifiableValueContainer(IdentifiableValueCache<T,EMPTYVALUE> *ptr) : m_mask(ptr->maxSize()),
   m_cache(ptr), m_own(false)
   {}

   bool present(size_t i) const
   {
       return m_mask.at(i);
   }

   bool setOrDrop(size_t i, const T &value){
      bool b = m_cache->setOrDrop(i, value);
      m_mask[i] = true;
      return b;
   }

   size_t maxSize() const { return m_mask.size(); }

   size_t numberSet() const{
        size_t count = 0;
        for(bool b : m_mask) count += b;
        return count;
   }

   bool tryAddFromCache(size_t i){
      if(i >= m_mask.size()) return false;
      bool b = m_cache->present(i);
      m_mask[i] = b;
      return b;
   }

   T retrieve(size_t i) const{
        if(m_mask[i]) return m_cache->retrieve(i);
        else return emptyValue();
   }

   std::vector<std::pair<size_t, T>> getAll() const{
      std::vector<std::pair<size_t, T>> list;
      const auto& raw = m_cache->rawReadAccess();
      for(size_t i =0; i<m_mask.size(); i++){
          if(m_mask[i]) list.emplace_back(i, raw[i].load());
      }
      return list;
   }

   const std::vector<std::atomic<T>>& wholeEventReadAccess() const { return m_cache->rawReadAccess(); }

private:
   std::vector<bool> m_mask;
   IdentifiableValueCache<T, EMPTYVALUE> *m_cache;
   bool m_own;
};

#endif