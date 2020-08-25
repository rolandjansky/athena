/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EventContainers/InternalOfflineMap.h"
#include <algorithm>
#include "EventContainers/IDC_WriteHandleBase.h"
#include "CxxUtils/AthUnlikelyMacros.h"

using namespace EventContainers;
typedef I_InternalIDC::InternalConstItr InternalConstItr;
InternalOfflineMap::InternalOfflineMap(size_t max) : m_needsupdate(false), m_maxsize(max) {}


bool InternalOfflineMap::tryAddFromCache(IdentifierHash hash, EventContainers::IDC_WriteHandleBase&) {
    return m_fullMap.count(hash);
}

bool InternalOfflineMap::tryAddFromCache(IdentifierHash hash)
{
    return m_fullMap.count(hash);
}

void InternalOfflineMap::wait() const {
   std::scoped_lock lock (m_waitMutex);
   if(m_needsupdate == false) return;
   m_map.clear();
   m_map.reserve(m_fullMap.size());
   for(const auto &pair : m_fullMap){
     m_map.emplace_back(pair.first, pair.second);
   }
   std::sort(m_map.begin(), m_map.end());
   m_needsupdate.store(false);
}

std::vector<IdentifierHash> InternalOfflineMap::getAllCurrentHashes() const {
    std::vector<IdentifierHash> ids;
    ids.reserve(m_fullMap.size());
    if(m_needsupdate == true){
       for(const auto &pair : m_fullMap){
         ids.emplace_back(pair.first);
       }
       std::sort(ids.begin(), ids.end());
    }else{
       for(const auto &pair : m_map){
          ids.emplace_back(pair.first);
       }
    }
    return ids;
}

InternalConstItr
 InternalOfflineMap::cend() const {
    if(m_needsupdate) wait();
    return m_map.cend();
}

const std::vector < I_InternalIDC::hashPair >& InternalOfflineMap::getAllHashPtrPair() const{
    if(m_needsupdate) wait();
    return m_map;
}

InternalConstItr
 InternalOfflineMap::cbegin() const {
    if(m_needsupdate) wait();
    return m_map.cbegin();
}

InternalConstItr InternalOfflineMap::indexFind( IdentifierHash hashId ) const{
   if(m_needsupdate) wait();
   auto itr = std::lower_bound( m_map.cbegin(), m_map.cend(), hashId.value(), [](const hashPair &lhs,  IdentifierHash::value_type rhs) -> bool { return lhs.first < rhs; } );
   if(itr!= m_map.cend() && itr->first==hashId) return itr;
   return m_map.cend();
}

size_t InternalOfflineMap::numberOfCollections() const {
    return m_fullMap.size();
}

void InternalOfflineMap::cleanUp(deleter_f* deleter) noexcept {
    destructor(deleter);
    m_map.clear();
    m_fullMap.clear();
    m_needsupdate.store(false, std::memory_order_relaxed);
}

bool InternalOfflineMap::insert(IdentifierHash hashId, const void* ptr) {
    auto &mapptr = m_fullMap[hashId];
    if(mapptr!=nullptr) return false; //already present
    mapptr = ptr;
    m_needsupdate.store(true, std::memory_order_relaxed);
    return true;
}

const void* InternalOfflineMap::findIndexPtr(IdentifierHash hashId) const noexcept{
    auto search = m_fullMap.find(hashId);
    if(search!=m_fullMap.cend()) return search->second;
    return nullptr;
}

StatusCode InternalOfflineMap::addLock(IdentifierHash hashId, const void* ptr) {
    bool added = insert(hashId, ptr);
    if(ATH_UNLIKELY(!added)) {
      throw std::runtime_error("IDC WARNING Deletion shouldn't occur in addLock paradigm");
    }
    return StatusCode::SUCCESS;
}

void* InternalOfflineMap::removeCollection( IdentifierHash hashId ) {
   auto search = m_fullMap.find(hashId);
   if(search==m_fullMap.cend()) return nullptr;
   void* ptr = const_cast< void* > (search->second);
   m_fullMap.erase(search);
   m_needsupdate.store(true, std::memory_order_relaxed);
   return ptr;
}

StatusCode InternalOfflineMap::fetchOrCreate(IdentifierHash) {
    throw std::runtime_error("Not implemented in offline mode");
}
StatusCode InternalOfflineMap::fetchOrCreate(const std::vector<IdentifierHash>&)
{
    throw std::runtime_error("Not implemented in offline mode");
}

void InternalOfflineMap::destructor(deleter_f* deleter) noexcept {
    if(!m_needsupdate) for(const auto& x : m_map)  deleter(x.second);
    else {
      for(const auto &pair : m_fullMap) {  deleter(pair.second);  }
    }
}
