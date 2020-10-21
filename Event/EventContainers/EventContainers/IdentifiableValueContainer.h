/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_IDENTIFIABLEVALUECONTAINER_H
#define EVENTCONTAINERS_IDENTIFIABLEVALUECONTAINER_H

#include "EventContainers/IdentifiableValueCache.h"
#include <set>

class IdentifiableValueContainerBase {};


/*
*  This class is the view specific container that can link to the IdentifiableValueCache
*  It allows you to link to an external cache and keep a mask to track the items in your specific view
*/

template<class T>
class IdentifiableValueContainer : IdentifiableValueContainerBase{

public:
  typedef T value_type;
  typedef IdentifiableValueCache<T> Cache;

   //Prevent accidental copying
   IdentifiableValueContainer(const IdentifiableValueContainer<T>&) = delete;
   IdentifiableValueContainer& operator=(const IdentifiableValueContainer&) = delete;

   ~IdentifiableValueContainer()  { if(m_own) delete m_cache; }

   ///Self Owning Constructor
   ///Pass the maximum hash to size the cache and the defaultValue which will be interpreted as an empty value
   IdentifiableValueContainer(size_t maxSize, T defaultValue) : m_own(true)
   {
       m_cache = new IdentifiableValueCache<T>(maxSize, std::move(defaultValue));
   }

   ///External Cache Constructor
   ///Pass the external cache to set up a view specific view interface
   IdentifiableValueContainer(IdentifiableValueCache<T> *ptr) : 
   m_cache(ptr), m_own(false)
   {}


   ///Return the empty value that is interpreted as an empty entry
   const T& emptyValue() const { return m_cache->emptyValue(); }


   ///Is the value for this has set and also accepted in the mask
   bool present(size_t i) const;

   ///Set the value for the given hash
   bool setOrDrop(size_t i, const T &value);

   ///Return the maxSize of the collection
   size_t maxSize() const { return m_cache->maxSize(); }

   ///Return the number of entries set and accessible according to the mask.
   ///This is not a trivial function do not repeatedly call.
   size_t numberSet() const;

   ///Returns true if the value is also in the external cache, sets mask to true if it is
   bool tryAddFromCache(size_t i);

   ///Retrieve the value of the hash, if accessible according to the mask
   T retrieve(size_t i) const;

   ///Make a vector of hashes and values,  convenient for iteration and other uses.
   std::vector<std::pair<size_t, T>> getAll() const;

   ///Get read only access to the whole external cache. This could be useful for special situations
   const std::vector<std::atomic<T>>& wholeEventReadAccess() const { return m_cache->rawReadAccess(); }

   /// Obtain const access to the cache
   const Cache* cache() const { return m_cache; }

   const std::set<size_t>& getMask() const { return m_mask; }
private:
   std::set<size_t> m_mask;
   Cache *m_cache;
   bool m_own;
};

template< class T >
bool IdentifiableValueContainer<T>::present(size_t i) const
{
   return m_mask.count(i);
}

template< class T >
std::vector<std::pair<size_t, T>> IdentifiableValueContainer<T>::getAll() const{
   std::vector<std::pair<size_t, T>> list;
   list.reserve(m_mask.size());
   const auto& raw = m_cache->rawReadAccess();
   for(size_t i : m_mask){
       list.emplace_back(i, raw[i].load());
   }
   return list;
}

template< class T >
T IdentifiableValueContainer<T>::retrieve(size_t i) const{
     auto r = m_cache->retrieve(i);
     //Should be quicker to establish empty cache than empty mask with a std::set
     //So the cache is checked first
     if(r!= m_cache->emptyValue() && present(i)) return r;
     else return m_cache->emptyValue();
}

template< class T >
bool IdentifiableValueContainer<T>::tryAddFromCache(size_t i){
   if(i >= maxSize()) return false;
   bool b = m_cache->present(i);
   if(b) m_mask.emplace(i);
   return b;
}



template< class T >
size_t IdentifiableValueContainer<T>::numberSet() const{
     return m_mask.size();
}

template< class T >
bool IdentifiableValueContainer<T>::setOrDrop(size_t i, const T &value){
   bool b = m_cache->setOrDrop(i, value);
   m_mask.emplace(i);
   return b;
}

#endif
