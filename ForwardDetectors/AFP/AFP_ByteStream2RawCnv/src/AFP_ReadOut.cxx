/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_ByteStream2RawCnv/AFP_ReadOut.h"

#include <string>

AFP_ReadOut::AFP_ReadOut() : m_word(0) {}

AFP_ReadOut::~AFP_ReadOut() {}

uint32_t AFP_ReadOut::getBits(const uint16_t bstart, const uint16_t bstop) {
  uint32_t word = 0;
  for (uint16_t i = bstop; i < bstart; i++) {
    word = ((word | 1) << 1);
  }
  uint32_t result = (m_word >> bstop) & (word | 1);
  return result;
}
