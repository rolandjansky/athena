/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ConditionsData/SCT_ModuleVetoCondData.h"

//////////////////////////////////
// constructor
SCT_ModuleVetoCondData::SCT_ModuleVetoCondData():
  m_badWaferId{},
  m_isFilled{false}
{
}

//////////////////////////////////
// destructor
SCT_ModuleVetoCondData::~SCT_ModuleVetoCondData() {
}

//////////////////////////////////
// set a bad wafer ID
bool SCT_ModuleVetoCondData::setBadWaferId(const Identifier waferId) {
  return m_badWaferId.insert(waferId).second;
}

//////////////////////////////////
// check if a wafer ID is bad or not
bool SCT_ModuleVetoCondData::isBadWaferId(const Identifier waferId) const {
  std::set<Identifier>::const_iterator it{m_badWaferId.find(waferId)};
  return (it!=m_badWaferId.end());
}

//////////////////////////////////
// clear
void SCT_ModuleVetoCondData::clear() {
  m_badWaferId.clear();
  m_isFilled = false;
}

//////////////////////////////////
// set data filled
void SCT_ModuleVetoCondData::setFilled() {
  m_isFilled = true;
}

//////////////////////////////////
// check the data are filled or not
bool SCT_ModuleVetoCondData::filled() const {
  return m_isFilled;
}

//////////////////////////////////
// get the number of bad wafers
long unsigned int SCT_ModuleVetoCondData::size() const {
  return m_badWaferId.size();
}
