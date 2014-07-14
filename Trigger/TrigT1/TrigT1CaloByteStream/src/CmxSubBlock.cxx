/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CmxSubBlock.h"

namespace LVL1BS {

// Static constant definitions

const int      CmxSubBlock::s_wordIdVal;

const int      CmxSubBlock::s_cmxSummingBit;
const int      CmxSubBlock::s_cmxFirmwareBit;
const int      CmxSubBlock::s_cmxPositionBit;
const uint32_t CmxSubBlock::s_cmxSummingMask;
const uint32_t CmxSubBlock::s_cmxFirmwareMask;
const uint32_t CmxSubBlock::s_cmxPositionMask;

const int      CmxSubBlock::s_glinkBitsPerSlice;

CmxSubBlock::CmxSubBlock()
{
}

CmxSubBlock::~CmxSubBlock()
{
}

// Store CMX header

void CmxSubBlock::setCmxHeader(const int version, const int format,
                               const int slice, const int crate,
                               const int summing, const int firmware,
			       const int position, const int timeslices)
{
  int module = 0;
  module |= (summing  & s_cmxSummingMask)  << s_cmxSummingBit;
  module |= (firmware & s_cmxFirmwareMask) << s_cmxFirmwareBit;
  module |= (position & s_cmxPositionMask) << s_cmxPositionBit;
  setHeader(s_wordIdVal, version, format, slice, crate, module, 0,
                                                           timeslices);
}

// Return number of timeslices

int CmxSubBlock::timeslices() const
{
  int slices = slices1();
  if (slices == 0 && format() == NEUTRAL) {
    slices = dataWords() / s_glinkBitsPerSlice;
  }
  if (slices == 0) slices = 1;
  return slices;
}

// Static function to determine CMX type

CmxSubBlock::CmxFirmwareCode CmxSubBlock::cmxType(const uint32_t word)
{
  CmxFirmwareCode type;
  const int module = L1CaloSubBlock::module(word);
  const int code   = (module >> s_cmxFirmwareBit) & s_cmxFirmwareMask;
  switch (code) {
    case CMX_CP:
      type = CMX_CP;
      break;
    case CMX_JET:
      type = CMX_JET;
      break;
    case CMX_ENERGY:
      type = CMX_ENERGY;
      break;
    default:
      type = CMX_UNKNOWN;
      break;
  }
  return type;
}

// Static function to determine if header word corresponds to CMX block

bool CmxSubBlock::cmxBlock(const uint32_t word)
{
  return L1CaloSubBlock::wordId(word) == s_wordIdVal;
}

} // end namespace
