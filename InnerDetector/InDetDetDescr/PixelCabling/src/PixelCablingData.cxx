/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCablingData.cxx
//   Implementation file for class PixelCablingData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// Class used to add and search entries in the maps:
// - add_entry_onoff() - add entries to m_idMap_onoff
// - add_entry_offon() - add entries in m_idMap_offon
// - add_entry_offrob() - add entries in m_idMap_offrob
// - add_entry_rodrob() - add entries in map m_idMap_rodrob
// - find_entry_onoff() - search entry in the map m_idMap_onoff - get the offlineId from the onlineId
// - find_entry_offon() - search entry in the map m_idMap_offon - get the onlineId from the offlineId
// - find_entry_offrob() - search entry in the map m_idMap_offrob - get the ROBId from the offlineId
// - find_entry_rodrob() - search entry in the map m_idMap_rodrob - get the ROBId from the RODId
// and to the list vector:
// - add_entry_offlineList() - add entries in m_offlineListVect
// - find_entry_offlineList() - return entries from m_offlineListVect for a ROBId
// - get_allRobs - returns the list of all ROBs in the mapping
// - getOnlineIdFromRobId - returns the onlineId from the robId and the link_module
///////////////////////////////////////////////////////////////////
//  Version 00-03-26 15/01/2007 Daniel Dobos
///////////////////////////////////////////////////////////////////

#include "PixelCablingData.h"
#include "Identifier/Identifier.h"
#include <iostream>
#include <fstream>
#include <sstream>


// Hashing function for Identifiers
std::size_t hash_value(Identifier const& id) {
  //boost::hash<int> hasher;
  boost::hash<Identifier::value_type> hasher;
    return hasher(id.get_compact());
}


////////////////////////
// constructor
////////////////////////
PixelCablingData::PixelCablingData() :
    m_commonHitDiscCnfg(3)
{
    m_allRods = new std::vector<uint32_t>;
    m_allRobs = new std::vector<uint32_t>;

    //m_commonHitDiscCnfg = 3;
}


////////////////////////
// destructor
////////////////////////
PixelCablingData::~PixelCablingData() {
    delete m_allRods;
    delete m_allRobs;
}

////////////////////////
// add_entry_onoff - add entries to m_idMap_onoff
////////////////////////
void PixelCablingData::add_entry_onoff(const uint64_t onlineId, const Identifier offlineId) {
    m_idMap_onoff.insert(std::make_pair(onlineId, offlineId)); // add online identifier -> offline identifier pair in m_idMap_onoff
}

////////////////////////
// add_entry_offon - add entries in m_idMap_offon
////////////////////////
void PixelCablingData::add_entry_offon(const Identifier offlineId, const uint64_t onlineId) {
    m_idMap_offon.insert(std::make_pair(offlineId, onlineId)); // add offline identifier -> online identifier pair in m_idMap_offon
}

////////////////////////
// add_entry_offrob - add entries in m_idMap_offrob
////////////////////////
void PixelCablingData::add_entry_offrob(const Identifier offlineId, const uint32_t robid) {
    m_idMap_offrob.insert(std::make_pair(offlineId, robid)); // add offline identifier -> offline ROBId pair in m_idMap_offrob
}

////////////////////////
// add_entry_rodrob - add entries in map m_idMap_rodrob
////////////////////////
void PixelCablingData::add_entry_rodrob(int rodid, int robid) {
    m_idMap_rodrob.insert(std::make_pair(rodid, robid)); // add RODId identifier -> offline ROBId pair in m_idMap_rodrob
}

////////////////////////
// add_entry_robrod - add entries in map m_idMap_robrod
////////////////////////
void PixelCablingData::add_entry_robrod(int robid, int rodid) {
    m_idMap_robrod.insert(std::make_pair(robid, rodid));
}

////////////////////////
// add_entry_offlineList - add entries in m_offlineListVect
////////////////////////
void PixelCablingData::add_entry_offlineList(const uint32_t robid, const Identifier offlineId) {
    m_offlineListVect[robid].push_back(offlineId); // add ROBid -> offline identifier pair in m_offlineListVect
}

