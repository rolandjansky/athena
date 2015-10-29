/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "L1TopoRDO/ModuleID.h"
#include "L1TopoRDO/Helpers.h"

namespace L1Topo {

  ModuleID::ModuleID(unsigned int link, unsigned int module, unsigned int roiDaq)
    :m_link(link), m_module(module), m_roiDaq(roiDaq), m_id(0) {
    this->encode();
  }
  
  ModuleID::ModuleID(const uint16_t id)
    :m_link(0), m_module(0), m_roiDaq(0), m_id(id) {
    this->decode();
  }

  uint16_t ModuleID::roiDaq() const {
    return m_roiDaq;
  }

  bool ModuleID::isDAQ() const {
    return m_roiDaq==0;
  }

  bool ModuleID::isROI() const {
    return m_roiDaq==1;
  }

  uint16_t ModuleID::module() const {
    return m_module;
  }

  uint16_t ModuleID::link() const {
    return m_link;
  }

  uint16_t ModuleID::id() const {
    return m_id;
  }

  void ModuleID::encode() {
    m_id  = (m_roiDaq & 0x1) << 7;
    m_id |= (m_module & 0x7) << 4;
    m_id |= (m_link & 0xf);
  }

  void ModuleID::decode() {
    m_roiDaq = (m_id >> 7) & 0x1;
    m_module = (m_id >> 4) & 0x7;
    m_link = m_id & 0xf;
  }

  std::ostream& operator<<(std::ostream& os, const ModuleID& m) {
    os << "ModuleID: " << " " << formatHex4(m.id()) << " 0b" << std::bitset<8>(m.id()) << " link " << m.link() << " " << " module " << m.module();
    if (m.isDAQ()){
      os << " DAQ";
    } 
    else if (m.isROI()){
      os << " ROI";
    }
    return os;
  }

} // namespace L1Topo

