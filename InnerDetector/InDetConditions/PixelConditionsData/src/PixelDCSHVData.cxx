/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelDCSHVData.h"

void PixelDCSHVData::setBiasVoltage(const int chanNum, const float value) {
  m_biasVoltage[chanNum] = value;
}

float PixelDCSHVData::getBiasVoltage(const int chanNum) const {
  auto itr = m_biasVoltage.find(chanNum);
  if (itr!=m_biasVoltage.end()) { return itr->second; }
  return 0;
}

