/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/BlockTypes.h"
#include "L1TopoRDO/Helpers.h"
//#include <cassert>

namespace L1Topo {

  Header::Header(const uint32_t version, const uint32_t active_fibres, const uint32_t payload_crc, const uint32_t fpga, const uint32_t last_block, const uint32_t bcn_sign, const uint32_t bcn_offset)
    :m_version(version), m_active_fibres(active_fibres), m_payload_crc(payload_crc), m_fpga(fpga),
     m_last_block(last_block), m_bcn_sign(bcn_sign), m_bcn_offset(bcn_offset), m_bcn(0), m_word(0) {
    this->encode();
    m_bcn = signedBCN(m_bcn_sign, m_bcn_offset);
  }

  Header::Header(const uint32_t word)
    : m_word(word){
    this->decode();
    m_bcn = signedBCN(m_bcn_sign, m_bcn_offset);
  }

  /* See https://twiki.cern.ch/twiki/pub/Atlas/L1CaloUpgrade/ROD_data_format_v1.0.9.xlsx */

  void Header::decode(){
    // assert (L1Topo::blockType(m_word)==L1Topo::BlockTypes::HEADER);
    m_bcn_offset = m_word & 0x7;
    m_bcn_sign = (m_word>>3) & 0x1;
    m_last_block = (m_word>>4) & 0x1;
    m_fpga = (m_word >> 5 ) & 0x1;
    m_payload_crc = (m_word>>8) & 0xff;
    m_active_fibres = (m_word>>16) & 0xff;
    m_version = (m_word>>24) & 0xf;
    return;
  }

  void Header::encode(){
    m_word = static_cast<int>(L1Topo::BlockTypes::HEADER) << 28;
    m_word |= (m_bcn_offset & 0x7);
    m_word |= (m_bcn_sign & 0x1) << 3;
    m_word |= (m_last_block & 0x1) << 4;
    m_word |= (m_fpga & 0x1) << 5;
    m_word |= (m_payload_crc & 0xff) << 8;
    m_word |= (m_active_fibres & 0xff) << 16;
    m_word |= (m_version & 0xf) << 24;
    return;
  }

  uint32_t Header::version() const{
    return m_version;
  }
  uint32_t Header::active_fibres() const{
    return m_active_fibres;
  }
  uint32_t Header::payload_crc() const{
    return m_payload_crc;
  }
  uint32_t Header::fpga() const{
    return m_fpga;
  }
  uint32_t Header::last_block() const{
    return m_last_block;
  }
  uint32_t Header::bcn_sign() const{
    return m_bcn_sign;
  }
  uint32_t Header::bcn_offset() const{
    return m_bcn_offset;
  }
  int Header::bcn() const{
    return m_bcn;
  }
  uint32_t Header::word() const{
    return m_word;
  }
  
  int signedBCN(const uint32_t sign, uint32_t offset){
    return offset * (sign?-1:1); // sign=0 is false
  }

  std::pair<uint32_t,uint32_t> signAndOffsetBCN(const int bcn){
    uint32_t offset = std::abs(bcn);
    uint32_t sign = (bcn<0)?1:0; // negative bcn encoded as 1
    return std::pair<uint32_t,uint32_t>(sign,offset);
  }

  std::ostream& operator<<(std::ostream& os, const Header& h) {
    os << "     Block header:" 
       << " version " << h.version() 
       << std::hex
       << " active_fibres 0x" << h.active_fibres()
       << " payload_crc 0x" << h.payload_crc()
       << std::dec
       << " fpga " << h.fpga()
       << " relative bcn " << h.bcn()
       << " last_block " << h.last_block();
    return os;
  }

} // namespace L1Topo

