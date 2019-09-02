/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "EventContainers/IdentifiableContainerBase.h"
#include <algorithm>
#include "EventContainers/IDC_WriteHandleBase.h"
#include "CxxUtils/AthUnlikelyMacros.h"

  IdentifiableContainerBase::IdentifiableContainerBase(EventContainers::IdentifiableCacheBase *cache, bool online)
  {
    m_cacheLink = cache;
    m_OnlineMode = online;
    m_mask.assign(cache->fullSize(), !online);
    m_waitNeeded.store(false, std::memory_order_relaxed);
  }

  IdentifiableContainerBase::Hash_Container::const_iterator IdentifiableContainerBase::findHash(IdentifiableContainerBase::Hash_Container* container, IdentifierHash hash){
    return std::find(container->begin(), container->end(), hash);
  }

  IdentifiableContainerBase::~IdentifiableContainerBase(){
#ifndef NDEBUG
    //Deregister unused interest
    while(!m_waitlist.empty()){
       IdentifierHash hash = m_waitlist.back();
       m_cacheLink->cancelWait(hash);
       m_waitlist.pop_back();
    }
#endif
  }

  void IdentifiableContainerBase::Wait() const{
    //lockguard to protect m_waitlist from multiple wait calls
    typedef std::lock_guard<decltype(m_waitMutex)> lockguard;
    lockguard lock(m_waitMutex);

    const void* ABORTstate = reinterpret_cast<const void*>(IdentifiableCacheBase::ABORTEDflag);
    while(!m_waitlist.empty()){
       IdentifierHash hash = m_waitlist.back();
       const void* ptr = m_cacheLink->waitFor(hash);
#ifndef NDEBUG
       m_cacheLink->cancelWait(hash);
#endif
       if(ptr == ABORTstate) m_mask[hash] = false;//reset flag
       m_waitlist.pop_back();
       
    }
    m_waitNeeded.store(false);
  }

  bool IdentifiableContainerBase::tryAddFromCache(IdentifierHash hashId, EventContainers::IDC_WriteHandleBase &lock)
  {
    if(!m_OnlineMode){
       return tryAddFromCache(hashId);//No point calling expensive lock method
    }
    int flag = m_cacheLink->tryLock(hashId, lock, m_waitlist);
    //Relaxed since this should not be running in threaded situation.
    if(!m_waitlist.empty()) m_waitNeeded.store(true, std::memory_order_relaxed);
    if(flag > 0) {
      m_mask[hashId] = flag!=3;
      return true;
    }
    return false;
  }

  bool IdentifiableContainerBase::tryAddFromCache(IdentifierHash hashId)
  {
    auto ptr = m_cacheLink->find(hashId);
    if(ptr==nullptr){
        return false; 
    }
    m_mask[hashId] = true;
    return true; 
  }

  std::vector<IdentifierHash> IdentifiableContainerBase::GetAllCurrentHashes() const {
     if(not m_OnlineMode) return m_cacheLink->ids();
     else{
         if(m_waitNeeded) Wait();
         std::vector<IdentifierHash> ids;
         for(size_t i =0 ; i < m_mask.size(); ++i) if(m_mask[i]) ids.emplace_back(i);
         return ids;
     }
  }

  size_t IdentifiableContainerBase::numberOfCollections() const{
     if(!m_OnlineMode) return m_cacheLink->numberOfHashes();
     if(m_waitNeeded) Wait(); 
     size_t count =std::accumulate(m_mask.begin(), m_mask.end(), 0);
     return count;
  }


  void IdentifiableContainerBase::ResetMask(){
     m_mask.assign(m_cacheLink->fullSize(), false);
  }

  StatusCode IdentifiableContainerBase::fetchOrCreate(IdentifierHash hashId){
    if(ATH_UNLIKELY(!m_cacheLink->IMakerPresent())) return StatusCode::FAILURE;
    auto ptr = m_cacheLink->get(hashId);
    m_mask[hashId] = ptr !=nullptr;
    return StatusCode::SUCCESS;
  }

  StatusCode IdentifiableContainerBase::fetchOrCreate(const std::vector<IdentifierHash> &hashIds){
    if(ATH_UNLIKELY(!m_cacheLink->IMakerPresent())) return StatusCode::FAILURE;
    m_cacheLink->createSet(hashIds, m_mask);
    return StatusCode::SUCCESS;
  }