////////////////////////
// add_entry_DCSoffline - add DCS ID offline Identifer mapping
////////////////////////
void PixelCablingData::add_entry_DCSoffline(std::string DCSname, const Identifier offlineId) {
    m_idMapDCSoff.insert(std::make_pair(DCSname, offlineId));
}

////////////////////////
// set_readout_map - set map containing readout speed for each ROD
////////////////////////
void PixelCablingData::set_readout_map(std::map<uint32_t, bool> rodReadoutMap) {
    m_rodReadoutMap = rodReadoutMap;
}

////////////////////////
// find_entry_onoff - search entry in the map m_idMap_onoff - get the offlineId from the onlineId
////////////////////////
Identifier PixelCablingData::find_entry_onoff(const uint64_t onlineId) const {

    // Search m_idMap_onoff for the onlineId
    uint64_t searchId = onlineId;
    Identifier offlineId(0);
    boost::unordered_map<uint64_t, Identifier>::const_iterator iter(m_idMap_onoff.find(searchId));

    // OnlineId has been found - get the offline identifier
    if (iter != m_idMap_onoff.end()) offlineId = (*iter).second;

    // Is onlineId not found? Then check if this is a b-layer module; if it is read out at
    // 80 Mbit, search again with link = 0, since FMT has enough info to identify the module
    else {
        uint32_t rodid = (onlineId & 0xFFFFFF); // last 24 bit are rodid

        if ((rodid & 0x130000) >> 16 == 0x13) {
            std::map<uint32_t, bool>::const_iterator it = m_rodReadoutMap.find(rodid);

            if (it != m_rodReadoutMap.end()) {
                if ((*it).second == true) { // true = is 80 MBit

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

////////////////////////
// find_entry_offon - search entry in the map m_idMap_offon - get the onlineId from the offlineId
////////////////////////
uint64_t PixelCablingData::find_entry_offon(const Identifier offlineId) const {
    uint64_t onlineId; // declare online identifier
    boost::unordered_map<Identifier, uint64_t>::const_iterator iter(m_idMap_offon.find(offlineId)); // find offline identifier in m_idMap_offon map
    if (iter == m_idMap_offon.end()) { // if offline identifier not found in m_idMap_offon map -> ERROR
        onlineId = 0; // fill online identifier with empty identifier
        return onlineId; // return empty online identifier
    }
    onlineId = (*iter).second; // fill online identifier with found online identifier
    return onlineId; // return found online identifier
}

////////////////////////
// find_entry_offrob - search entry in the map m_idMap_offrob - get the ROBId from the offlineId
////////////////////////
uint32_t PixelCablingData::find_entry_offrob(const Identifier offlineId) const {
    uint32_t robid; // declare ROBId
    boost::unordered_map<Identifier, uint32_t>::const_iterator iter(m_idMap_offrob.find(offlineId)); // find offline identifier in m_idMap_offrob map
    if (iter == m_idMap_offrob.end()) { // if offline identifier not found in m_idMap_offrob map -> ERROR
        robid = 0; // fill ROBId with empty identifier
        return robid; // return empty ROBId
    }
    robid = (*iter).second; // fill ROBId with found ROBId
    return robid; // return found ROBId
}

////////////////////////
// find_entry_rodrob - search entry in the map m_idMap_rodrob - get the ROBId from the RODId
////////////////////////
int PixelCablingData::find_entry_rodrob(const int rodid) {
    boost::unordered_map<int, int>::const_iterator iter(m_idMap_rodrob.find(rodid));
    if (iter == m_idMap_rodrob.end()) {
        return 0;
    }
    int robid = iter->second;
    return robid;
}


////////////////////////
// find_entry_robrod - search entry in the map m_idMap_robrod - get the RODId from the ROBId
////////////////////////
int PixelCablingData::find_entry_robrod(const int robid) {
    boost::unordered_map<int,int>::const_iterator iter(m_idMap_robrod.find(robid));
    if (iter == m_idMap_robrod.end()) return 0;
    return iter->second;
}



////////////////////////
// find_entry_offlineList - return entries from m_offlineListVect for a ROBId 
////////////////////////
std::deque<Identifier> PixelCablingData::find_entry_offlineList(uint32_t robid) {
    std::deque<Identifier> offlineId; // declare collection identifier list for the ROBId
    std::deque<Identifier>::const_iterator it1 = m_offlineListVect[robid].begin(); // first offline identifier for the ROBId
    std::deque<Identifier>::const_iterator it2 = m_offlineListVect[robid].end(); // last offline identifier for the ROBId
    for (; it1 != it2; ++it1) { // loop thrue offline identifiers
        offlineId.push_back(*it1); // fill offline identifier into offline identifier list
    }
    return offlineId; // return the offline identifier list for the ROBId
}

////////////////////////
// find_entry_DCSoffline - return offline Id for a DCS name
////////////////////////
Identifier PixelCablingData::find_entry_DCSoffline(std::string DCSname) {
    boost::unordered_map<std::string, Identifier>::const_iterator iter(m_idMapDCSoff.find(DCSname));
    if (iter == m_idMapDCSoff.end()) {
        Identifier offlineId(0);
        return offlineId;
    }
    return iter->second;
}

////////////////////////
// find_entry_DCSoffline - return DCS name for an offline ID
////////////////////////
std::string PixelCablingData::find_entry_offlineDCS(const Identifier offlineId) {
    boost::unordered_map<std::string, Identifier>::const_iterator iter = m_idMapDCSoff.begin();
    for (; iter != m_idMapDCSoff.end(); iter++) {
        if (iter->second == offlineId)
            return iter->first;
    }
    return ""; // not found
}

////////////////////////
// get_allRods - returns the list of all Rods in the Mapping
////////////////////////
std::vector<uint32_t>& PixelCablingData::get_allRods() {
    if (m_allRods->empty()) { // fill vector from map only once
        for (boost::unordered_map<int, int>::iterator rodrobmap_iterator = m_idMap_rodrob.begin(); rodrobmap_iterator != m_idMap_rodrob.end(); rodrobmap_iterator++)
            m_allRods->push_back(rodrobmap_iterator->first);
    }
    return *m_allRods;
}


////////////////////////
// get_allRobs - returns the list of all ROBs in the mapping
////////////////////////
std::vector<uint32_t>& PixelCablingData::get_allRobs() {
    if (m_allRobs->empty()) {
        for (boost::unordered_map<int,int>::const_iterator robrod_iter = m_idMap_robrod.begin(); robrod_iter != m_idMap_robrod.end(); robrod_iter++) {
            m_allRobs->push_back(robrod_iter->first);
        }
    }
    return *m_allRobs;
}


////////////////////////
// getOnlineIdFromRobId - returns the onlineId from the robId and the link number
// onlineId convention: onlineId is constructed as 0xAABBDDMMMM, where MMMM = robId,
// DD = subdetector id, AABB = link number.
// For pixels, there is only one link number per onlineId, so AA = 00.
////////////////////////
uint64_t PixelCablingData::getOnlineIdFromRobId(const uint32_t robid, const uint32_t link) {

    uint32_t subDetId = (robid & 0xFFFFFF) >> 16;

    // Pixels:
    if (subDetId < 0x14) return (robid & 0xFFFFFF) | (link << 24);

    // IBL / DBM:
    else if (subDetId == 0x14 || subDetId == 0x15) {

        // Need to search the on-off map for the appropriate onlineId
        uint32_t linknum_temp;
        boost::unordered_map<uint64_t, Identifier>::const_iterator itr = m_idMap_onoff.begin();
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





////////////////////////
// add_entryHitDiscCngf - insert or modify entry in map
////////////////////////
void PixelCablingData::add_entry_HitDiscCnfg(const uint32_t frontendId, const int hitdisccnfg) {

    // Insert new key-value pair
    if (m_HitDiscCnfgMap.find(frontendId) == m_HitDiscCnfgMap.end()) {
        m_HitDiscCnfgMap.insert(std::make_pair(frontendId, hitdisccnfg));
    }
    // Modify an existing entry
    else {
        m_HitDiscCnfgMap[frontendId] = hitdisccnfg;
    }
}



////////////////////////
// getHitDiscCnfg - get HitDiscCnfg mode for given FE-I4
////////////////////////
int PixelCablingData::getHitDiscCnfg(const uint32_t frontendId) {

    // Search the map for given frontend id
    // If not found, return the common HitDiscCnfg setting.
    boost::unordered_map<uint32_t,int>::const_iterator iter(m_HitDiscCnfgMap.find(frontendId));
    if (iter != m_HitDiscCnfgMap.end()) return iter->second;
    else return m_commonHitDiscCnfg;

}


////////////////////////
// printHitDiscCnfg - print the contents of the  HitDiscCnfg map
////////////////////////
void PixelCablingData::printHitDiscCnfg() {

    std::cout << "Common HitDiscCnfg value: " << m_commonHitDiscCnfg << std::endl;
    std::cout << "Contents of HitDiscCnfg map: " << std::endl;
    boost::unordered_map<uint32_t,int>::const_iterator iter = m_HitDiscCnfgMap.begin();
    for (; iter != m_HitDiscCnfgMap.end(); iter++) {
        std::cout << "  FE ID: 0x" << std::hex << iter->first << "  -  HDC = " << std::dec << iter->second << std::endl;
    }
}



// void PixelCablingData::copy_offonMap (std::map< Identifier, uint64_t> &outputMap){
//   outputMap = m_idMap_offon;
// }


// void  PixelCablingData::copy_DCSoffMap (std::map< std::string, Identifier> &outputMap){
//   outputMap = m_idMapDCSoff;
// }

// void PixelCablingData::copy_offrobMap(std::map< Identifier, uint32_t> &outputMap){
//   outputMap = m_idMap_offrob;
// }


// Give access to the maps (return a std::map copy)
std::map<uint64_t, Identifier> PixelCablingData::get_idMap_onoff() {

    std::map<uint64_t, Identifier> copy;
    for (boost::unordered_map<uint64_t, Identifier>::const_iterator itr = m_idMap_onoff.begin();
         itr != m_idMap_onoff.end(); ++itr) {
        copy.insert(*itr);
    }
    return copy;
}

std::map<Identifier, uint64_t> PixelCablingData::get_idMap_offon(){

    std::map<Identifier, uint64_t> copy;
    for (boost::unordered_map<Identifier, uint64_t>::const_iterator itr = m_idMap_offon.begin();
         itr != m_idMap_offon.end(); ++itr) {
        copy.insert(*itr);
    }
    return copy;
}

std::map<Identifier, uint32_t> PixelCablingData::get_idMap_offrob(){

    std::map<Identifier, uint32_t> copy;
    for (boost::unordered_map<Identifier, uint32_t>::const_iterator itr = m_idMap_offrob.begin();
         itr != m_idMap_offrob.end(); ++itr) {
        copy.insert(*itr);
    }
    return copy;
}

std::map<int,int> PixelCablingData::get_idMap_rodrob(){

    std::map<int,int> copy;
    for (boost::unordered_map<int,int>::const_iterator itr = m_idMap_rodrob.begin();
         itr != m_idMap_rodrob.end(); ++itr) {
        copy.insert(*itr);
    }
    return copy;
}

std::map<int,int> PixelCablingData::get_idMap_robrod(){

    std::map<int,int> copy;
    for (boost::unordered_map<int,int>::const_iterator itr = m_idMap_robrod.begin();
         itr != m_idMap_robrod.end(); ++itr) {
        copy.insert(*itr);
    }
    return copy;
}

std::map<std::string, Identifier> PixelCablingData::get_idMapDCSoff(){

    std::map<std::string, Identifier> copy;
    for (boost::unordered_map<std::string, Identifier>::const_iterator itr = m_idMapDCSoff.begin();
         itr != m_idMapDCSoff.end(); ++itr) {
        copy.insert(*itr);
    }
    return copy;
}


////////////////////////
// clear the maps
////////////////////////
void PixelCablingData::clear_all_maps() {
    m_idMap_onoff.clear();
    m_idMap_offon.clear();
    m_idMap_offrob.clear();
    m_idMap_rodrob.clear();
    m_idMap_robrod.clear();
    m_offlineListVect.clear();
    m_idMapDCSoff.clear();
    m_rodReadoutMap.clear();
    m_HitDiscCnfgMap.clear();
    m_allRods->clear();
    m_allRobs->clear();
}

