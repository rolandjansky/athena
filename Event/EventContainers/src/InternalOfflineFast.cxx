/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EventContainers/InternalOfflineFast.h"
#include <algorithm>
#include "EventContainers/IDC_WriteHandleBase.h"
#include "CxxUtils/AthUnlikelyMacros.h"

using namespace EventContainers;
typedef I_InternalIDC::InternalConstItr InternalConstItr;
InternalOfflineFast::InternalOfflineFast(size_t max) : m_fullMap(max, nullptr), m_needsupdate(false) {}


bool InternalOfflineFast::tryAddFromCache(IdentifierHash hash, EventContainers::IDC_WriteHandleBase&) {
    return m_fullMap[hash] != nullptr;
}

bool InternalOfflineFast::tryAddFromCache(IdentifierHash hash)
{
    return m_fullMap[hash] != nullptr;
}

void InternalOfflineFast::wait() const {
   std::scoped_lock lock (m_waitMutex);
   if(m_needsupdate == false) return;
   m_map.clear();
   for(size_t i=0 ;i < m_fullMap.size(); ++i){
    if(m_fullMap[i]) m_map.emplace_back(i, m_fullMap[i]);
   }
   m_map.shrink_to_fit();
   m_needsupdate.store(false);
}

std::vector<IdentifierHash> InternalOfflineFast::getAllCurrentHashes() const {
    if(m_needsupdate) wait();
    std::vector<IdentifierHash> ids;
    ids.reserve(m_map.size());
    for(auto &x : m_map) {
        ids.emplace_back(x.first);
    }
    return ids;
}

InternalConstItr
 InternalOfflineFast::cend() const {
    if(m_needsupdate) wait();
    return m_map.cend();
}

const std::vector < I_InternalIDC::hashPair >& InternalOfflineFast::getAllHashPtrPair() const{
    if(m_needsupdate) wait();
    return m_map;
}

InternalConstItr
 InternalOfflineFast::cbegin() const {
    if(m_needsupdate) wait();
    return m_map.cbegin();
}

InternalConstItr InternalOfflineFast::indexFind( IdentifierHash hashId ) const{
   if(m_needsupdate) wait();
   auto itr = std::lower_bound( m_map.cbegin(), m_map.cend(), hashId.value(), [](const hashPair &lhs,  IdentifierHash::value_type rhs) -> bool { return lhs.first < rhs; } );
   if(itr!= m_map.cend() && itr->first==hashId) return itr;
   return m_map.cend();
}

size_t InternalOfflineFast::numberOfCollections() const {
    if(m_needsupdate) wait();
    return m_map.size();
}

void InternalOfflineFast::cleanUp(deleter_f* deleter) noexcept {
    if(!m_needsupdate) {
        for(const auto& x : m_map) { deleter(x.second); m_fullMap[x.first] = nullptr; }
        if(!m_map.empty()) m_needsupdate.store(true, std::memory_order_relaxed);
    }
    else {
      for(size_t i=0 ;i < m_fullMap.size(); ++i){
         if(m_fullMap[i]) { deleter(m_fullMap[i]); m_fullMap[i] = nullptr; }
      }
    }
    m_map.clear();
}

bool InternalOfflineFast::insert(IdentifierHash hashId, const void* ptr) {
    if(m_fullMap[hashId]!= nullptr) return false; //already in
    m_fullMap[hashId] = ptr;
    m_needsupdate.store(true, std::memory_order_relaxed);
    return true;
}

const void* InternalOfflineFast::findIndexPtr(IdentifierHash hashId) const noexcept{
    if(hashId >= m_fullMap.size()) return nullptr;
    return m_fullMap[hashId];
}

StatusCode InternalOfflineFast::addLock(IdentifierHash hashId, const void* ptr) {
    bool added = insert(hashId, ptr);
    if(!added) {
      throw std::runtime_error("IDC WARNING Deletion shouldn't occur in addLock paradigm");
    }
    return StatusCode::SUCCESS;
}

void* InternalOfflineFast::removeCollection( IdentifierHash hashId ) {
   void* ptr ATLAS_THREAD_SAFE = const_cast< void* > (m_fullMap[hashId]);
   m_fullMap[hashId] = nullptr;
   m_needsupdate.store(true, std::memory_order_relaxed);
   return ptr;
}

StatusCode InternalOfflineFast::fetchOrCreate(IdentifierHash) {
    throw std::runtime_error("Not implemented in offline mode");
}
StatusCode InternalOfflineFast::fetchOrCreate(const std::vector<IdentifierHash>&)
{
    throw std::runtime_error("Not implemented in offline mode");
}

void InternalOfflineFast::destructor(deleter_f* deleter) noexcept {
    if(!m_needsupdate) for(const auto& x : m_map)  deleter(x.second);
    else {
      for(size_t i=0 ;i < m_fullMap.size(); ++i){
         if(m_fullMap[i]) deleter(m_fullMap[i]);
      }
    }
}
