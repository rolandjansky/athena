/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelCablingCondData.h"
#include "Identifier/Identifier.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>


PixelCablingCondData::PixelCablingCondData():
  m_idMap_onoff(),
  m_idMap_offon(),
  m_idMap_offrob(),
  m_idMap_rodrob(),
  m_idMap_robrod(),
  m_offlineListVect(),
  m_idMapDCSoff(),
  m_rodReadoutMap(),
  m_allRods(),
  m_allRobs()
{ }

PixelCablingCondData::~PixelCablingCondData() { }

void PixelCablingCondData::add_entry_onoff(const uint64_t onlineId, const Identifier offlineId) {
  m_idMap_onoff.insert(std::make_pair(onlineId, offlineId)); // add online identifier -> offline identifier pair in m_idMap_onoff
}

void PixelCablingCondData::add_entry_offon(const Identifier offlineId, const uint64_t onlineId) {
  m_idMap_offon.insert(std::make_pair(offlineId, onlineId)); // add offline identifier -> online identifier pair in m_idMap_offon
}

void PixelCablingCondData::add_entry_offrob(const Identifier offlineId, const uint32_t robid) {
  m_idMap_offrob.insert(std::make_pair(offlineId, robid)); // add offline identifier -> offline ROBId pair in m_idMap_offrob
}

void PixelCablingCondData::add_entry_rodrob(int rodid, int robid) {
  auto [iter, newElementAdded] = m_idMap_rodrob.insert(std::make_pair(rodid, robid)); // add RODId identifier -> offline ROBId pair in m_idMap_rodrob
  if ( newElementAdded )
    m_allRods.push_back(rodid);
}

void PixelCablingCondData::add_entry_robrod(int robid, int rodid) {
  auto [iter, newElementAdded] = m_idMap_robrod.insert(std::make_pair(robid, rodid));
  if ( newElementAdded )
    m_allRobs.push_back(robid);
}

void PixelCablingCondData::add_entry_offlineList(const uint32_t robid, const Identifier offlineId) {
  m_offlineListVect[robid].push_back(offlineId); // add ROBid -> offline identifier pair in m_offlineListVect
}

void PixelCablingCondData::add_entry_DCSoffline(const std::string& DCSname, const Identifier offlineId) {
  m_idMapDCSoff.insert(std::make_pair(DCSname, offlineId));
}

void PixelCablingCondData::set_readout_map(std::map<uint32_t, bool> rodReadoutMap) {
  m_rodReadoutMap = std::move(rodReadoutMap);
}

Identifier PixelCablingCondData::find_entry_onoff(const uint64_t onlineId) const {
  // Search m_idMap_onoff for the onlineId
  uint64_t searchId = onlineId;
  Identifier offlineId(0);
  std::unordered_map<uint64_t, Identifier>::const_iterator iter(m_idMap_onoff.find(searchId));

  // OnlineId has been found - get the offline identifier
  if (iter != m_idMap_onoff.end()) offlineId = (*iter).second;

  // Is onlineId not found? Then check if this is a b-layer module; if it is read out at
  // 80 Mbit, search again with link = 0, since FMT has enough info to identify the module
  else {
    uint32_t rodid = (onlineId & 0xFFFFFF); // last 24 bit are rodid

    if ((rodid & 0x130000) >> 16 == 0x13) {
      std::map<uint32_t, bool>::const_iterator it = m_rodReadoutMap.find(rodid);

      if (it != m_rodReadoutMap.end()) {
        if ((*it).second) { // true = is 80 MBit

          // Search again
          searchId = onlineId & (0xF0FFFFFF);
          iter = m_idMap_onoff.find(searchId);

          // Get identifier
          if (iter != m_idMap_onoff.end()) offlineId = (*iter).second;
        }
      }
    }
    // If onlineId is still not found, the empty identifier is returned.
  }
  return offlineId;
}

uint64_t PixelCablingCondData::find_entry_offon(const Identifier offlineId) const {
  uint64_t onlineId; // declare online identifier
  std::unordered_map<Identifier, uint64_t, idHasher>::const_iterator iter(m_idMap_offon.find(offlineId)); // find offline identifier in m_idMap_offon map
  if (iter == m_idMap_offon.end()) { // if offline identifier not found in m_idMap_offon map -> ERROR
    onlineId = 0; // fill online identifier with empty identifier
    return onlineId; // return empty online identifier
  }
  onlineId = (*iter).second; // fill online identifier with found online identifier
  return onlineId; // return found online identifier
}

uint32_t PixelCablingCondData::find_entry_offrob(Identifier offlineId) const {
  uint32_t robid; // declare ROBId
  std::unordered_map<Identifier, uint32_t, idHasher>::const_iterator iter(m_idMap_offrob.find(offlineId)); // find offline identifier in m_idMap_offrob map
  if (iter == m_idMap_offrob.end()) { // if offline identifier not found in m_idMap_offrob map -> ERROR
    robid = 0; // fill ROBId with empty identifier
    return robid; // return empty ROBId
  }
  robid = (*iter).second; // fill ROBId with found ROBId
  return robid; // return found ROBId
}

