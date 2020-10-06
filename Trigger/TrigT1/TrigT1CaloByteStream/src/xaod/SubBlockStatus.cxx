/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SubBlockStatus.h"

#include "WordDecoder.h"

using namespace LVL1BS;

namespace {
  const WordDecoder subBlockStatusDecoder(
      {
        BitField("Timeout", 7, 1),
        BitField("Link", 6, 1),
        BitField("Specific", 4, 1),
        BitField("Fifo", 3, 1),
        BitField("BCN", 2, 1),
        BitField("Protocol", 1, 1),
        BitField("Parity", 0, 1),
        BitField("BCLowBits", 22, 6)
     });
}



SubBlockStatus::SubBlockStatus(uint32_t status):
  m_status(status){
}

uint8_t SubBlockStatus::timeout() const {
  return ::subBlockStatusDecoder.get<uint8_t>(m_status, 0); }

uint8_t SubBlockStatus::link() const {
  return ::subBlockStatusDecoder.get<uint8_t>(m_status, 1);
}
uint8_t SubBlockStatus::specific() const {
  return ::subBlockStatusDecoder.get<uint8_t>(m_status,2);
}
uint8_t SubBlockStatus::fifo() const {
  return ::subBlockStatusDecoder.get<uint8_t>(m_status, 3);
}
uint8_t SubBlockStatus::bcn() const {
  return ::subBlockStatusDecoder.get<uint8_t>(m_status, 4);
}
uint8_t SubBlockStatus::protocol() const {
  return ::subBlockStatusDecoder.get<uint8_t>(m_status, 5);
}
uint8_t SubBlockStatus::parity() const {
  return ::subBlockStatusDecoder.get<uint8_t>(m_status, 6);
}
uint8_t SubBlockStatus::bcLowBits() const {
  return ::subBlockStatusDecoder.get<uint8_t>(m_status, 7);
}
