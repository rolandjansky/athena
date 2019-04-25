/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "IovStore.h"

namespace IOVDbNamespace{

  IovStore::IovStore(){ 

  }
  ///Constructor, cache bounds set to supplied parameter
  IovStore::IovStore(const IovStore::Iov_t & cacheBounds):m_cacheBounds(cacheBounds){
    //
  }
  ///Add an iov to the store and update the span variables
  void 
  IovStore::addIov(const IovStore::Iov_t & iov){ 
    m_iovs.push_back(iov);
    const auto straddles = straddling(iov, m_cacheBounds);
    m_countStraddling.first += straddles.first;
    m_countStraddling.second += straddles.second;
    if (straddles.first) m_maxStraddlingSince = std::max(m_maxStraddlingSince, iov.first); 
    if (straddles.second) m_minStraddlingUntil = std::min(m_minStraddlingUntil, iov.second);
  }
  void
  IovStore::addIov(const cool::ValidityKey & since, const cool::ValidityKey & until){
    addIov(IovStore::Iov_t(since, until));
  }
  
  ///Report the current cache bounds
  IovStore::Iov_t 
  IovStore::getCacheBounds() const{ 
    return m_cacheBounds;
  }
  ///Set the cache bound variables
  void 
  IovStore::setCacheBounds(const IovStore::Iov_t & cacheBounds){ 
    if (cacheBounds.second > cool::ValidityKeyMax){
      m_cacheBounds=IovStore::Iov_t(cacheBounds.first, cool::ValidityKeyMax);
    } else {
      m_cacheBounds = cacheBounds;
    }
  }
  ///Get the minimum straddling span (max 'since' - min 'until')
  IovStore::Iov_t 
  IovStore::getMinimumStraddlingSpan() const{
    return IovStore::Iov_t(m_maxStraddlingSince, m_minStraddlingUntil);
  }
  
  ///Return the current count of added Iovs which straddled cache boundaries
  std::pair<unsigned int, unsigned int> 
  IovStore::numberOfIovsOnBoundaries() const{
    return m_countStraddling;
  }

  ///clear the store
  void IovStore::clear()
  {
    m_iovs.clear();
    m_countStraddling = std::make_pair(0,0);
  }
  
  ///Return the internal vector store
  std::vector<IovStore::Iov_t> &
  IovStore::vectorStore() { 
    return m_iovs;
  }
  
  std::pair<bool,bool> 
  IovStore::straddling(const Iov_t & iov, const Iov_t & bounds) const{
    std::pair<bool, bool> straddles(false, false);
    straddles.first = (iov.first<=bounds.first) and (iov.second>bounds.first);
    straddles.second = (iov.first<bounds.second) and (iov.second>bounds.second);
    return straddles;
  }
  
  void 
  IovStore::extendIov(const unsigned int idx, const cool::ValidityKey & newUntilTime){
    m_iovs[idx].second = newUntilTime;
  }
  
  void 
  IovStore::setIovSpan(const Iov_t & span){
    m_maxStraddlingSince=span.first;
    m_minStraddlingUntil=span.second;
  }
  
  ///Extend lower cache bound  to the maximum 'since' time; return whether this changes its value
  bool 
  IovStore::extendCacheLo(){
    const bool extend = m_maxStraddlingSince < m_cacheBounds.first;
    if (extend) m_cacheBounds.first = m_maxStraddlingSince;
    return extend;
  }
  ///Extend upper cache bound to the minimum 'until' time; return whether this changes its value
  bool 
  IovStore::extendCacheHi(){
    const bool extend = m_minStraddlingUntil > m_cacheBounds.second;
    if (extend) m_cacheBounds.second = m_minStraddlingUntil;
    return extend;
  }
  
  
  



}
