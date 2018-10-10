/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CPMTobRoI.h"

namespace LVL1 {

// Static constants

const int CPMTobRoI::s_wordIdVal;

const int CPMTobRoI::s_wordIdBit;
const int CPMTobRoI::s_crateBit;
const int CPMTobRoI::s_cpmBit;
const int CPMTobRoI::s_chipBit;
const int CPMTobRoI::s_locationBit;
const int CPMTobRoI::s_isolBit;
const int CPMTobRoI::s_energyBit;

const int CPMTobRoI::s_wordIdMask;
const int CPMTobRoI::s_crateMask;
const int CPMTobRoI::s_cpmMask;
const int CPMTobRoI::s_chipMask;
const int CPMTobRoI::s_locationMask;
const int CPMTobRoI::s_isolMask;
const int CPMTobRoI::s_energyMask;
const int CPMTobRoI::s_typeMask;

CPMTobRoI::CPMTobRoI() : m_roiWord(0)
{
}

CPMTobRoI::CPMTobRoI(uint32_t roiWord) : m_roiWord(roiWord)
{
}

CPMTobRoI::CPMTobRoI(int crate, int cpm, int chip, int location, int type,
                                             int energy, int isol)
{
  m_roiWord  =  s_wordIdVal                    << s_wordIdBit;
  m_roiWord |= (type       & s_typeMask)       << s_wordIdBit;
  m_roiWord |= (crate      & s_crateMask)      << s_crateBit;
  m_roiWord |= (cpm        & s_cpmMask)        << s_cpmBit;
  m_roiWord |= (chip       & s_chipMask)       << s_chipBit;
  m_roiWord |= (location   & s_locationMask)   << s_locationBit;
  m_roiWord |= (isol       & s_isolMask)       << s_isolBit;
  m_roiWord |= (energy     & s_energyMask)     << s_energyBit;
}

CPMTobRoI::~CPMTobRoI()
{
}

// Set RoI word with ID check

bool CPMTobRoI::setRoiWord(uint32_t roiWord)
{
  int wordId = (roiWord >> s_wordIdBit) & s_wordIdMask;
  bool valid = ((wordId == s_wordIdVal) ||
                (wordId == (s_wordIdVal|s_typeMask)));
  if (valid) m_roiWord = roiWord;
  return valid;
}

} // end namespace
