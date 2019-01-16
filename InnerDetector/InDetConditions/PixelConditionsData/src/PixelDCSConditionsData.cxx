/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelDCSConditionsData.h"

PixelDCSConditionsData::PixelDCSConditionsData():
  m_channelValues()
{
}

PixelDCSConditionsData::~PixelDCSConditionsData() { }

void PixelDCSConditionsData::setValue(const int chanNum, const float value) {
  m_channelValues[chanNum] = value;
}

bool PixelDCSConditionsData::getValue(const int chanNum, float& value) const {
  auto itr = m_channelValues.find(chanNum);
  if (itr!=m_channelValues.end()) {
    value = itr->second;
    return true;
  }
  return false;
}

void PixelDCSConditionsData::clear() {
  m_channelValues.clear();
}

