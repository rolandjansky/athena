/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_ByteStream2RawCnv/AFP_WordReadOut.h"

uint32_t AFP_WordReadOut::getBits(const uint16_t start, const uint16_t stop) const
{
  uint32_t word = 0;
  for (uint16_t i = stop; i < start; i++) {
    word = ((word | 1) << 1);
  }
  uint32_t result = (m_word >> stop) & (word | 1);
  return result;
}
