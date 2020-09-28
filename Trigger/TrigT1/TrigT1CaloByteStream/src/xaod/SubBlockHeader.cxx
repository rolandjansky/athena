/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SubBlockHeader.h"

#include "WordDecoder.h"

using namespace LVL1BS;

namespace {
  const WordDecoder subBlockHeaderDecoder(
      {
        BitField("Type", 28, 4),
        BitField("Version", 25, 3),
        BitField("Format", 22, 3),
        BitField("SeqNum", 16, 6),
        BitField("Crate", 12, 4),
        BitField("Module", 8, 4),
        BitField("nSlice2", 3, 5),
        BitField("nSlice1", 0, 3)
     });
}



SubBlockHeader::SubBlockHeader(uint32_t header):
  m_header(header){
}


uint8_t SubBlockHeader::type() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 0); }

uint8_t SubBlockHeader::version() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 1);
}
uint8_t SubBlockHeader::format() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header,2);
}
uint8_t SubBlockHeader::seqNum() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 3);
}
uint8_t SubBlockHeader::crate() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 4);
}
uint8_t SubBlockHeader::module() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 5);
}
uint8_t SubBlockHeader::nSlice2() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 6);
}
uint8_t SubBlockHeader::nSlice1() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 7);
}

bool SubBlockHeader::isSubBlockHeader() const {
  return (type() & 0xc) == 0xc;
}

bool SubBlockHeader::isSubBlockHeader(uint32_t word) {
  return SubBlockHeader(word).isSubBlockHeader();
}

bool SubBlockHeader::isPpmBlock() const {
  return (type() & 0xe) == 0xc;
}


