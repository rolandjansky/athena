/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelModuleData.h"

PixelModuleData::PixelModuleData():
  m_moduleStatus(),
  m_chipStatus(),
  m_tdaqStatus()
{
}

PixelModuleData::~PixelModuleData() { }

// Module status
void PixelModuleData::setModuleStatus(const CondAttrListCollection::ChanNum& chanNum, const int value) {
  m_moduleStatus[chanNum] = value;
}

int PixelModuleData::getModuleStatus(const CondAttrListCollection::ChanNum& chanNum) const {
  auto itr = m_moduleStatus.find(chanNum);
  if (itr!=m_moduleStatus.end()) { return itr->second; }
  return 0;
}

// Chip (FE) status
void PixelModuleData::setChipStatus(const CondAttrListCollection::ChanNum& chanNum, const int value) {
  m_chipStatus[chanNum] = value;
}

int PixelModuleData::getChipStatus(const CondAttrListCollection::ChanNum& chanNum) const {
  auto itr = m_chipStatus.find(chanNum);
  if (itr!=m_chipStatus.end()) { return itr->second; }
  return 0;
}

// TDAQ module status
void PixelModuleData::setTDAQModuleStatus(const CondAttrListCollection::ChanNum& chanNum, const int value) {
  m_tdaqStatus[chanNum] = value;
}

int PixelModuleData::getTDAQModuleStatus(const CondAttrListCollection::ChanNum& chanNum) const {
  auto itr = m_tdaqStatus.find(chanNum);
  if (itr!=m_tdaqStatus.end()) { return itr->second; }
  return 1;
}

void PixelModuleData::clear() {
  m_moduleStatus.clear();
  m_chipStatus.clear();
  m_tdaqStatus.clear();
}

