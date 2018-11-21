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
int PixelModuleData::getBarrelAnalogThreshold(int chanNum) const { return m_BarrelAnalogThreshold.at(chanNum); }
int PixelModuleData::getEndcapAnalogThreshold(int chanNum) const { return m_EndcapAnalogThreshold.at(chanNum); }
int PixelModuleData::getDBMAnalogThreshold(int chanNum)    const { return m_DBMAnalogThreshold.at(chanNum); }

void PixelModuleData::setBarrelToTThreshold(std::vector<int> BarrelToTThreshold) { m_BarrelToTThreshold = BarrelToTThreshold; }
void PixelModuleData::setEndcapToTThreshold(std::vector<int> EndcapToTThreshold) { m_EndcapToTThreshold = EndcapToTThreshold; }
void PixelModuleData::setDBMToTThreshold(std::vector<int>    DBMToTThreshold)    { m_DBMToTThreshold = DBMToTThreshold; }
int PixelModuleData::getBarrelToTThreshold(int chanNum) const { return m_BarrelToTThreshold.at(chanNum); }
int PixelModuleData::getEndcapToTThreshold(int chanNum) const { return m_EndcapToTThreshold.at(chanNum); }
int PixelModuleData::getDBMToTThreshold(int chanNum)    const { return m_DBMToTThreshold.at(chanNum); }

void PixelModuleData::setBarrelLatency(std::vector<int> BarrelLatency) { m_BarrelLatency = BarrelLatency; }
void PixelModuleData::setEndcapLatency(std::vector<int> EndcapLatency) { m_EndcapLatency = EndcapLatency; }
void PixelModuleData::setDBMLatency(std::vector<int>    DBMLatency)    { m_DBMLatency = DBMLatency; }
int PixelModuleData::getBarrelLatency(int chanNum) const { return m_BarrelLatency.at(chanNum); }
int PixelModuleData::getEndcapLatency(int chanNum) const { return m_EndcapLatency.at(chanNum); }
int PixelModuleData::getDBMLatency(int chanNum)    const { return m_DBMLatency.at(chanNum); }

void PixelModuleData::setBarrelCrossTalk(std::vector<double> BarrelCrossTalk) { m_BarrelCrossTalk = BarrelCrossTalk; }
void PixelModuleData::setEndcapCrossTalk(std::vector<double> EndcapCrossTalk) { m_EndcapCrossTalk = EndcapCrossTalk; }
void PixelModuleData::setDBMCrossTalk(std::vector<double>    DBMCrossTalk)    { m_DBMCrossTalk = DBMCrossTalk; }
double PixelModuleData::getBarrelCrossTalk(int chanNum) const { return m_BarrelCrossTalk.at(chanNum); }
double PixelModuleData::getEndcapCrossTalk(int chanNum) const { return m_EndcapCrossTalk.at(chanNum); }
double PixelModuleData::getDBMCrossTalk(int chanNum)    const { return m_DBMCrossTalk.at(chanNum); }

void PixelModuleData::setBarrelThermalNoise(std::vector<double> BarrelThermalNoise) { m_BarrelThermalNoise = BarrelThermalNoise; }
void PixelModuleData::setEndcapThermalNoise(std::vector<double> EndcapThermalNoise) { m_EndcapThermalNoise = EndcapThermalNoise; }
void PixelModuleData::setDBMThermalNoise(std::vector<double>    DBMThermalNoise) { m_DBMThermalNoise = DBMThermalNoise; }
double PixelModuleData::getBarrelThermalNoise(int chanNum) const { return m_BarrelThermalNoise.at(chanNum); }
double PixelModuleData::getEndcapThermalNoise(int chanNum) const { return m_EndcapThermalNoise.at(chanNum); }
double PixelModuleData::getDBMThermalNoise(int chanNum)    const { return m_DBMThermalNoise.at(chanNum); }

void PixelModuleData::setBarrelHitDuplication(std::vector<bool> BarrelHitDuplication) { m_BarrelHitDuplication = BarrelHitDuplication; }
void PixelModuleData::setEndcapHitDuplication(std::vector<bool> EndcapHitDuplication) { m_EndcapHitDuplication = EndcapHitDuplication; }
void PixelModuleData::setDBMHitDuplication(std::vector<bool>    DBMHitDuplication) { m_DBMHitDuplication = DBMHitDuplication; }
bool PixelModuleData::getBarrelHitDuplication(int chanNum) const { return m_BarrelHitDuplication.at(chanNum); }
bool PixelModuleData::getEndcapHitDuplication(int chanNum) const { return m_EndcapHitDuplication.at(chanNum); }
bool PixelModuleData::getDBMHitDuplication(int chanNum)    const { return m_DBMHitDuplication.at(chanNum); }

void PixelModuleData::setBarrelSmallHitToT(std::vector<int> BarrelSmallHitToT) { m_BarrelSmallHitToT = BarrelSmallHitToT; }
void PixelModuleData::setEndcapSmallHitToT(std::vector<int> EndcapSmallHitToT) { m_EndcapSmallHitToT = EndcapSmallHitToT; }
void PixelModuleData::setDBMSmallHitToT(std::vector<int>    DBMSmallHitToT) { m_DBMSmallHitToT = DBMSmallHitToT; }
int PixelModuleData::getBarrelSmallHitToT(int chanNum) const { return m_BarrelSmallHitToT.at(chanNum); }
int PixelModuleData::getEndcapSmallHitToT(int chanNum) const { return m_EndcapSmallHitToT.at(chanNum); }
int PixelModuleData::getDBMSmallHitToT(int chanNum)    const { return m_DBMSmallHitToT.at(chanNum); }

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

