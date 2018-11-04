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

void PixelModuleData::setBarrelAnalogThreshold(std::vector<int> BarrelAnalogThreshold) { m_BarrelAnalogThreshold = BarrelAnalogThreshold; }
void PixelModuleData::setEndcapAnalogThreshold(std::vector<int> EndcapAnalogThreshold) { m_EndcapAnalogThreshold = EndcapAnalogThreshold; }
void PixelModuleData::setDBMAnalogThreshold(std::vector<int>    DBMAnalogThreshold)    { m_DBMAnalogThreshold = DBMAnalogThreshold; }
std::vector<int> PixelModuleData::getBarrelAnalogThreshold() const { return m_BarrelAnalogThreshold; }
std::vector<int> PixelModuleData::getEndcapAnalogThreshold() const { return m_EndcapAnalogThreshold; }
std::vector<int> PixelModuleData::getDBMAnalogThreshold()    const { return m_DBMAnalogThreshold; }

void PixelModuleData::setBarrelToTThreshold(std::vector<int> BarrelToTThreshold) { m_BarrelToTThreshold = BarrelToTThreshold; }
void PixelModuleData::setEndcapToTThreshold(std::vector<int> EndcapToTThreshold) { m_EndcapToTThreshold = EndcapToTThreshold; }
void PixelModuleData::setDBMToTThreshold(std::vector<int>    DBMToTThreshold)    { m_DBMToTThreshold = DBMToTThreshold; }
std::vector<int> PixelModuleData::getBarrelToTThreshold() const { return m_BarrelToTThreshold; }
std::vector<int> PixelModuleData::getEndcapToTThreshold() const { return m_EndcapToTThreshold; }
std::vector<int> PixelModuleData::getDBMToTThreshold()    const { return m_DBMToTThreshold; }

void PixelModuleData::setBarrelLatency(std::vector<int> BarrelLatency) { m_BarrelLatency = BarrelLatency; }
void PixelModuleData::setEndcapLatency(std::vector<int> EndcapLatency) { m_EndcapLatency = EndcapLatency; }
void PixelModuleData::setDBMLatency(std::vector<int>    DBMLatency)    { m_DBMLatency = DBMLatency; }
std::vector<int> PixelModuleData::getBarrelLatency() const { return m_BarrelLatency; }
std::vector<int> PixelModuleData::getEndcapLatency() const { return m_EndcapLatency; }
std::vector<int> PixelModuleData::getDBMLatency()    const { return m_DBMLatency; }

void PixelModuleData::setBarrelCrossTalk(std::vector<double> BarrelCrossTalk) { m_BarrelCrossTalk = BarrelCrossTalk; }
void PixelModuleData::setEndcapCrossTalk(std::vector<double> EndcapCrossTalk) { m_EndcapCrossTalk = EndcapCrossTalk; }
void PixelModuleData::setDBMCrossTalk(std::vector<double>    DBMCrossTalk)    { m_DBMCrossTalk = DBMCrossTalk; }
std::vector<double> PixelModuleData::getBarrelCrossTalk() const { return m_BarrelCrossTalk; }
std::vector<double> PixelModuleData::getEndcapCrossTalk() const { return m_EndcapCrossTalk; }
std::vector<double> PixelModuleData::getDBMCrossTalk()    const { return m_DBMCrossTalk; }

void PixelModuleData::setBarrelThermalNoise(std::vector<double> BarrelThermalNoise) { m_BarrelThermalNoise = BarrelThermalNoise; }
void PixelModuleData::setEndcapThermalNoise(std::vector<double> EndcapThermalNoise) { m_EndcapThermalNoise = EndcapThermalNoise; }
void PixelModuleData::setDBMThermalNoise(std::vector<double>    DBMThermalNoise) { m_DBMThermalNoise = DBMThermalNoise; }
std::vector<double> PixelModuleData::getBarrelThermalNoise() const { return m_BarrelThermalNoise; }
std::vector<double> PixelModuleData::getEndcapThermalNoise() const { return m_EndcapThermalNoise; }
std::vector<double> PixelModuleData::getDBMThermalNoise()    const { return m_DBMThermalNoise; }

void PixelModuleData::setBarrelHitDuplication(std::vector<bool> BarrelHitDuplication) { m_BarrelHitDuplication = BarrelHitDuplication; }
void PixelModuleData::setEndcapHitDuplication(std::vector<bool> EndcapHitDuplication) { m_EndcapHitDuplication = EndcapHitDuplication; }
void PixelModuleData::setDBMHitDuplication(std::vector<bool>    DBMHitDuplication) { m_DBMHitDuplication = DBMHitDuplication; }
std::vector<bool> PixelModuleData::getBarrelHitDuplication() const { return m_BarrelHitDuplication; }
std::vector<bool> PixelModuleData::getEndcapHitDuplication() const { return m_EndcapHitDuplication; }
std::vector<bool> PixelModuleData::getDBMHitDuplication()    const { return m_DBMHitDuplication; }

void PixelModuleData::setBarrelSmallHitToT(std::vector<int> BarrelSmallHitToT) { m_BarrelSmallHitToT = BarrelSmallHitToT; }
void PixelModuleData::setEndcapSmallHitToT(std::vector<int> EndcapSmallHitToT) { m_EndcapSmallHitToT = EndcapSmallHitToT; }
void PixelModuleData::setDBMSmallHitToT(std::vector<int>    DBMSmallHitToT) { m_DBMSmallHitToT = DBMSmallHitToT; }
std::vector<int> PixelModuleData::getBarrelSmallHitToT() const { return m_BarrelSmallHitToT; }
std::vector<int> PixelModuleData::getEndcapSmallHitToT() const { return m_EndcapSmallHitToT; }
std::vector<int> PixelModuleData::getDBMSmallHitToT()    const { return m_DBMSmallHitToT; }

void PixelModuleData::setIBLHitDiscConfig(const int hitDiscConfig) { m_hitDiscConfig = hitDiscConfig; }
int PixelModuleData::getIBLHitDiscConfig() const { return m_hitDiscConfig; }

int PixelModuleData::getIBLOverflowToT() const {
  int overflow = 16;
  if (m_hitDiscConfig== 0) { overflow=14; }
  if (m_hitDiscConfig== 1) { overflow=15; }
  if (m_hitDiscConfig== 2) { overflow=16; }
  if (m_hitDiscConfig== 3) { overflow=14; }
  return overflow;
}

void PixelModuleData::clear() {
  m_moduleStatus.clear();
  m_chipStatus.clear();
  m_tdaqStatus.clear();
}

