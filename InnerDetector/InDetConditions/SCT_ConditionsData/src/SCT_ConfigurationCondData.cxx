/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  m_badWaferIds{},
  m_badModuleIds{},
  m_badLinks{},
  m_badChips{}
{}

//----------------------------------------------------------------------
// Destructor
SCT_ConfigurationCondData::~SCT_ConfigurationCondData()
{}

//----------------------------------------------------------------------
// Set a bad strip identifier
void SCT_ConfigurationCondData::setBadStripId(const Identifier& badStripId) {
  m_badStripIds.insert(badStripId);
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
}

//----------------------------------------------------------------------
// Check if a strip identifier is bad one
bool SCT_ConfigurationCondData::isBadStripId(const Identifier& stripId) const {
  return (m_badStripIds.find(stripId)!=m_badStripIds.end());
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
void SCT_ConfigurationCondData::setBadLinks(const Identifier& moduleId, const bool isBadLink0, const bool isBadLink1) {
  m_badLinks[moduleId].first  |= isBadLink0;
  m_badLinks[moduleId].second |= isBadLink1;
}

//----------------------------------------------------------------------
// Get all bad links
const std::map<Identifier, std::pair<bool, bool>>* SCT_ConfigurationCondData::getBadLinks() const {
  return &m_badLinks;
}

//----------------------------------------------------------------------
// Clear all bad links
void SCT_ConfigurationCondData::clearBadLinks() {
  m_badLinks.clear();
}

//----------------------------------------------------------------------
// Check if a module has bad links
std::pair<bool, bool> SCT_ConfigurationCondData::areBadLinks(const Identifier& moduleId) const {
  // Bad convetion is used. true is for good link and false is for bad link...
  std::map<Identifier, std::pair<bool, bool>>::const_iterator it{m_badLinks.find(moduleId)};
  return (it!=m_badLinks.end()) ? (*it).second : std::make_pair(true, true);
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
