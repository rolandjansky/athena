/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  ZdcUserHeader
 *
 *  This should be called ZdcRodHeader..
 *
 *  TRY_LINK_LVL1
 *
 *  Marco Leite
 *
 */
#include "ZdcByteStream/ZdcUserHeader.h"

// Static constant definitions

//const int      ZdcUserHeader::s_jepCmmBit;
//const int      ZdcUserHeader::s_cpCmmBit;
//const int      ZdcUserHeader::s_jemBit;
//const int      ZdcUserHeader::s_cpmBit;
const int      ZdcUserHeader::s_ppmLutBit;
const int      ZdcUserHeader::s_ppmFadcBit;
const int      ZdcUserHeader::s_lowerBoundBit;
const int      ZdcUserHeader::s_ppmLutBitV2;
const int      ZdcUserHeader::s_ppmFadcBitV2;
const uint32_t ZdcUserHeader::s_mask;
const uint32_t ZdcUserHeader::s_lowerBoundMask;
const uint32_t ZdcUserHeader::s_ppmLutMaskV2;
const uint32_t ZdcUserHeader::s_ppmFadcMaskV2;
const int      ZdcUserHeader::s_version1;

// Constructor

ZdcUserHeader::ZdcUserHeader(uint32_t header) : m_header(header),
                                                      m_version2(true)
{
}

// Test for valid header word

bool ZdcUserHeader::isValid(const uint32_t word)
{
  return ((word >> s_wordIdBit) & s_mask) == s_mask;
}

