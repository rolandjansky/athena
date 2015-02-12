/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CpmWord.h"
#include "WordDecoder.h"

using namespace LVL1BS;

namespace {
  WordDecoder cpmWordDecoder(
      {
        BitField("Tower1 Et", 0, 8),
        BitField("P1", 8, 1),
        BitField("Tower0 Et", 9, 8),
        BitField("P0", 17, 1),
        BitField("LinkDown", 19, 2),
        BitField("TTPair", 21, 2),
        BitField("Serializer", 23, 5)
     });
}


CpmWord::CpmWord(uint32_t word):
  m_word(word){
}


uint8_t CpmWord::tower1Et() const {
  return ::cpmWordDecoder.get<uint8_t>(m_word, 0); }

uint8_t CpmWord::p1() const {
  return ::cpmWordDecoder.get<uint8_t>(m_word, 1);
}
uint8_t CpmWord::tower0Et() const {
  return ::cpmWordDecoder.get<uint8_t>(m_word,2);
}
uint8_t CpmWord::p0() const {
  return ::cpmWordDecoder.get<uint8_t>(m_word, 3);
}
uint8_t CpmWord::linkDown() const {
  return ::cpmWordDecoder.get<uint8_t>(m_word, 4);
}
uint8_t CpmWord::ttPair() const {
  return ::cpmWordDecoder.get<uint8_t>(m_word, 5);
}
uint8_t CpmWord::serialiser() const {
  return ::cpmWordDecoder.get<uint8_t>(m_word, 6);
}


bool CpmWord::isValid() const {
  return BitField::mask(m_word, 28, 0xc) != 0;
}

bool CpmWord::isValid(uint32_t word) {
  return CpmWord(word).isValid();
}
