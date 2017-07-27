/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for the data object class for SCT_MonitorConditionsSvc
//----------------------------------------------------------------------

#include "SCT_ConditionsData/SCT_MonitorConditionsCondData.h"

#include <utility>

//----------------------------------------------------------------------
// Constructor
SCT_MonitorConditionsCondData::SCT_MonitorConditionsCondData():
  m_defectListMap{}
{}

//----------------------------------------------------------------------
// Destructor
SCT_MonitorConditionsCondData::~SCT_MonitorConditionsCondData()
{}

//----------------------------------------------------------------------
// Check if a module has a defect (a list of bad strips). If it does not have defect return false.
bool SCT_MonitorConditionsCondData::find(const int& channelNumber, std::string& defectList) const
{
  std::map<const int, const std::string>::const_iterator it{m_defectListMap.find(channelNumber)};
  if(it!=m_defectListMap.end()) {
    // A defect is found, set the defect to defectList and return true.
    defectList = it->second;
    return true;
  }
  // Any defect is not found and return true.
  return false;
}

//----------------------------------------------------------------------
// Insert a new defect (a list of bad strips) for a module
void SCT_MonitorConditionsCondData::insert(const int& channelNumber, const std::string& defectList)
{
  m_defectListMap.insert(std::make_pair(channelNumber, defectList));
}

//----------------------------------------------------------------------
// Clear m_defectListMap
void SCT_MonitorConditionsCondData::clear()
{
  m_defectListMap.clear();
}
