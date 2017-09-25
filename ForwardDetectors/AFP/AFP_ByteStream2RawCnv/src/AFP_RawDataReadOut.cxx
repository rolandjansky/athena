/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_ByteStream2RawCnv/AFP_RawDataReadOut.h"

AFP_RawDataReadOut::AFP_RawDataReadOut()
    : m_dataWord(0), m_wordHeader(0), m_SlotId(0), m_LsbTdcId(0), m_ecnt_BOT(0),
      m_ecnt_EOT(0), m_bcId(0), m_wcnt(0), m_Column(0), m_bit26_27(0),
      m_bit24_27(0), m_HitDiscConfig(0), m_Link(0), m_fiber(0), m_ADC(0),
      m_pattern(), m_BCId(0), m_bit16(0), m_error_bit17(0), m_bit18(0), ToT() {}

AFP_RawDataReadOut::~AFP_RawDataReadOut() {}

void AFP_RawDataReadOut::decodeWord(uint32_t dataWord) {

  setZero();
  m_dataWord = dataWord;
  m_wordHeader = (dataWord >> headerPos) & headerBits;

  if (is_TDCt()) {
    m_bit18 = (bool)getBits(18, 18);
    m_error_bit17 = (bool)getBits(17, 17);
    m_HitDiscConfig = getBits(25, 24);
    m_Link = getBits(23, 19);
    m_bit16 = (bool)getBits(16, 16);
    m_bit26_27 = getBits(27, 26);
    m_fiber = getBits(15, 0);
    m_ADC = getBits(11, 0);

    for (int i = 15; i > -1; i--)

    {
      if (getBits(i, i))
        m_pattern.push_back(true);
      else
        m_pattern.push_back(false);
    }
    m_BCId = getBits(15, 0);
    m_Column = getBits(7, 0);
  } else if (is_BOT()) {

    m_SlotId = getBits(28, 24);
    m_ecnt_BOT = getBits(23, 12);
    m_bcId = getBits(11, 0);
  }

  else if (is_EOT()) {
    m_LsbTdcId = getBits(25, 24);
    m_ecnt_EOT = getBits(23, 12);
    m_wcnt = getBits(11, 0);
    m_bit24_27 = getBits(27, 24);
  }
}

void AFP_RawDataReadOut::setZero() {
  m_SlotId = 0;
  m_LsbTdcId = 0;
  m_ecnt_BOT = 0;
  m_ecnt_EOT = 0;
  m_bcId = 0;
  m_wcnt = 0;
  m_HitDiscConfig = 0;
  m_Link = 0;
  m_fiber = 0;
  m_ADC = 0;
  m_pattern.clear();
  m_BCId = 0;
  m_bit16 = false;
  m_error_bit17 = false;
  m_bit18 = false;
  m_Column = 0;
  m_bit26_27 = 0;
  m_bit24_27 = 0;
}
