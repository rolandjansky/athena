/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelModuleData.h"

PixelModuleData::PixelModuleData():
  m_moduleStatus(),
  m_chipStatus()
{
}

PixelModuleData::~PixelModuleData() { }

// Module status
void PixelModuleData::setModuleStatus(const int chanNum, const int value) {
  m_moduleStatus[chanNum] = value;
}

int PixelModuleData::getModuleStatus(const int chanNum) const {
  auto itr = m_moduleStatus.find(chanNum);
  if (itr!=m_moduleStatus.end()) { return itr->second; }
  return 0;
}

// Chip (FE) status
void PixelModuleData::setChipStatus(const int chanNum, const int value) {
  m_chipStatus[chanNum] = value;
}

int PixelModuleData::getChipStatus(const int chanNum) const {
  auto itr = m_chipStatus.find(chanNum);
  if (itr!=m_chipStatus.end()) { return itr->second; }
  return 0;
}

void PixelModuleData::setBarrelAnalogThreshold(std::vector<int> BarrelAnalogThreshold) { m_BarrelAnalogThreshold = BarrelAnalogThreshold; }
void PixelModuleData::setEndcapAnalogThreshold(std::vector<int> EndcapAnalogThreshold) { m_EndcapAnalogThreshold = EndcapAnalogThreshold; }
void PixelModuleData::setDBMAnalogThreshold(std::vector<int>    DBMAnalogThreshold)    { m_DBMAnalogThreshold = DBMAnalogThreshold; }

int PixelModuleData::getAnalogThreshold(int bec, int layer) const {
  int analogThreshold = -1;
  if (std::abs(bec)==0 && layer<(int)m_BarrelAnalogThreshold.size()) { analogThreshold=m_BarrelAnalogThreshold.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapAnalogThreshold.size()) { analogThreshold=m_EndcapAnalogThreshold.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMAnalogThreshold.size())    { analogThreshold=m_DBMAnalogThreshold.at(layer); }
  return analogThreshold;
}

void PixelModuleData::setBarrelToTThreshold(std::vector<int> BarrelToTThreshold) { m_BarrelToTThreshold = BarrelToTThreshold; }
void PixelModuleData::setEndcapToTThreshold(std::vector<int> EndcapToTThreshold) { m_EndcapToTThreshold = EndcapToTThreshold; }
void PixelModuleData::setDBMToTThreshold(std::vector<int>    DBMToTThreshold)    { m_DBMToTThreshold = DBMToTThreshold; }

int PixelModuleData::getToTThreshold(int bec, int layer) const {
  int totThreshold = -1;
  if (std::abs(bec)==0 && layer<(int)m_BarrelToTThreshold.size()) { totThreshold=m_BarrelToTThreshold.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapToTThreshold.size()) { totThreshold=m_EndcapToTThreshold.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMToTThreshold.size())    { totThreshold=m_DBMToTThreshold.at(layer); }
  return totThreshold;
}

void PixelModuleData::setBarrelLatency(std::vector<int> BarrelLatency) { m_BarrelLatency = BarrelLatency; }
void PixelModuleData::setEndcapLatency(std::vector<int> EndcapLatency) { m_EndcapLatency = EndcapLatency; }
void PixelModuleData::setDBMLatency(std::vector<int>    DBMLatency)    { m_DBMLatency = DBMLatency; }

int PixelModuleData::getLatency(int bec, int layer) const {
  int  latency = -1;
  if (std::abs(bec)==0 && layer<(int)m_BarrelLatency.size()) { latency=m_BarrelLatency.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapLatency.size()) { latency=m_EndcapLatency.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMLatency.size())    { latency=m_DBMLatency.at(layer); }
  return latency;
}

void PixelModuleData::setBarrelCrossTalk(std::vector<double> BarrelCrossTalk) { m_BarrelCrossTalk = BarrelCrossTalk; }
void PixelModuleData::setEndcapCrossTalk(std::vector<double> EndcapCrossTalk) { m_EndcapCrossTalk = EndcapCrossTalk; }
void PixelModuleData::setDBMCrossTalk(std::vector<double>    DBMCrossTalk)    { m_DBMCrossTalk = DBMCrossTalk; }