int PixelCablingCondData::find_entry_rodrob(const int rodid) const {
  std::unordered_map<int, int>::const_iterator iter(m_idMap_rodrob.find(rodid));
  if (iter == m_idMap_rodrob.end()) {
    return 0;
  }
  int robid = iter->second;
  return robid;
}

int PixelCablingCondData::find_entry_robrod(const int robid) const {
  std::unordered_map<int,int>::const_iterator iter(m_idMap_robrod.find(robid));
  if (iter == m_idMap_robrod.end()) return 0;
  return iter->second;
}

std::deque<Identifier> PixelCablingCondData::find_entry_offlineList(uint32_t robid) const {
  std::deque<Identifier> offlineId; // declare collection identifier list for the ROBId
  std::deque<Identifier>::const_iterator it1 = m_offlineListVect.at(robid).begin(); // first offline identifier for the ROBId
  std::deque<Identifier>::const_iterator it2 = m_offlineListVect.at(robid).end(); // last offline identifier for the ROBId
  for (; it1 != it2; ++it1) { // loop thrue offline identifiers
    offlineId.push_back(*it1); // fill offline identifier into offline identifier list
  }
  return offlineId; // return the offline identifier list for the ROBId
}

Identifier PixelCablingCondData::find_entry_DCSoffline(const std::string& DCSname) const {
  std::unordered_map<std::string, Identifier>::const_iterator iter(m_idMapDCSoff.find(DCSname));
  if (iter == m_idMapDCSoff.end()) {
    return Identifier(0);
  }
  return iter->second;
}

std::string PixelCablingCondData::find_entry_offlineDCS(const Identifier offlineId) const {
  std::unordered_map<std::string, Identifier>::const_iterator iter = m_idMapDCSoff.begin();
  for (; iter != m_idMapDCSoff.end(); iter++) {
    if (iter->second == offlineId)
      return iter->first;
  }
  return ""; // not found
}

const std::vector<uint32_t>& PixelCablingCondData::get_allRods() const {
  return m_allRods;
}

const std::vector<uint32_t>& PixelCablingCondData::get_allRobs() const {
  return m_allRobs;
}

////////////////////////
// getOnlineIdFromRobId - returns the onlineId from the robId and the link number
// onlineId convention: onlineId is constructed as 0xAABBDDMMMM, where MMMM = robId,
// DD = subdetector id, AABB = link number.
// For pixels, there is only one link number per onlineId, so AA = 00.
////////////////////////
uint64_t PixelCablingCondData::getOnlineIdFromRobId(uint32_t robid, uint32_t link) const {
  uint32_t subDetId = (robid & 0xFFFFFF) >> 16;

  // Pixels:
  if (subDetId < 0x14) return (robid & 0xFFFFFF) | (link << 24);

  // IBL / DBM:
  else if (subDetId == 0x14 || subDetId == 0x15) {

    // Need to search the on-off map for the appropriate onlineId
    uint32_t linknum_temp;
    std::unordered_map<uint64_t, Identifier>::const_iterator itr = m_idMap_onoff.begin();
    for (; itr != m_idMap_onoff.end(); ++itr) {
      if ((itr->first & 0xFFFFFF) == robid) {
        linknum_temp = (itr->first >> 24);
        // Check for linknum in both first and third nibble
        if ((linknum_temp & 0xF) == link || ((linknum_temp >> 8) & 0xF) == link) return itr->first;
      }
    }
  }
  return 0;
}

const std::unordered_map<uint64_t, Identifier>& PixelCablingCondData::get_idMap_onoff() const {
  return m_idMap_onoff;
}

const std::unordered_map<Identifier, uint64_t, idHasher>& PixelCablingCondData::get_idMap_offon() const {
  return m_idMap_offon;
}

const std::unordered_map<Identifier, uint32_t, idHasher>& PixelCablingCondData::get_idMap_offrob() const {
  return m_idMap_offrob;
}

const std::unordered_map<int,int>& PixelCablingCondData::get_idMap_rodrob() const {
  return m_idMap_rodrob;
}

const std::unordered_map<int,int>& PixelCablingCondData::get_idMap_robrod() const {
  return m_idMap_robrod;
}

const std::unordered_map<std::string, Identifier>& PixelCablingCondData::get_idMapDCSoff() const {
  return m_idMapDCSoff;
}

void PixelCablingCondData::clear() {
  m_idMap_onoff.clear();
  m_idMap_offon.clear();
  m_idMap_offrob.clear();
  m_idMap_rodrob.clear();
  m_idMap_robrod.clear();
  m_offlineListVect.clear();
  m_idMapDCSoff.clear();
  m_rodReadoutMap.clear();
  m_allRods.clear();
  m_allRobs.clear();
}

