/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for the data object class for SCT_MajorityConditionsSvc
//----------------------------------------------------------------------

#include "SCT_ConditionsData/SCT_MajorityCondData.h"

//----------------------------------------------------------------------
// Constructor
SCT_MajorityCondData::SCT_MajorityCondData():
  m_majorityState{},
  m_hvFraction{},
  m_filled{false}
{}

//----------------------------------------------------------------------
// Destructor
SCT_MajorityCondData::~SCT_MajorityCondData()
{}

//----------------------------------------------------------------------
// Set majority state for a region
void SCT_MajorityCondData::setMajorityState(const int& region, const bool& majorityState)
{
  m_majorityState[region] = majorityState;
}

//----------------------------------------------------------------------
// Get majority state for a region
bool SCT_MajorityCondData::getMajorityState(const int& region) const
{
  std::map<int, bool>::const_iterator it{m_majorityState.find(region)};
  if (it != m_majorityState.end()) return (*it).second;
  return true; // If the region is not found, true is returned.
}

//----------------------------------------------------------------------
// Clear majority states
void SCT_MajorityCondData::clearMajorityStates()
{
  m_majorityState.clear();
  setFilled(false);
}

//----------------------------------------------------------------------
// Set majority state for a region
void SCT_MajorityCondData::setHVFraction(const int& region, const float& hvFraction)
{
  m_hvFraction[region] = hvFraction;
}

//----------------------------------------------------------------------
// Get majority state for a region
float SCT_MajorityCondData::getHVFraction(const int& region) const
{
  std::map<int, float>::const_iterator it{m_hvFraction.find(region)};
  if (it != m_hvFraction.end()) return (*it).second;
  return 1.; // If the region is not found, 1. is returned.
}

//----------------------------------------------------------------------
// Clear majority states
void SCT_MajorityCondData::clearHVFractions()
{
  m_hvFraction.clear();
  setFilled(false);
}

//----------------------------------------------------------------------
// Set filled variable
void SCT_MajorityCondData::setFilled(const bool& filled)
{
  m_filled = filled;
}

//----------------------------------------------------------------------
// Set filled variable
bool SCT_MajorityCondData::isFilled() const
{
  return m_filled;
}
