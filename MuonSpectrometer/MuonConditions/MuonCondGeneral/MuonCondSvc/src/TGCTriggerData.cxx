/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/TGCTriggerData.h"
#include "AthenaBaseComps/AthMessaging.h"

TGCTriggerData::TGCTriggerData()
{}

TGCTriggerData::~TGCTriggerData()
{}

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

const std::map<unsigned short, std::map<unsigned short, unsigned char>>& TGCTriggerData::getPtMapBw(const unsigned char side, const unsigned char octant) const
{
  unsigned char octantbit = (side<<3) + octant;
  std::map<unsigned char, std::map<unsigned short, std::map<unsigned short, unsigned char>>>::const_iterator it = m_ptmap_bw.find(octantbit);
  if(it == m_ptmap_bw.end()) return m_ptmap_bw.find(0)->second;  // also for non-full-CW
  return it->second;
}

unsigned short TGCTriggerData::getTrigBitEifi(int side, int slot, int ssc, int sectorId) const
{
  if(m_active[CW_EIFI][0] == false) return 0;   // not required EIFI coincidence.
  int sideindex = (this->getType(CW_EIFI) != "full") ? 0 : side;
  return m_trigbit_eifi[sectorId][ssc][slot][sideindex];
}

unsigned char TGCTriggerData::getFlagPtEifi(int side, int ssc, int sectorId) const
{
  if(m_active[CW_EIFI][0] == false) return 0;   // not required EIFI coincidence.
  int sideindex = (this->getType(CW_EIFI) != "full") ? 0 : side;
  return m_flagpt_eifi[sectorId][ssc][sideindex];
}

unsigned char TGCTriggerData::getFlagRoiEifi(int side, int ssc, int sectorId) const
{
  if(m_active[CW_EIFI][0] == false) return 0;   // not required EIFI coincidence.
  int sideindex = (this->getType(CW_EIFI) != "full") ? 0 : side;
  return m_flagroi_eifi[sectorId][ssc][sideindex];
}

unsigned short TGCTriggerData::getTrigMaskTile(int ssc, int sectorId, int side) const {
  return m_trigbit_tile[ssc][sectorId][side];
}

unsigned char TGCTriggerData::getFlagPtTile(int ssc, int sectorId, int side) const {
  return m_flagpt_tile[ssc][sectorId][side];
}

unsigned char TGCTriggerData::getFlagRoiTile(int ssc, int sectorId, int side) const {
  return m_flagroi_tile[ssc][sectorId][side];
}
