/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include <bitset>
#include "L1TopoRDO/L1TopoTOB.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/BlockTypes.h"

namespace L1Topo {

  L1TopoTOB::L1TopoTOB(const uint32_t ctp_signal, uint32_t overflow_bits, uint32_t trigger_bits, uint32_t error_bits)
    : m_ctp_signal(ctp_signal), m_overflow_bits(overflow_bits), m_trigger_bits(trigger_bits),m_error_bits(error_bits),m_word(0){
    this->encode();
  }
  L1TopoTOB::L1TopoTOB(const uint32_t ctp_signal, uint32_t overflow_bits, uint32_t trigger_bits)
    : m_ctp_signal(ctp_signal), m_overflow_bits(overflow_bits), m_trigger_bits(trigger_bits),m_error_bits(0),m_word(0){
    this->encode();
  }
  L1TopoTOB::L1TopoTOB(const uint32_t word)
    : m_word(word){
    this->decode();
  }

  bool operator==(const L1Topo::L1TopoTOB& lhs, const  L1Topo::L1TopoTOB& rhs){
    return (lhs.word()==rhs.word());
  }
  bool operator!=(const L1Topo::L1TopoTOB& lhs, const  L1Topo::L1TopoTOB& rhs){
    return (lhs.word()!=rhs.word());
  }
  bool operator<(const L1Topo::L1TopoTOB& lhs, const  L1Topo::L1TopoTOB& rhs){
    return (lhs.word()<rhs.word());
  }
  bool operator>(const L1Topo::L1TopoTOB& lhs, const  L1Topo::L1TopoTOB& rhs){
    return (lhs.word()>rhs.word());
  }
  
  /* See https://twiki.cern.ch/twiki/pub/Atlas/L1CaloUpgrade/ROD_data_format_v1.0.8.xlsx */

  void L1TopoTOB::decode(){
    // assert (L1Topo::blockType(m_word)==L1Topo::BlockTypes::L1TOPO_TOB);
    m_trigger_bits = m_word & 0xff;
    m_overflow_bits = (m_word>>8) & 0xff;
    m_error_bits = (m_word>>16) & 0x1f;
    m_ctp_signal = (m_word>>24) & 0xf;
    return;
  }

  void L1TopoTOB::encode(){
    m_word = static_cast<int>(L1Topo::BlockTypes::L1TOPO_TOB) << 28;
    m_word |= (m_trigger_bits & 0xff);
    m_word |= (m_overflow_bits & 0xff) << 8;
    m_word |= (m_error_bits & 0x1f) << 16;
    m_word |= (m_ctp_signal &= 0xf) << 24; 
    return;
  }

  uint32_t L1TopoTOB::ctp_signal() const{
    return m_ctp_signal;
  }
  uint32_t L1TopoTOB::error_bits() const{
    return m_error_bits;
  }
  bool L1TopoTOB::crc_EM() const{
    return (m_error_bits & 0x01);
  }
  bool L1TopoTOB::crc_Tau() const{
    return (m_error_bits & 0x02);
  }
  bool L1TopoTOB::crc_Muon() const{
    return (m_error_bits & 0x04);
  }
  bool L1TopoTOB::crc_Jet() const{
    return (m_error_bits & 0x08);
  }
  bool L1TopoTOB::crc_Energy() const{
    return (m_error_bits & 0x10);
  }
  uint32_t L1TopoTOB::overflow_bits() const{
    return m_overflow_bits;
  }
  uint32_t L1TopoTOB::trigger_bits() const{
    return m_trigger_bits;
  }
  uint32_t L1TopoTOB::word() const{
    return m_word;
  }

  // CTP index can be unpacked into 3 bits: index, clock and fpga
  // 
  uint32_t L1TopoTOB::index() const{
    return m_ctp_signal & 0x1;
  }
  uint32_t L1TopoTOB::clock() const{
    return (m_ctp_signal >> 1) & 0x1;
  }
  uint32_t L1TopoTOB::fpga() const{
    return (m_ctp_signal >> 3) & 0x1;
  }

  std::ostream& operator<<(std::ostream& os, const L1TopoTOB& c) {
    os << "     L1Topo TOB: "
       << " ctp_signal 0x" << std::hex << c.ctp_signal() << std::dec
       << " ( index " << c.index() << " clock " << c.clock() << " fpga " << c.fpga() << " )"
       << " CRC error bits 0b" << std::bitset<5>(c.error_bits())
       << " overflow bits 0b" << std::bitset<8>(c.overflow_bits())
       << " trigger bits 0b" << std::bitset<8>(c.trigger_bits())
       << std::dec << "\n";
    return os;
  }

} // namespace L1Topo
  
