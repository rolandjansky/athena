/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelDCSTempData.h"

void PixelDCSTempData::setTemperature(const int chanNum, const float value) {
  m_temperature[chanNum] = value;
}

float PixelDCSTempData::getTemperature(const int chanNum) const {
  auto itr = m_temperature.find(chanNum);
  if (itr!=m_temperature.end()) { return itr->second; }
  return -7.0; // this is temporaly fix
}


