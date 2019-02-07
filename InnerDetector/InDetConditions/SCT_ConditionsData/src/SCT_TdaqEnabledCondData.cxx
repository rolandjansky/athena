/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for the data object class for SCT_TdaqEnabledSvc
//----------------------------------------------------------------------

#include "SCT_ConditionsData/SCT_TdaqEnabledCondData.h"

#include <algorithm>
#include <iterator>

//----------------------------------------------------------------------
// Constructor
SCT_TdaqEnabledCondData::SCT_TdaqEnabledCondData():
  m_goodRods{},
  m_goodIds{},
  m_noneBad{false},
  m_filled{false}
{}

//----------------------------------------------------------------------
// Set a good ROD number
bool SCT_TdaqEnabledCondData::setGoodRod(const unsigned int rodNumber)
{
  return m_goodRods.insert(rodNumber).second;
}

//----------------------------------------------------------------------
// Get good ROD numbers
const std::set<unsigned int>& SCT_TdaqEnabledCondData::getGoodRods() const
{
  return m_goodRods;
}

//----------------------------------------------------------------------
// Set a list of good modules
void SCT_TdaqEnabledCondData::setGoodModules(const std::vector<IdentifierHash>& idVec)
{
  std::copy(idVec.begin(), idVec.end(), std::inserter(m_goodIds, m_goodIds.end()));
}

//----------------------------------------------------------------------
// Set filled variable
void SCT_TdaqEnabledCondData::setFilled(const bool filled)
{
  m_filled = filled;
}

//----------------------------------------------------------------------
// Get filled variable
bool SCT_TdaqEnabledCondData::isFilled() const
{
  return m_filled;
}

//----------------------------------------------------------------------
// Set noneBad variable
void SCT_TdaqEnabledCondData::setNoneBad(const bool noneBad)
{
  m_noneBad = noneBad;
}

//----------------------------------------------------------------------
// Get noneBad variable
bool SCT_TdaqEnabledCondData::isNoneBad() const
{
  return m_noneBad;
}

//----------------------------------------------------------------------
// Check if a module is good
bool SCT_TdaqEnabledCondData::isGood(const IdentifierHash& hashId) const
{
  if(m_noneBad) return true;
  return (m_goodIds.find(hashId)!=m_goodIds.end());
}

//----------------------------------------------------------------------
// Clear 
void SCT_TdaqEnabledCondData::clear() {
  m_goodRods.clear();
  m_goodIds.clear();
  m_noneBad = false;
  m_filled = false;
}