double PixelModuleData::getCrossTalk(int bec, int layer) const { 
  double crossTalk = -1.0;
  if (std::abs(bec)==0 && layer<(int)m_BarrelCrossTalk.size()) { crossTalk=m_BarrelCrossTalk.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapCrossTalk.size()) { crossTalk=m_EndcapCrossTalk.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMCrossTalk.size())    { crossTalk=m_DBMCrossTalk.at(layer); }
  return crossTalk;
}

void PixelModuleData::setBarrelThermalNoise(std::vector<double> BarrelThermalNoise) { m_BarrelThermalNoise = BarrelThermalNoise; }
void PixelModuleData::setEndcapThermalNoise(std::vector<double> EndcapThermalNoise) { m_EndcapThermalNoise = EndcapThermalNoise; }
void PixelModuleData::setDBMThermalNoise(std::vector<double>    DBMThermalNoise) { m_DBMThermalNoise = DBMThermalNoise; }

double PixelModuleData::getThermalNoise(int bec, int layer) const {
  double noise = -1.0;
  if (std::abs(bec)==0 && layer<(int)m_BarrelThermalNoise.size()) { noise=m_BarrelThermalNoise.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapThermalNoise.size()) { noise=m_EndcapThermalNoise.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMThermalNoise.size())    { noise=m_DBMThermalNoise.at(layer); }
  return noise;
}

void PixelModuleData::setBarrelHitDuplication(std::vector<bool> BarrelHitDuplication) { m_BarrelHitDuplication = BarrelHitDuplication; }
void PixelModuleData::setEndcapHitDuplication(std::vector<bool> EndcapHitDuplication) { m_EndcapHitDuplication = EndcapHitDuplication; }
void PixelModuleData::setDBMHitDuplication(std::vector<bool>    DBMHitDuplication) { m_DBMHitDuplication = DBMHitDuplication; }

bool PixelModuleData::getHitDuplication(int bec, int layer) const {
  bool hitdupli = false;
  if (std::abs(bec)==0 && layer<(int)m_BarrelHitDuplication.size()) { hitdupli=m_BarrelHitDuplication.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapHitDuplication.size()) { hitdupli=m_EndcapHitDuplication.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMHitDuplication.size())    { hitdupli=m_DBMHitDuplication.at(layer); }
  return hitdupli;
}

void PixelModuleData::setBarrelSmallHitToT(std::vector<int> BarrelSmallHitToT) { m_BarrelSmallHitToT = BarrelSmallHitToT; }
void PixelModuleData::setEndcapSmallHitToT(std::vector<int> EndcapSmallHitToT) { m_EndcapSmallHitToT = EndcapSmallHitToT; }
void PixelModuleData::setDBMSmallHitToT(std::vector<int>    DBMSmallHitToT) { m_DBMSmallHitToT = DBMSmallHitToT; }

int PixelModuleData::getSmallHitToT(int bec, int layer) const {
  int smallToT = -1;
  if (std::abs(bec)==0 && layer<(int)m_BarrelSmallHitToT.size()) { smallToT=m_BarrelSmallHitToT.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapSmallHitToT.size()) { smallToT=m_EndcapSmallHitToT.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMSmallHitToT.size())    { smallToT=m_DBMSmallHitToT.at(layer); }
  return smallToT;
}

void PixelModuleData::setIBLHitDiscConfig(const int hitDiscConfig) { m_hitDiscConfig = hitDiscConfig; }
int PixelModuleData::getIBLHitDiscConfig() const { return m_hitDiscConfig; }

int PixelModuleData::getIBLOverflowToT() const {
  static const int overflow[]={14,15,16,14,16};
  unsigned int idx=static_cast<unsigned int>(m_hitDiscConfig) < 4 ? m_hitDiscConfig : 4;
  return overflow[idx];
}

void PixelModuleData::clear() {
  m_moduleStatus.clear();
  m_chipStatus.clear();
}

