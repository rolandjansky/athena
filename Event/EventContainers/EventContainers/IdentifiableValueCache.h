/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_IDENTIFIABLEVALUECACHE_H
#define EVENTCONTAINERS_IDENTIFIABLEVALUECACHE_H

#include <atomic>
#include <vector>



template<class T, T EMPTYVALUE>
class IdentifiableValueCache{

public:
  static constexpr T emptyValue() {
      return EMPTYVALUE;
   }

   IdentifiableValueCache(const IdentifiableValueCache&) = delete;
   IdentifiableValueCache(size_t maxSize)
      : m_vec(maxSize)
   {
        for(auto &x : m_vec) x.store(emptyValue(), std::memory_order_relaxed);
   }

   void forceReset(){
        for(auto &x : m_vec) x.store(emptyValue(), std::memory_order_relaxed);
   }

   size_t maxSize() const { return m_vec.size(); }

   ~IdentifiableValueCache() = default;

   T retrieve(size_t i){
        return m_vec.at(i).load();
   }

   bool present(size_t i){
        return m_vec.at(i).load() != emptyValue();
   }

   bool setOrDrop(size_t i, const T &value){
       T val = emptyValue();
       return m_vec.at(i).compare_exchange_strong(val, value);
   }

   const std::vector<std::atomic<T>>& rawReadAccess() const { return m_vec; }

private:
   std::vector<std::atomic<T>> m_vec;

};

#endif
