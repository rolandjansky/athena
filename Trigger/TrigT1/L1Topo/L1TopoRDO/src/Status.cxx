/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "L1TopoRDO/Status.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/BlockTypes.h"

namespace L1Topo {

  Status::Status(const uint32_t overflow, const uint32_t crc)
    :m_overflow(overflow), m_crc(crc), m_word(0) {
    this->encode();
  }
  
  Status::Status(const uint32_t word)
    :m_overflow(0), m_crc(0), m_word(word) {
    this->decode();
  }
  
  void Status::encode(){
    m_word = static_cast<int>(L1Topo::BlockTypes::STATUS) << 28;
    m_word |= (m_overflow & 0x1) << 27;
    m_word |= (m_crc & 0x1) << 26;
  }
  
  void Status::decode(){
    m_overflow = (m_word >> 27) & 0x1;
    m_crc = (m_word >> 26) & 0x1;
  }
  
  uint32_t Status::word() const{
    return m_word;
  }
  
  uint32_t Status::overflow() const{
    return m_overflow;
  }
  
  uint32_t Status::crc() const{
    return m_crc;
  }
  std::ostream& operator<<(std::ostream& os, const Status& s) {
    os << "     Status: overflow " << s.overflow() << " crc mismatch " << s.crc();
    return os;
  }
    
} // namespace L1Topo

