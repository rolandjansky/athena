/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/TGCTriggerData.h"
#include "AthenaBaseComps/AthMessaging.h"

TGCTriggerData::TGCTriggerData()
{}

TGCTriggerData::~TGCTriggerData()
{}

std::string TGCTriggerData::getType(int cwtype) const
{
  return m_type[cwtype];
}

bool TGCTriggerData::isActive(int cwtype) const
{
  return m_active[cwtype];
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
  int sideindex = (m_type[CW_EIFI] != "full") ? 0 : side;
  uint16_t addr = ((sideindex & SIDE_MASK)<<ADDR_SIDE_SHIFT) +
                  ((sectorId & SECTOR_MASK)<<ADDR_SECTOR_SHIFT) +
                  (ssc & SSC_MASK);
  addr = (addr<<EIFI_TRIGBIT_SHIFT) + slot;
  std::unordered_map<uint16_t, uint16_t>::const_iterator it = m_trigbit_eifi.find(addr);
  if(it == m_trigbit_eifi.end()) return 0x0;        // undefined sector (or active=0)
  else                           return it->second;
}

unsigned char TGCTriggerData::getFlagPtEifi(int side, int ssc, int sectorId) const
{
  int sideindex = (m_type[CW_EIFI] != "full") ? 0 : side;
  uint16_t addr = ((sideindex & SIDE_MASK)<<ADDR_SIDE_SHIFT) +
                  ((sectorId & SECTOR_MASK)<<ADDR_SECTOR_SHIFT) +
                  (ssc & SSC_MASK);
  std::unordered_map<uint16_t, uint8_t>::const_iterator it = m_flagpt_eifi.find(addr);
  if(it == m_flagpt_eifi.end()) return 0x0;        // undefined sector (or active=0)
  else                          return it->second;
}

unsigned char TGCTriggerData::getFlagRoiEifi(int side, int ssc, int sectorId) const
{
  int sideindex = (m_type[CW_EIFI] != "full") ? 0 : side;
  uint16_t addr = ((sideindex & SIDE_MASK)<<ADDR_SIDE_SHIFT) +
                  ((sectorId & SECTOR_MASK)<<ADDR_SECTOR_SHIFT) +
                  (ssc & SSC_MASK);
  std::unordered_map<uint16_t, uint8_t>::const_iterator it = m_flagroi_eifi.find(addr);
  if(it == m_flagroi_eifi.end()) return 0x0;        // undefined sector (or active=0)
  else                           return it->second;
}

uint16_t TGCTriggerData::getTrigMaskTile(int ssc, int sectorId, int side) const
{
  uint16_t addr = ((side & SIDE_MASK)<<ADDR_SIDE_SHIFT) +
                  ((sectorId & SECTOR_MASK)<<ADDR_SECTOR_SHIFT) +
                  (ssc & SSC_MASK);
  std::unordered_map<uint16_t, uint16_t>::const_iterator it = m_trigbit_tile.find(addr);
  if(it == m_trigbit_tile.end()) return 0x0;        // undefined sector (or active=0)
  else                           return it->second;
}

uint8_t TGCTriggerData::getFlagPtTile(int ssc, int sectorId, int side) const
{
  uint16_t addr = ((side & SIDE_MASK)<<ADDR_SIDE_SHIFT) +
                  ((sectorId & SECTOR_MASK)<<ADDR_SECTOR_SHIFT) +
                  (ssc & SSC_MASK);
  std::unordered_map<uint16_t, uint8_t>::const_iterator it = m_flagpt_tile.find(addr);
  if(it == m_flagpt_tile.end()) return 0x0;        // undefined sector (or active=0)
  else                          return it->second;
}

uint8_t TGCTriggerData::getFlagRoiTile(int ssc, int sectorId, int side) const
{
  uint16_t addr = ((side & SIDE_MASK)<<ADDR_SIDE_SHIFT) +
                  ((sectorId & SECTOR_MASK)<<ADDR_SECTOR_SHIFT) +
                  (ssc & SSC_MASK);
  std::unordered_map<uint16_t, uint8_t>::const_iterator it = m_flagroi_tile.find(addr);
  if(it == m_flagroi_tile.end()) return 0x0;        // undefined sector (or active=0)
  else                           return it->second;
}

