/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Cabling/SCT_CablingData.h"

// STL includes 
#include <algorithm>

//Utility functions at file scope
namespace {
  //make a number even
  IdentifierHash even(const IdentifierHash& hash) {
    return (hash>>1) << 1;
  }
}

const IdentifierHash SCT_CablingData::s_invalidHash{};
const SCT_OnlineId SCT_CablingData::s_invalidId{};
const SCT_SerialNumber SCT_CablingData::s_invalidSn{};

SCT_CablingData::SCT_CablingData() {
  invalidate();
}

void SCT_CablingData::invalidate() {
  invalidateHashesForOnlineIds();
  invalidateOnlineIdsForHashes();
  invalidateHashesForSerialNumbers();
  invalidateSerialNumbersForHashes();
  invalidateRods();
}

void SCT_CablingData::invalidateHashesForOnlineIds() {
  m_onlineId2HashArray.fill(s_invalidHash);
}

bool SCT_CablingData::setHashForOnlineId(const IdentifierHash& hash, const SCT_OnlineId& onlineId) {
  //compute the index for insertion to the onlineId/Hash array
  const unsigned int indx{onlineId.index()};
  if (m_onlineId2HashArray[indx].is_valid()) return false; //don't insert if theres already a valid hash there
  m_onlineId2HashArray[indx] = hash;
  return true;
}

IdentifierHash SCT_CablingData::getHashFromOnlineId(const SCT_OnlineId& onlineId) const {
  const unsigned int indx{onlineId.index()};
  if (indx==SCT_OnlineId::INVALID_INDEX) {
    return s_invalidHash;
  }
  return m_onlineId2HashArray[indx];
}

void SCT_CablingData::invalidateOnlineIdsForHashes() {
  m_hash2OnlineIdArray.fill(s_invalidId);
}

bool SCT_CablingData::setOnlineIdForHash(const SCT_OnlineId& onlineId, const IdentifierHash& hash) {
  if (m_hash2OnlineIdArray[hash].is_valid()) return false; //don't insert if there's already a valid online id there
  //insert to hash/onlineId array
  m_hash2OnlineIdArray[hash] = onlineId;
  return true;
}

SCT_OnlineId SCT_CablingData::getOnlineIdFromHash(const IdentifierHash& hash) const {
  return hash.is_valid()? m_hash2OnlineIdArray[hash] : s_invalidId;
}

void SCT_CablingData::invalidateHashesForSerialNumbers() {
  m_sn2HashMap.clear();
}

bool SCT_CablingData::setHashForSerialNumber(const IdentifierHash& hash, const SCT_SerialNumber& sn) {
  return m_sn2HashMap.insert(std::make_pair(sn, hash)).second;
}

IdentifierHash SCT_CablingData::getHashFromSerialNumber(const SCT_SerialNumber& sn) const {
  std::map<SCT_SerialNumber, IdentifierHash>::const_iterator it{m_sn2HashMap.find(sn)};
  if (it==m_sn2HashMap.end()) return s_invalidHash;
  return it->second;
}

void SCT_CablingData::invalidateSerialNumbersForHashes() {
  m_hash2SnArray.fill(s_invalidSn);
}

bool SCT_CablingData::setSerialNumberForHash(const SCT_SerialNumber& sn, const IdentifierHash& hash) {
  const IdentifierHash evenHash{even(hash)};
  if (m_hash2SnArray[evenHash]!=s_invalidSn) return false; //don't insert if there's already a valid online id there
  m_hash2SnArray[evenHash] = sn;
  return true;
}

SCT_SerialNumber SCT_CablingData::getSerialNumberFromHash(const IdentifierHash& hash) const {
  return hash.is_valid() ? m_hash2SnArray[even(hash)] : s_invalidSn;
}

void SCT_CablingData::invalidateRods() {
  m_rodIdSet.clear();
}

void SCT_CablingData::setRod(const std::uint32_t& rod) {
  m_rodIdSet.insert(rod);
}

void SCT_CablingData::getRods(std::vector<std::uint32_t>& usersVector) const {
  std::copy(m_rodIdSet.begin(), m_rodIdSet.end(), std::back_inserter(usersVector));
}

unsigned int SCT_CablingData::getHashEntries() const {
  return m_sn2HashMap.size();
}
