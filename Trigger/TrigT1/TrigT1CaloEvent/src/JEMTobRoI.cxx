/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/JEMTobRoI.h"

namespace LVL1 {

// Static constants

const int JEMTobRoI::s_wordIdVal;

const int JEMTobRoI::s_wordIdBit;
const int JEMTobRoI::s_crateBit;
const int JEMTobRoI::s_jemBit;
const int JEMTobRoI::s_frameBit;
const int JEMTobRoI::s_locationBit;
const int JEMTobRoI::s_energySmallBit;
const int JEMTobRoI::s_energyLargeBit;

const int JEMTobRoI::s_wordIdMask;
const int JEMTobRoI::s_crateMask;
const int JEMTobRoI::s_jemMask;
const int JEMTobRoI::s_frameMask;
const int JEMTobRoI::s_locationMask;
const int JEMTobRoI::s_energySmallMask;
const int JEMTobRoI::s_energyLargeMask;

JEMTobRoI::JEMTobRoI() : m_roiWord(0)
{
}

JEMTobRoI::JEMTobRoI(uint32_t roiWord) : m_roiWord(roiWord)
{
}

JEMTobRoI::JEMTobRoI(int crate, int jem, int frame, int location,
                                int energyLarge, int energySmall)
{
  m_roiWord  =  s_wordIdVal                      << s_wordIdBit;
  m_roiWord |= (crate       & s_crateMask)       << s_crateBit;
  m_roiWord |= (jem         & s_jemMask)         << s_jemBit;
  m_roiWord |= (frame       & s_frameMask)       << s_frameBit;
  m_roiWord |= (location    & s_locationMask)    << s_locationBit;
  m_roiWord |= (energySmall & s_energySmallMask) << s_energySmallBit;
  m_roiWord |= (energyLarge & s_energyLargeMask) << s_energyLargeBit;
}

JEMTobRoI::~JEMTobRoI()
{
}

// Set RoI word with ID check

bool JEMTobRoI::setRoiWord(uint32_t roiWord)
{
  int wordId = (roiWord >> s_wordIdBit) & s_wordIdMask;
  bool valid = (wordId == s_wordIdVal);
  if (valid) m_roiWord = roiWord;
  return valid;
}

} // end namespace
