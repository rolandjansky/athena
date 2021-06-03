/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_IDENTIFIABLEVALUECACHE_H
#define EVENTCONTAINERS_IDENTIFIABLEVALUECACHE_H

#include <atomic>
#include <vector>
#include <cstdlib>

/**
*  This class is to provide an event wide MT container for concurrent storing of basic types, like ints
*  This is a version of the identifiable container optimized for basic types
*  The cache is designed for event level storage and concurrent writing.
*/

template<class T>
class IdentifiableValueCache{

public:

   typedef std::true_type thread_safe;

   ///Return the empty value that is interpreted as an empty entry
   const T& emptyValue() const {
      return m_emptyValue;
   }

   //Prevent accidental copying
   IdentifiableValueCache(const IdentifiableValueCache&) = delete;

   ///Pass the maximum hash to size the cache and the defaultValue which will be interpreted as an empty value
   IdentifiableValueCache(size_t maxSize, T emptyValuein)
      : m_vec(maxSize), m_emptyValue(std::move(emptyValuein))
   {
        for(auto &x : m_vec) x.store(emptyValue(), std::memory_order_relaxed);
   }

   ///Forceable empty the container, DO NOT USE THIS IN MT ENVIRONMENT
   void forceReset();

   ///Return the maxSize of the collection
   size_t maxSize() const { return m_vec.size(); }

   ~IdentifiableValueCache() = default;

   ///Retrieve the Value stored in that hash
   T retrieve(size_t i){   return m_vec.at(i).load(); }

   /// As above, but no cache extension
   T retrieve(size_t i) const {  return i < maxSize() ? static_cast<T>(m_vec[i]) : emptyValue() ; }

   ///Returns true if the value is set to anything but the emptyValue
   bool present(size_t i){  return m_vec.at(i).load() != emptyValue();   }

   /// As above, but no cache extension
   bool present(size_t i) const {  return  i < maxSize() and m_vec[i] != emptyValue() ? true : false; }

   ///Set the given hash to the value
   bool setOrDrop(size_t i, const T &value);

   ///Make a vector of hashes and values,  convenient for iteration and other uses.
   std::vector<std::pair<size_t, T>> getAll() const;

   const std::vector<std::atomic<T>>& rawReadAccess() const { return m_vec; }

private:
   std::vector<std::atomic<T>> m_vec;
   const T m_emptyValue;
};

template<class T>
std::vector<std::pair<size_t, T>>
IdentifiableValueCache<T>::getAll() const{
   std::vector<std::pair<size_t, T>> list;
   for(size_t i =0; i<m_vec.size(); i++){
       T item = m_vec[i].load();
       if(item!=m_emptyValue) list.emplace_back(i, std::move(item));
   }
   return list;
}

template<class T>
void IdentifiableValueCache<T>::forceReset(){
    for(auto &x : m_vec) x.store(emptyValue(), std::memory_order_relaxed);
}

template<class T>
bool IdentifiableValueCache<T>::setOrDrop(size_t i, const T &value){
    T val = emptyValue();
    return m_vec.at(i).compare_exchange_strong(val, value);
}

#endif
