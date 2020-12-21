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

int8_t TGCTriggerData::getTYPE(const int16_t lDR, const int16_t hDR, const int16_t lDPhi, const int16_t hDPhi) const
{
  if((lDR == -DR_HIGH_RANGE) && (hDR == DR_HIGH_RANGE)) {
    if     ((lDPhi == -DPHI_HIGH_RANGE) && (hDPhi == DPHI_HIGH_RANGE)) return COIN_HH;
    else if((lDPhi == -DPHI_LOW_RANGE) && (hDPhi == DPHI_LOW_RANGE))   return COIN_HL;
  } else if((lDR == -DR_LOW_RANGE) && (hDR == DR_LOW_RANGE)) {
    if     ((lDPhi == -DPHI_HIGH_RANGE) && (hDPhi == DPHI_HIGH_RANGE)) return COIN_LH;
    else if((lDPhi == -DPHI_LOW_RANGE) && (hDPhi == DPHI_LOW_RANGE))   return COIN_LL;
  }
  return -1;
}

uint8_t TGCTriggerData::getBigWheelPt(const uint32_t addr) const
{
  std::unordered_map<uint32_t, uint8_t>::const_iterator it = m_ptmap_bw.find(addr);
  if(it == m_ptmap_bw.end()) return 0x0;        // outside from defined window, i.e. pT=0
  else                       return it->second;
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
