/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for the data object class for SCT_MonitorConditionsSvc
//----------------------------------------------------------------------

#include "SCT_ConditionsData/SCT_MonitorCondData.h"

#include <utility>

//----------------------------------------------------------------------
// Constructor
SCT_MonitorCondData::SCT_MonitorCondData()
{
  clear();
}

//----------------------------------------------------------------------
// Check if a module has a defect (a list of bad strips). If it does not have defect return false.
bool SCT_MonitorCondData::find(const IdentifierHash& hash, std::string& defectList) const
{
  std::string result{m_defectListArray[hash/2]};
  if (result.empty()) return false;
  defectList = result;
  return true;
}

//----------------------------------------------------------------------
// Insert a new defect (a list of bad strips) for a module
void SCT_MonitorCondData::insert(const IdentifierHash& hash, const std::string& defectList)
{
  m_defectListArray[hash/2] = defectList;
}

//----------------------------------------------------------------------
// Clear m_defectListMap
void SCT_MonitorCondData::clear()
{
  m_defectListArray.fill("");
}
