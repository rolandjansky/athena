/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CmmSubBlock.h"

namespace LVL1BS {

// Static constant definitions

const int      CmmSubBlock::s_wordIdVal;

const int      CmmSubBlock::s_cmmSummingBit;
const int      CmmSubBlock::s_cmmFirmwareBit;
const int      CmmSubBlock::s_cmmPositionBit;
const uint32_t CmmSubBlock::s_cmmSummingMask;
const uint32_t CmmSubBlock::s_cmmFirmwareMask;
const uint32_t CmmSubBlock::s_cmmPositionMask;

const int      CmmSubBlock::s_glinkBitsPerSlice;

CmmSubBlock::CmmSubBlock()
{
}

CmmSubBlock::~CmmSubBlock()
{
}

// Store CMM header

void CmmSubBlock::setCmmHeader(const int version, const int format,
                               const int slice, const int crate,
                               const int summing, const int firmware,
			       const int position, const int timeslices)
{
  int module = 0;
  module |= (summing  & s_cmmSummingMask)  << s_cmmSummingBit;
  module |= (firmware & s_cmmFirmwareMask) << s_cmmFirmwareBit;
  module |= (position & s_cmmPositionMask) << s_cmmPositionBit;
  setHeader(s_wordIdVal, version, format, slice, crate, module, 0,
                                                           timeslices);
}

// Return number of timeslices

int CmmSubBlock::timeslices() const
{
  int slices = slices1();
  if (slices == 0 && format() == NEUTRAL) {
    slices = dataWords() / s_glinkBitsPerSlice;
  }
  if (slices == 0) slices = 1;
  return slices;
}

// Static function to determine CMM type

CmmSubBlock::CmmFirmwareCode CmmSubBlock::cmmType(const uint32_t word)
{
  CmmFirmwareCode type;
  const int module = L1CaloSubBlock::module(word);
  const int code   = (module >> s_cmmFirmwareBit) & s_cmmFirmwareMask;
  switch (code) {
    case CMM_CP:
      type = CMM_CP;
      break;
    case CMM_JET:
      type = CMM_JET;
      break;
    case CMM_ENERGY:
      type = CMM_ENERGY;
      break;
    default:
      type = CMM_UNKNOWN;
      break;
  }
  return type;
}

// Static function to determine if header word corresponds to CMM block

bool CmmSubBlock::cmmBlock(const uint32_t word)
{
  return L1CaloSubBlock::wordId(word) == s_wordIdVal;
}

} // end namespace
