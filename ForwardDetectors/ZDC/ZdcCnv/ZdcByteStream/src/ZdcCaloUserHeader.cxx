/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcByteStream/ZdcCaloUserHeader.h"
#include "ZdcByteStream/ZdcWordDecoder.h"

//using namespace LVL1BS;

namespace {
  const ZdcWordDecoder caloUserWordDecoder({
    BitField("Length", 0, 4),
    BitField("L1A Slice: PP FADC", 4, 5),
    BitField("LUT", 9, 3),
    BitField("CP", 12, 4),
    BitField("JEP", 16, 4),
    BitField("PP Lower Bound", 20, 8),
    BitField("Type", 28, 4)
 });
}


ZdcCaloUserHeader::ZdcCaloUserHeader(uint32_t header):
  m_header(header){
}


uint8_t ZdcCaloUserHeader::length() const {
  return ::caloUserWordDecoder.get<uint8_t>(m_header, 0);
}
uint8_t ZdcCaloUserHeader::ppFadc() const  {
  return ::caloUserWordDecoder.get<uint8_t>(m_header, 1);
}
uint8_t ZdcCaloUserHeader::lut()  const {
  return ::caloUserWordDecoder.get<uint8_t>(m_header,2);
}
uint8_t ZdcCaloUserHeader::cp()  const {
  return ::caloUserWordDecoder.get<uint8_t>(m_header, 3);
}
uint8_t ZdcCaloUserHeader::jep() const  {
  return ::caloUserWordDecoder.get<uint8_t>(m_header, 4);
}
uint8_t ZdcCaloUserHeader::ppLowerBound() const  {
  return ::caloUserWordDecoder.get<uint8_t>(m_header, 5);
}
bool ZdcCaloUserHeader::isValid() const {
  return ::caloUserWordDecoder.get<uint8_t>(m_header, 6) == 0xf;
}
bool ZdcCaloUserHeader::isValid(uint32_t word) {
  return ZdcCaloUserHeader(word).isValid();
}

