/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelDeadMapCondData.h"

PixelDeadMapCondData::PixelDeadMapCondData():
  m_moduleStatus(),
  m_chipStatus()
{
}

PixelDeadMapCondData::~PixelDeadMapCondData() { }

// Module status
void PixelDeadMapCondData::setModuleStatus(const int chanNum, const int value) {
  m_moduleStatus[chanNum] = value;
}

int PixelDeadMapCondData::getModuleStatus(const int chanNum) const {
  auto itr = m_moduleStatus.find(chanNum);
  if (itr!=m_moduleStatus.end()) { return itr->second; }
  return 0;
}

// Chip (FE) status
void PixelDeadMapCondData::setChipStatus(const int chanNum, const int value) {
  m_chipStatus[chanNum] = value;
}

int PixelDeadMapCondData::getChipStatus(const int chanNum) const {
  auto itr = m_chipStatus.find(chanNum);
  if (itr!=m_chipStatus.end()) { return itr->second; }
  return 0;
}

void PixelDeadMapCondData::clear() {
  m_moduleStatus.clear();
  m_chipStatus.clear();
}

