/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelDCSConditionsData.h"

PixelDCSConditionsData::PixelDCSConditionsData():
  m_channelValues()
{
}

PixelDCSConditionsData::~PixelDCSConditionsData() { }

void PixelDCSConditionsData::setValue(const CondAttrListCollection::ChanNum& chanNum, const float value) {
  m_channelValues[chanNum] = value;
}

void PixelDCSConditionsData::setValue(const CondAttrListCollection::ChanNum& chanNum, const std::string value) {
  m_channelStrings[chanNum] = value;
}

bool PixelDCSConditionsData::getValue(const CondAttrListCollection::ChanNum& chanNum, float& value) const {
  auto itr = m_channelValues.find(chanNum);
  if (itr!=m_channelValues.end()) {
    value = itr->second;
    return true;
  }
  return false;
}

bool PixelDCSConditionsData::getValue(const CondAttrListCollection::ChanNum& chanNum, std::string& value) const {
  auto itr = m_channelStrings.find(chanNum);
  if (itr!=m_channelStrings.end()) {
    value = itr->second;
    return true;
  }
  return false;
}

void PixelDCSConditionsData::clear() {
  m_channelValues.clear();
}

