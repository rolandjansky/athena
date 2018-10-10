/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/JEMRoI.h"

namespace LVL1 {

// Static constants

const int JEMRoI::s_wordIdVal;

const int JEMRoI::s_wordIdBit;
const int JEMRoI::s_crateBit;
const int JEMRoI::s_jemBit;
const int JEMRoI::s_frameBit;
const int JEMRoI::s_locationBit;
const int JEMRoI::s_forwardBit;
const int JEMRoI::s_parityBit;
const int JEMRoI::s_saturationBit;
const int JEMRoI::s_forwardHitsBit;
const int JEMRoI::s_mainHitsBit;

const int JEMRoI::s_wordIdMask;
const int JEMRoI::s_crateMask;
const int JEMRoI::s_jemMask;
const int JEMRoI::s_frameMask;
const int JEMRoI::s_locationMask;
const int JEMRoI::s_forwardMask;
const int JEMRoI::s_parityMask;
const int JEMRoI::s_saturationMask;
const int JEMRoI::s_forwardHitsMask;
const int JEMRoI::s_mainHitsMask;

JEMRoI::JEMRoI() : m_roiWord(0)
{
}

JEMRoI::JEMRoI(uint32_t roiWord) : m_roiWord(roiWord)
{
}

JEMRoI::JEMRoI(int crate, int jem, int frame, int location, int forward,
               int hits, int error)
{
  m_roiWord  =  s_wordIdVal                    << s_wordIdBit;
  m_roiWord |= (crate      & s_crateMask)      << s_crateBit;
  m_roiWord |= (jem        & s_jemMask)        << s_jemBit;
  m_roiWord |= (frame      & s_frameMask)      << s_frameBit;
  m_roiWord |= (location   & s_locationMask)   << s_locationBit;
  m_roiWord |= (forward    & s_forwardMask)    << s_forwardBit;
  m_roiWord |= ((error>>1) & s_parityMask)     << s_parityBit;
  m_roiWord |= (error      & s_saturationMask) << s_saturationBit;
  if (forward) m_roiWord |= (hits & s_forwardHitsMask) << s_forwardHitsBit;
  else         m_roiWord |= (hits & s_mainHitsMask)    << s_mainHitsBit;
}

JEMRoI::~JEMRoI()
{
}

// Return Jet hit map (8 bits Main or 4 bits Forward)

int JEMRoI::hits() const
{
  int hits = 0;
  if (forward()) hits = (m_roiWord >> s_forwardHitsBit) & s_forwardHitsMask;
  else           hits = (m_roiWord >> s_mainHitsBit)    & s_mainHitsMask;
  return hits;
}

// Set RoI word with ID check

bool JEMRoI::setRoiWord(uint32_t roiWord)
{
  int wordId = (roiWord >> s_wordIdBit) & s_wordIdMask;
  bool valid = (wordId == s_wordIdVal);
  if (valid) m_roiWord = roiWord;
  return valid;
}

} // end namespace
