/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/TGCTriggerData.h"
#include "AthenaBaseComps/AthMessaging.h"

TGCTriggerData::TGCTriggerData()
{  
}


TGCTriggerData::~TGCTriggerData()
{  
}


std::string TGCTriggerData::getData(int cwtype, std::string file) const {
  auto itr = m_datamap[cwtype].find(file);
  if (itr == m_datamap[cwtype].end()) {
    return "";
  }
 
  return itr->second;
}


std::string TGCTriggerData::getData(int cwtype, int channel) const {
  return m_data[cwtype][channel];
}


std::string TGCTriggerData::getFile(int cwtype, int channel) const {
  return m_file[cwtype][channel];
}

std::string TGCTriggerData::getVersion(int cwtype, int channel) const {
  return m_version[cwtype][channel];
}

std::string TGCTriggerData::getType(int cwtype, int channel) const {
  return m_type[cwtype][channel];
}


bool TGCTriggerData::isActive(int cwtype, int channel) const {
  return m_active[cwtype][channel];
}

std::map<int, std::map<int,int> > TGCTriggerData::getReadMapBw(int side, int octantId, int pt) const {
  return m_readmap_bw[side][octantId][pt];
}

bool TGCTriggerData::getTrigBitEifi(int side, int slot, int ssc, int sectorId, int reg, int read, int bit) const {
  return m_trigbit_eifi[side][slot][ssc][sectorId][reg][read][bit];
}

int TGCTriggerData::getFlagPtEifi(int side, int pt, int ssc, int sectorId) const {
  return m_flagpt_eifi[side][pt][ssc][sectorId];
}

int TGCTriggerData::getFlagRoiEifi(int side, int roi, int ssc, int sectorId) const {
  return m_flagroi_eifi[side][roi][ssc][sectorId];
}

int TGCTriggerData::getTrigMaskTile(int slot, int ssc, int sectorId, int side) const {
  return m_trigbit_tile[slot][ssc][sectorId][side];
}

int TGCTriggerData::getFlagPtTile(int pt, int ssc, int sectorId, int side) const {
  return m_flagpt_tile[pt][ssc][sectorId][side];
}

int TGCTriggerData::getFlagRoiTile(int roi, int ssc, int sectorId, int side) const {
  return m_flagroi_tile[roi][ssc][sectorId][side];
}
