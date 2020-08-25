/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EventContainers/InternalOnline.h"
#include <algorithm>
#include "EventContainers/IDC_WriteHandleBase.h"
#include "CxxUtils/AthUnlikelyMacros.h"
#include "EventContainers/IdentifiableCacheBase.h"
#include "CxxUtils/AthUnlikelyMacros.h"

using namespace EventContainers;
typedef I_InternalIDC::InternalConstItr InternalConstItr;


InternalOnline::InternalOnline(EventContainers::IdentifiableCacheBase *cache) : m_cacheLink(cache),
    m_mask(cache->fullSize(), false), m_waitNeeded(false) {}

const std::vector < I_InternalIDC::hashPair >& InternalOnline::getAllHashPtrPair() const{
  if(m_waitNeeded) wait();
  return m_map; 
}


InternalConstItr
 InternalOnline::cend() const {
    if(m_waitNeeded) wait();
    return m_map.cend();
}

InternalConstItr
 InternalOnline::cbegin() const {
    if(m_waitNeeded) wait();
    return m_map.cbegin();
}

InternalConstItr InternalOnline::indexFind( IdentifierHash hashId ) const{
  if(m_waitNeeded) wait();
   auto itr = std::lower_bound( m_map.begin(), m_map.end(), hashId.value(), [](hashPair &lhs,  IdentifierHash::value_type rhs) -> bool { return lhs.first < rhs; } );
   if(itr!= m_map.end() && itr->first==hashId) return itr;
   return m_map.end();
}

void InternalOnline::wait() const {
    //lockguard to protect m_waitlist from multiple wait calls
    std::scoped_lock lock (m_waitMutex);
    if(m_waitNeeded == false) return;
    using namespace EventContainers;
    const void* ABORTstate = reinterpret_cast<const void*>(IdentifiableCacheBase::ABORTEDflag);
    while(!m_waitlist.empty()) {
        IdentifierHash hash = m_waitlist.back();
        const void* ptr = m_cacheLink->waitFor(hash);
        if(ptr == ABORTstate) {
          m_mask[hash] = false;
        }
        m_waitlist.pop_back();
    }
    m_map.clear();
    for(size_t i =0;i<m_mask.size();i++){
        if(m_mask[i]) m_map.emplace_back(i, m_cacheLink->m_vec[i].load());
    }
    m_waitNeeded.store(false);
}

bool InternalOnline::tryAddFromCache(IdentifierHash hashId, EventContainers::IDC_WriteHandleBase &lock) {
    int flag = m_cacheLink->tryLock(hashId, lock, m_waitlist);
    //Relaxed since this should not be running in threaded situation.
    if(!m_waitlist.empty()) m_waitNeeded.store(true, std::memory_order_relaxed);
    if(flag > 0) {
        if(flag!=3){
           m_mask[hashId] = true;
           m_waitNeeded.store(true, std::memory_order_relaxed);
        }
        return true;
    }
    return false;
}

bool InternalOnline::tryAddFromCache(IdentifierHash hashId)
{
    auto ptr = m_cacheLink->find(hashId);
    if(ptr==nullptr) {
        return false;
    }
    m_mask[hashId] = true;
    m_waitNeeded.store(true, std::memory_order_relaxed);
    return true;
}

std::vector<IdentifierHash> InternalOnline::getAllCurrentHashes() const {
    if(m_waitNeeded) wait();
    std::vector<IdentifierHash> ids;
    ids.reserve(m_map.size());
    for(auto &x : m_map) {
        ids.emplace_back(x.first);
    }
    return ids;
}

size_t InternalOnline::numberOfCollections() const {
    if(m_waitNeeded) wait();
    return m_map.size();
}

void InternalOnline::resetMask() {
    if(m_waitNeeded) wait();
    m_mask.assign(m_cacheLink->fullSize(), false);
    m_map.clear();
    m_waitNeeded.store(true, std::memory_order_relaxed);
}

StatusCode InternalOnline::fetchOrCreate(IdentifierHash hashId) {
    if(ATH_UNLIKELY(!m_cacheLink->IMakerPresent())) return StatusCode::FAILURE;
    auto ptr = m_cacheLink->get(hashId);
    if(ptr) { m_mask[hashId] =true; m_waitNeeded.store(true, std::memory_order_relaxed); }
    return StatusCode::SUCCESS;
}

StatusCode InternalOnline::fetchOrCreate(const std::vector<IdentifierHash> &/*hashIds*/) {
    throw std::runtime_error("Not implemented");
//    if(ATH_UNLIKELY(!m_cacheLink->IMakerPresent())) return StatusCode::FAILURE;
//    m_cacheLink->createSet(hashIds, m_mask);
//    return StatusCode::SUCCESS;
}


bool InternalOnline::insert(IdentifierHash hashId, const void* ptr) {
    std::pair<bool, const void*> cacheinserted = m_cacheLink->add(hashId, ptr);
    m_mask[hashId] = true; //it wasn't added it is already present therefore mask could be true
    m_waitNeeded.store(true, std::memory_order_relaxed);
    return ptr == cacheinserted.second;
}

const void* InternalOnline::findIndexPtr(IdentifierHash hashId) const noexcept {
    if(hashId < m_mask.size() and m_mask[hashId]) return m_cacheLink->findWait(hashId);
    return nullptr;
}

StatusCode InternalOnline::addLock(IdentifierHash hashId, const void* ptr) {
    std::pair<bool, const void*> added = m_cacheLink->addLock(hashId, ptr);
    if(ATH_UNLIKELY(!added.first)) {
      throw std::runtime_error("IDC WARNING Deletion shouldn't occur in addLock paradigm");
    }
    m_mask[hashId] = true; //it wasn't added it is already present therefore mask could be true
    m_waitNeeded.store(true, std::memory_order_relaxed);
    return StatusCode::SUCCESS;
}

void* InternalOnline::removeCollection( IdentifierHash  ) {
    throw std::runtime_error("Do not remove things from an online IDC");
}

void InternalOnline::destructor(deleter_f*) noexcept {
    //deliberately empty
}

void InternalOnline::cleanUp(deleter_f*) noexcept {
    resetMask();
}
