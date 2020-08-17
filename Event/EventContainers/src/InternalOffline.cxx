/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EventContainers/InternalOffline.h"
#include <algorithm>
#include "EventContainers/IDC_WriteHandleBase.h"
#include "CxxUtils/AthUnlikelyMacros.h"

using namespace EventContainers;
typedef I_InternalIDC::InternalConstItr InternalConstItr;
InternalOffline::InternalOffline(size_t max) : m_maximumSize(max) {
  //Check optimization assumptions
  static_assert(std::is_trivially_copyable<hashPair>::value);
  static_assert(std::is_trivially_destructible<hashPair>::value);
  static_assert(std::is_trivially_copyable<IdentifierHash>::value);
  static_assert(std::is_trivially_destructible<IdentifierHash>::value);
}


bool InternalOffline::tryAddFromCache(IdentifierHash hash, EventContainers::IDC_WriteHandleBase&) {
    return indexFind(hash) != m_map.cend();//No Cache in offline mode
}

bool InternalOffline::tryAddFromCache(IdentifierHash hash)
{
    return indexFind(hash) != m_map.cend();//No Cache in offline mode
}

void InternalOffline::wait() const {
    //No need to wait in offline, deliberately empty method
}

std::vector<IdentifierHash> InternalOffline::getAllCurrentHashes() const {
    std::vector<IdentifierHash> ids;
    ids.reserve(m_map.size());
    for(auto &x : m_map) {
        ids.emplace_back(x.first);
    }
    return ids;
}

InternalConstItr
 InternalOffline::cend() const {
    return m_map.cend();
}

const std::vector < I_InternalIDC::hashPair >& InternalOffline::getAllHashPtrPair() const{ return m_map; }

InternalConstItr
 InternalOffline::cbegin() const {
    return m_map.cbegin();
}

InternalConstItr InternalOffline::indexFind( IdentifierHash hashId ) const{
   auto itr = std::lower_bound( m_map.cbegin(), m_map.cend(), hashId.value(), [](const hashPair &lhs,  IdentifierHash::value_type rhs) -> bool { return lhs.first < rhs; } );
   if(itr!= m_map.cend() && itr->first==hashId) return itr;
   return m_map.cend();
//  return std::find_if(m_map.begin(), m_map.end(), [hashId](const hashPair &lhs) -> bool { return lhs.first == hashId; });
}

size_t InternalOffline::numberOfCollections() const {
    return m_map.size();
}

void InternalOffline::cleanUp(deleter_f* deleter) noexcept {
    for(const auto& x : m_map)  deleter(x.second);
    m_map.clear();
}

bool InternalOffline::insert(IdentifierHash hashId, const void* ptr) {
    if(m_map.empty() || m_map.back().first < hashId){
        m_map.emplace_back(hashId, ptr);
        return true;
    }
    auto itr = std::lower_bound( m_map.begin(), m_map.end(), hashId.value(), [](const hashPair &lhs,  IdentifierHash::value_type rhs) -> bool { return lhs.first < rhs; } );
    if(itr == std::end(m_map) || itr->first != hashId)
    {
        m_map.emplace(itr, hashId, ptr);
        return true;
    }
    return false;
}

const void* InternalOffline::findIndexPtr(IdentifierHash hashId) const noexcept{
    auto itr = indexFind(hashId);
    if(itr != m_map.end()) return itr->second;
    return nullptr;
}

StatusCode InternalOffline::addLock(IdentifierHash hashId, const void* ptr) {
    bool added = insert(hashId, ptr);
    if(ATH_UNLIKELY(!added)) {
      throw std::runtime_error("IDC WARNING Deletion shouldn't occur in addLock paradigm");
    }
    return StatusCode::SUCCESS;
}

void* InternalOffline::removeCollection( IdentifierHash hashId ) {
   auto itr = std::lower_bound( m_map.begin(), m_map.end(), hashId.value(), [](hashPair &lhs,  IdentifierHash::value_type rhs) -> bool { return lhs.first < rhs; } );
   if(itr== m_map.end() || itr->first!=hashId) return nullptr;
   void* ptr = const_cast< void* > (itr->second);
   m_map.erase(itr);
   return ptr;
}

StatusCode InternalOffline::fetchOrCreate(IdentifierHash) {
    throw std::runtime_error("Not implemented in offline mode");
}
StatusCode InternalOffline::fetchOrCreate(const std::vector<IdentifierHash>&)
{
    throw std::runtime_error("Not implemented in offline mode");
}

void InternalOffline::destructor(deleter_f* deleter) noexcept {
    for(const auto& x : m_map)  deleter(x.second);
}
