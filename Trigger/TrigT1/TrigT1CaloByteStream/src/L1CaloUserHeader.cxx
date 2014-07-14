/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "L1CaloUserHeader.h"

namespace LVL1BS {

// Static constant definitions

const int      L1CaloUserHeader::s_jepCmmBit;
//const int      L1CaloUserHeader::s_cpCmmBit;
const int      L1CaloUserHeader::s_jemBit;
const int      L1CaloUserHeader::s_cpmBit;
const int      L1CaloUserHeader::s_ppmLutBit;
const int      L1CaloUserHeader::s_ppmFadcBit;
const int      L1CaloUserHeader::s_lowerBoundBit;
const int      L1CaloUserHeader::s_ppmLutBitV2;
const int      L1CaloUserHeader::s_ppmFadcBitV2;
const uint32_t L1CaloUserHeader::s_mask;
const uint32_t L1CaloUserHeader::s_lowerBoundMask;
const uint32_t L1CaloUserHeader::s_ppmLutMaskV2;
const uint32_t L1CaloUserHeader::s_ppmFadcMaskV2;
const int      L1CaloUserHeader::s_version1;

// Constructor

L1CaloUserHeader::L1CaloUserHeader(uint32_t header) : m_header(header),
                                                      m_version2(true)
{
}

// Test for valid header word

bool L1CaloUserHeader::isValid(const uint32_t word)
{
  return ((word >> s_wordIdBit) & s_mask) == s_mask;
}

} // end namespace
