/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcByteStream/ZdcSubBlockHeader.h"

#include "ZdcByteStream/ZdcWordDecoder.h"

//using namespace LVL1BS;

namespace {
  const ZdcWordDecoder subBlockHeaderDecoder(
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

ZdcSubBlockHeader::ZdcSubBlockHeader(uint32_t header):
  m_header(header){
}


uint8_t ZdcSubBlockHeader::type() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 0); }

uint8_t ZdcSubBlockHeader::version() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 1);
}
uint8_t ZdcSubBlockHeader::format() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header,2);
}
uint8_t ZdcSubBlockHeader::seqNum() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 3);
}
uint8_t ZdcSubBlockHeader::crate() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 4);
}
uint8_t ZdcSubBlockHeader::module() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 5);
}
uint8_t ZdcSubBlockHeader::nSlice2() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 6);
}
uint8_t ZdcSubBlockHeader::nSlice1() const {
  return ::subBlockHeaderDecoder.get<uint8_t>(m_header, 7);
}

bool ZdcSubBlockHeader::isSubBlockHeader() const {
  return (type() & 0xc) == 0xc;
}

bool ZdcSubBlockHeader::isSubBlockHeader(uint32_t word) {
  return ZdcSubBlockHeader(word).isSubBlockHeader();
}

bool ZdcSubBlockHeader::isPpmBlock() const {
  return (type() & 0xe) == 0xc;
}


