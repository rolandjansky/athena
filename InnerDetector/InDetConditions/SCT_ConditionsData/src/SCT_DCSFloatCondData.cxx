/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for the SCT DCS float data object class for HV and temperature
// The object is a map of channel number <-> float

#include "SCT_ConditionsData/SCT_DCSFloatCondData.h"

//////////////////////////////////
// constructor
SCT_DCSFloatCondData::SCT_DCSFloatCondData():
  m_channelValues{}
{
}

//////////////////////////////////
// destructor
SCT_DCSFloatCondData::~SCT_DCSFloatCondData() {
}

//////////////////////////////////
// set a float value for a channel
void SCT_DCSFloatCondData::setValue(const CondAttrListCollection::ChanNum& chanNum, const float value) {
  m_channelValues[chanNum] = value;
}

//////////////////////////////////
// get the float value for a channel
bool SCT_DCSFloatCondData::getValue(const CondAttrListCollection::ChanNum& chanNum, float& value) const {
  auto itr{m_channelValues.find(chanNum)};
  if(itr!=m_channelValues.end()) {
    value = itr->second;
    return true;
  }
  // the channel is not found.
  return false;
}

//////////////////////////////////
// clear
void SCT_DCSFloatCondData::clear() {
  m_channelValues.clear();
}
