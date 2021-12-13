/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCTriggerCondSvc/TGCTriggerLUTs.h"
#include "AthenaBaseComps/AthMessaging.h"

TGCTriggerLUTs::TGCTriggerLUTs()
{  
}


TGCTriggerLUTs::~TGCTriggerLUTs()
{  
}


std::string TGCTriggerLUTs::getData(int cwtype, std::string file) const {
  auto itr = m_datamap[cwtype].find(file);
  if (itr == m_datamap[cwtype].end()) {
    return "";
  }
 
  return itr->second;
}


std::string TGCTriggerLUTs::getData(int cwtype, int channel) const {
  return m_data[cwtype][channel];
}


std::string TGCTriggerLUTs::getFile(int cwtype, int channel) const {
  return m_file[cwtype][channel];
}

std::string TGCTriggerLUTs::getVersion(int cwtype, int channel) const {
  return m_version[cwtype][channel];
}

std::string TGCTriggerLUTs::getType(int cwtype, int channel) const {
  return m_type[cwtype][channel];
}


int8_t TGCTriggerLUTs::getBigWheelPt(const uint32_t addr) const {
  std::unordered_map<uint32_t, int8_t>::const_iterator it = m_ptmap_bw.find(addr);
  if(it == m_ptmap_bw.end()) return 0x0;        // outside from defined window, i.e. pT=0
  else                       return it->second;
}
