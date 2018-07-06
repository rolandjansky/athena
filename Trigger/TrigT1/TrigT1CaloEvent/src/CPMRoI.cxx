/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CPMRoI.h"

namespace LVL1 {

// Static constants

const int CPMRoI::s_wordIdVal;

const int CPMRoI::s_wordIdBit;
const int CPMRoI::s_crateBit;
const int CPMRoI::s_cpmBit;
const int CPMRoI::s_chipBit;
const int CPMRoI::s_locationBit;
const int CPMRoI::s_parityBit;
const int CPMRoI::s_saturationBit;
const int CPMRoI::s_hitsBit;

const int CPMRoI::s_wordIdMask;
const int CPMRoI::s_crateMask;
const int CPMRoI::s_cpmMask;
const int CPMRoI::s_chipMask;
const int CPMRoI::s_locationMask;
const int CPMRoI::s_parityMask;
const int CPMRoI::s_saturationMask;
const int CPMRoI::s_hitsMask;

CPMRoI::CPMRoI() : m_roiWord(0)
{
}

CPMRoI::CPMRoI(uint32_t roiWord) : m_roiWord(roiWord)
{
}

CPMRoI::CPMRoI(int crate, int cpm, int chip, int location, int hits, int error)
{
  m_roiWord  =  s_wordIdVal                    << s_wordIdBit;
  m_roiWord |= (crate      & s_crateMask)      << s_crateBit;
  m_roiWord |= (cpm        & s_cpmMask)        << s_cpmBit;
  m_roiWord |= (chip       & s_chipMask)       << s_chipBit;
  m_roiWord |= (location   & s_locationMask)   << s_locationBit;
  m_roiWord |= ((error>>1) & s_parityMask)     << s_parityBit;
  m_roiWord |= (error      & s_saturationMask) << s_saturationBit;
  m_roiWord |= (hits       & s_hitsMask)       << s_hitsBit;
}

CPMRoI::~CPMRoI()
{
}

// Set RoI word with ID check

bool CPMRoI::setRoiWord(uint32_t roiWord)
{
  int wordId = (roiWord >> s_wordIdBit) & s_wordIdMask;
  bool valid = (wordId == s_wordIdVal);
  if (valid) m_roiWord = roiWord;
  return valid;
}

} // end namespace
