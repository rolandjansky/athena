/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMXRoI.h"

namespace LVL1 {

// Static constants

const int CMXRoI::s_wordIdVal0;
const int CMXRoI::s_wordIdVal1;
const int CMXRoI::s_wordIdVal2;

const int CMXRoI::s_wordIdBit;
const int CMXRoI::s_sumEtHitsBit;
const int CMXRoI::s_missingEtHitsBit;
const int CMXRoI::s_missingEtSigHitsBit;
const int CMXRoI::s_energyBit;
const int CMXRoI::s_overflowBit;
const int CMXRoI::s_sumTypeBit;

const int CMXRoI::s_wordIdMask;
const int CMXRoI::s_sumEtHitsMask;
const int CMXRoI::s_missingEtHitsMask;
const int CMXRoI::s_missingEtSigHitsMask;
const int CMXRoI::s_energyMask;
const int CMXRoI::s_overflowMask;
const int CMXRoI::s_sumTypeMask;

const int CMXRoI::s_exOffset;
const int CMXRoI::s_eyOffset;
const int CMXRoI::s_etOffset;
const int CMXRoI::s_maxRoiWords;

CMXRoI::CMXRoI() : m_roiWords(s_maxRoiWords)
{
}

CMXRoI::CMXRoI(uint32_t roiWord0, uint32_t roiWord1, uint32_t roiWord2,
               uint32_t roiWord3, uint32_t roiWord4, uint32_t roiWord5)
	       : m_roiWords(s_maxRoiWords)
{
  m_roiWords[0] = roiWord0;
  m_roiWords[1] = roiWord1;
  m_roiWords[2] = roiWord2;
  m_roiWords[3] = roiWord3;
  m_roiWords[4] = roiWord4;
  m_roiWords[5] = roiWord5;
}

CMXRoI::CMXRoI(unsigned int ex, unsigned int ey, unsigned int et,
               int exError, int eyError, int etError,
               unsigned int sumEtHits, unsigned int missingEtHits,
	       unsigned int missingEtSigHits,
	       unsigned int exM, unsigned int eyM, unsigned int etM,
	       int exErrorM, int eyErrorM, int etErrorM,
	       unsigned int sumEtHitsM, unsigned int missingEtHitsM)
	       : m_roiWords(s_maxRoiWords)
{
  int type = NORMAL;
  int ix   = s_exOffset + type;
  m_roiWords[ix]  =  s_wordIdVal0                  << s_wordIdBit;
  m_roiWords[ix] |=  type                          << s_sumTypeBit;
  m_roiWords[ix] |= (ex & s_energyMask)            << s_energyBit;
  m_roiWords[ix] |= (exError & s_overflowMask)     << s_overflowBit;
  m_roiWords[ix] |= (missingEtSigHits & s_missingEtSigHitsMask)
                                                   << s_missingEtSigHitsBit;
  type = MASKED;
  ix   = s_exOffset + type;
  m_roiWords[ix]  =  s_wordIdVal0                  << s_wordIdBit;
  m_roiWords[ix] |=  type                          << s_sumTypeBit;
  m_roiWords[ix] |= (exM & s_energyMask)           << s_energyBit;
  m_roiWords[ix] |= (exErrorM & s_overflowMask)    << s_overflowBit;

  type = NORMAL;
  ix   = s_eyOffset + type;
  m_roiWords[ix]  =  s_wordIdVal1                  << s_wordIdBit;
  m_roiWords[ix] |=  type                          << s_sumTypeBit;
  m_roiWords[ix] |= (ey & s_energyMask)            << s_energyBit;
  m_roiWords[ix] |= (eyError & s_overflowMask)     << s_overflowBit;
  m_roiWords[ix] |= (missingEtHits & s_missingEtHitsMask)
                                                   << s_missingEtHitsBit;
  type = MASKED;
  ix   = s_eyOffset + type;
  m_roiWords[ix]  =  s_wordIdVal1                  << s_wordIdBit;
  m_roiWords[ix] |=  type                          << s_sumTypeBit;
  m_roiWords[ix] |= (eyM & s_energyMask)           << s_energyBit;
  m_roiWords[ix] |= (eyErrorM & s_overflowMask)    << s_overflowBit;
  m_roiWords[ix] |= (missingEtHitsM & s_missingEtHitsMask)
                                                   << s_missingEtHitsBit;
  type = NORMAL;
  ix   = s_etOffset + type;
  m_roiWords[ix]  =  s_wordIdVal2                  << s_wordIdBit;
  m_roiWords[ix] |=  type                          << s_sumTypeBit;
  m_roiWords[ix] |= (et & s_energyMask)            << s_energyBit;
  m_roiWords[ix] |= (etError & s_overflowMask)     << s_overflowBit;
  m_roiWords[ix] |= (sumEtHits & s_sumEtHitsMask)  << s_sumEtHitsBit;

  type = MASKED;
  ix   = s_etOffset + type;
  m_roiWords[ix]  =  s_wordIdVal2                  << s_wordIdBit;
  m_roiWords[ix] |=  type                          << s_sumTypeBit;
  m_roiWords[ix] |= (etM & s_energyMask)           << s_energyBit;
  m_roiWords[ix] |= (etErrorM & s_overflowMask)    << s_overflowBit;
  m_roiWords[ix] |= (sumEtHitsM & s_sumEtHitsMask) << s_sumEtHitsBit;
}

CMXRoI::~CMXRoI()
{
}

// Set RoI word with ID check

bool CMXRoI::setRoiWord(uint32_t roiWord)
{
  bool valid = true;
  int wordId = (roiWord >> s_wordIdBit)  & s_wordIdMask;
  int type   = (roiWord >> s_sumTypeBit) & s_sumTypeMask;
  if      (wordId == s_wordIdVal0) m_roiWords[s_exOffset+type] = roiWord;
  else if (wordId == s_wordIdVal1) m_roiWords[s_eyOffset+type] = roiWord;
  else if (wordId == s_wordIdVal2) m_roiWords[s_etOffset+type] = roiWord;
  else valid = false;
  return valid;
}

} // end namespace
