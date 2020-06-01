/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EventContainers/InternalOfflineFast.h"
#include <algorithm>
#include "EventContainers/IDC_WriteHandleBase.h"

using namespace EventContainers;
typedef I_InternalIDC::InternalConstItr InternalConstItr;
InternalOfflineFast::InternalOfflineFast(size_t max) : m_fullMap(max, nullptr) {}


bool InternalOfflineFast::tryAddFromCache(IdentifierHash hash, EventContainers::IDC_WriteHandleBase&) {
    return m_fullMap[hash] != nullptr;
}

bool InternalOfflineFast::tryAddFromCache(IdentifierHash hash)
{
    return m_fullMap[hash] != nullptr;
}

void InternalOfflineFast::wait() const {

}

std::vector<IdentifierHash> InternalOfflineFast::getAllCurrentHashes() const {
    std::vector<IdentifierHash> ids;
    ids.reserve(m_map.size());
    for(auto &x : m_map) {
        ids.emplace_back(x.first);
    }
    return ids;
}

InternalConstItr
 InternalOfflineFast::cend() const {
    return m_map.cend();
}

const std::vector < I_InternalIDC::hashPair >& InternalOfflineFast::getAllHashPtrPair() const{ return m_map; }

InternalConstItr
 InternalOfflineFast::cbegin() const {
    return m_map.cbegin();
}

InternalConstItr InternalOfflineFast::indexFind( IdentifierHash hashId ) const{
   auto itr = std::lower_bound( m_map.cbegin(), m_map.cend(), hashId.value(), [](const hashPair &lhs,  IdentifierHash::value_type rhs) -> bool { return lhs.first < rhs; } );
   if(itr!= m_map.cend() && itr->first==hashId) return itr;
   return m_map.cend();
//  return std::find_if(m_map.begin(), m_map.end(), [hashId](const hashPair &lhs) -> bool { return lhs.first == hashId; });
}

size_t InternalOfflineFast::numberOfCollections() const {
    return m_map.size();
}

void InternalOfflineFast::cleanUp(deleter_f* deleter) noexcept {
    for(const auto& x : m_map) { deleter(x.second); m_fullMap[x.first] = nullptr; }
    m_map.clear();
}

bool InternalOfflineFast::insert(IdentifierHash hashId, const void* ptr) {
    if(hashId >= m_fullMap.size()) return false;
    if(m_fullMap[hashId]!= nullptr) return false; //already in
    if(m_map.empty() || m_map.back().first < hashId){
        m_map.emplace_back(hashId, ptr);
        m_fullMap[hashId] = ptr;
        return true;
    }
    auto itr = std::lower_bound( m_map.begin(), m_map.end(), hashId.value(), [](const hashPair &lhs,  IdentifierHash::value_type rhs) -> bool { return lhs.first < rhs; } );
    if(itr == std::end(m_map) || itr->first != hashId)
    {
        m_map.emplace(itr, hashId, ptr);
        m_fullMap[hashId] = ptr;
        return true;
    }
    return false;
}

const void* InternalOfflineFast::findIndexPtr(IdentifierHash hashId) const noexcept{
    if(hashId >= m_fullMap.size()) return nullptr;
    return m_fullMap[hashId];
}

StatusCode InternalOfflineFast::addLock(IdentifierHash hashId, const void* ptr) {
    bool added = insert(hashId, ptr);
    if(!added) {
#ifndef NDEBUG
        std::cout << "IDC WARNING Deletion shouldn't occur in addLock paradigm" << std::endl;
#endif
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

void* InternalOfflineFast::removeCollection( IdentifierHash hashId ) {
   auto itr = std::lower_bound( m_map.begin(), m_map.end(), hashId.value(), [](hashPair &lhs,  IdentifierHash::value_type rhs) -> bool { return lhs.first < rhs; } );
   if(itr== m_map.end() || itr->first!=hashId) return nullptr;
   void* ptr = const_cast< void* > (itr->second);
   m_map.erase(itr);
   m_fullMap[hashId] = nullptr;
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
    for(const auto& x : m_map)  deleter(x.second);
}
