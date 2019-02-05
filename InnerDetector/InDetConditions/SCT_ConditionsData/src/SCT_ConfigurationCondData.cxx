/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for the data object class for SCT_ConfigurationSvc
//----------------------------------------------------------------------

#include "SCT_ConditionsData/SCT_ConfigurationCondData.h"

#include <algorithm>
#include <iterator>

//----------------------------------------------------------------------
// Constructor
SCT_ConfigurationCondData::SCT_ConfigurationCondData():
  m_badStripIds{},
  m_badStripArray{},
  m_badWaferIds{},
  m_badModuleIds{},
  m_badChips{}
{
  clearBadLinks();
}

//----------------------------------------------------------------------
// Set a bad strip identifier
void SCT_ConfigurationCondData::setBadStripId(const Identifier& badStripId, const IdentifierHash& hash, const int strip) {
  m_badStripIds.insert(badStripId);
  m_badStripArray[hash].set(strip);
}

//----------------------------------------------------------------------
// Get all bad strip identifiers
const std::set<Identifier>* SCT_ConfigurationCondData::getBadStripIds() const {
  return &m_badStripIds;
}

//----------------------------------------------------------------------
// Clear all bad strip identifiers
void SCT_ConfigurationCondData::clearBadStripIds() {
  m_badStripIds.clear();
  m_badStripArray.fill(std::bitset<N_STRIPS>());
}

//----------------------------------------------------------------------
// Check if a strip identifier is bad one
bool SCT_ConfigurationCondData::isBadStrip(const IdentifierHash& hash, const int strip) const {
  return m_badStripArray[hash][strip];
}

//----------------------------------------------------------------------
// Set a bad wafer identifier
void SCT_ConfigurationCondData::setBadWaferId(const Identifier& badWaferId) {
  m_badWaferIds.insert(badWaferId);
}

//----------------------------------------------------------------------
// Get all bad wafer identifiers
const std::set<Identifier>* SCT_ConfigurationCondData::getBadWaferIds() const {
  return &m_badWaferIds;
}

//----------------------------------------------------------------------
// Clear all bad wafer identifiers
void SCT_ConfigurationCondData::clearBadWaferIds() {
  m_badWaferIds.clear();
}

//----------------------------------------------------------------------
// Check if a wafer identifier is bad one
bool SCT_ConfigurationCondData::isBadWaferId(const Identifier& waferId) const {
  return (m_badWaferIds.find(waferId)!=m_badWaferIds.end());
}

//----------------------------------------------------------------------
// Set a bad module identifier
void SCT_ConfigurationCondData::setBadModuleId(const Identifier& badModuleId) {
  m_badModuleIds.insert(badModuleId);
}

//----------------------------------------------------------------------
// Get all bad module identifiers
const std::set<Identifier>* SCT_ConfigurationCondData::getBadModuleIds() const {
  return &m_badModuleIds;
}

//----------------------------------------------------------------------
// Clear all bad module identifiers
void SCT_ConfigurationCondData::clearBadModuleIds() {
  m_badModuleIds.clear();
}

//----------------------------------------------------------------------
// Check if a module identifier is bad one
bool SCT_ConfigurationCondData::isBadModuleId(const Identifier& moduleId) const {
  return (m_badModuleIds.find(moduleId)!=m_badModuleIds.end());
}

//----------------------------------------------------------------------
// Set bad links for a module
void SCT_ConfigurationCondData::setBadLinks(const IdentifierHash& hash, const bool isBadLink0, const bool isBadLink1) {
  unsigned int iHash{hash};
  iHash = (iHash/2)*2; // Make iHash even
  m_badLinks[iHash].first  &= isBadLink0;
  m_badLinks[iHash].second &= isBadLink1;
  m_badLinksArray[iHash/2].first  &= isBadLink0;
  m_badLinksArray[iHash/2].second &= isBadLink1;
}

//----------------------------------------------------------------------
// Get all bad links
const std::map<IdentifierHash, std::pair<bool, bool>>* SCT_ConfigurationCondData::getBadLinks() const {
  return &m_badLinks;
}

//----------------------------------------------------------------------
// Clear all bad links
void SCT_ConfigurationCondData::clearBadLinks() {
  m_badLinks.clear();
  m_badLinksArray.fill(std::make_pair(true, true));
}

//----------------------------------------------------------------------
// Check if a module has bad links
std::pair<bool, bool> SCT_ConfigurationCondData::areBadLinks(const IdentifierHash& hash) const {
  // Bad convetion is used. true is for good link and false is for bad link...
  return m_badLinksArray[hash/2];
}

//----------------------------------------------------------------------
// Set bad chips for a module
void SCT_ConfigurationCondData::setBadChips(const Identifier& moduleId, const unsigned int chipStatus) {
  if (chipStatus!=0) m_badChips[moduleId] = chipStatus;
}

//----------------------------------------------------------------------
// bad chips for a module
unsigned int SCT_ConfigurationCondData::getBadChips(const Identifier& moduleId) const {
  std::map<Identifier, unsigned int>::const_iterator it{m_badChips.find(moduleId)};
  return (it!=m_badChips.end()) ? (*it).second : 0;
}

//----------------------------------------------------------------------
// Get all bad chips
const std::map<Identifier, unsigned int>* SCT_ConfigurationCondData::getBadChips() const {
  return &m_badChips;
}

//----------------------------------------------------------------------
// Clear all bad chips
void SCT_ConfigurationCondData::clearBadChips() {
  m_badChips.clear();
}

//----------------------------------------------------------------------
// Clear all bad information
void SCT_ConfigurationCondData::clear() {
  clearBadStripIds();
  clearBadWaferIds();
  clearBadModuleIds();
  clearBadLinks();
  clearBadChips();
}
