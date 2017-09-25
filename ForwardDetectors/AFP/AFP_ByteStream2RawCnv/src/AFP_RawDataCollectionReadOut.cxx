/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_ByteStream2RawCnv/AFP_RawDataCollectionReadOut.h"

AFP_RawDataCollectionReadOut::AFP_RawDataCollectionReadOut()
    :

      m_dataWord(0),
      m_wordHeader(0), m_wordHeader2(0), m_lvl1Id(0), m_link(0), m_flag(0),
      m_ToT2(0), m_bcid(0), m_DiscConf(0), m_row(0), m_column(0), m_ToT1(0) {}

AFP_RawDataCollectionReadOut::~AFP_RawDataCollectionReadOut() {}

void AFP_RawDataCollectionReadOut::decodeWord(uint32_t dataWord)

{
  setZero();

  m_dataWord = dataWord;
  m_word = dataWord;
  m_wordHeader = getBits(31, 28);
  m_wordHeader2 = getBits(31, 30);

  if (is_BOB()) {

    // https://indico.cern.ch/event/493415/contributions/1172054/attachments/1222236/1787442/bytestream.pdf
    // using the data format defined in the slides:

    m_lvl1Id = getBits(14, 10); // L1id
    m_link = getBits(27, 24);   // link number
    m_flag = getBits(15, 15);   // frontend flag, no idea what this is
    m_bcid = getBits(9, 0);     // BCID
  }

  else if (is_LWC()) {
    m_DiscConf = getBits(29, 28); // HitDiscConfig
    m_link = getBits(27, 24);     // link number
    m_row = getBits(23, 17);      // row
    m_column = getBits(16, 8);    // column
    m_ToT1 = getBits(7, 4);       // ToT1
    m_ToT2 = getBits(3, 0);       // ToT2
  }
}

void AFP_RawDataCollectionReadOut::setZero() {
  m_lvl1Id = 0;
  m_link = 0;
  m_flag = 0;
  m_ToT1 = 0;
  m_DiscConf = 0;
  m_ToT2 = 0;
  m_bcid = 0;
  m_row = 0;
  m_column = 0;
}
