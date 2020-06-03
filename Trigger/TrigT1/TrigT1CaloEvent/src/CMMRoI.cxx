/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMMRoI.h"

namespace LVL1 {

// Static constants

const int CMMRoI::s_jetEtWordIdVal;
const int CMMRoI::s_wordIdVal0;
const int CMMRoI::s_wordIdVal1;
const int CMMRoI::s_wordIdVal2;

const int CMMRoI::s_jetEtWordIdBit;
const int CMMRoI::s_wordIdBit;
const int CMMRoI::s_jetEtHitsBit;
const int CMMRoI::s_sumEtHitsBit;
const int CMMRoI::s_missingEtHitsBit;
const int CMMRoI::s_missingEtSigHitsBit;
const int CMMRoI::s_energyBit;
const int CMMRoI::s_jetEtParityBit;
const int CMMRoI::s_parityBit;
const int CMMRoI::s_overflowBit;

const int CMMRoI::s_jetEtWordIdMask;
const int CMMRoI::s_wordIdMask;
const int CMMRoI::s_jetEtHitsMask;
const int CMMRoI::s_sumEtHitsMask;
const int CMMRoI::s_missingEtHitsMask;
const int CMMRoI::s_missingEtSigHitsMask;
const int CMMRoI::s_energyMask;

CMMRoI::CMMRoI() : m_jetEtRoiWord(0),
                   m_energyRoiWord0(0),
		   m_energyRoiWord1(0),
		   m_energyRoiWord2(0)
{
}

CMMRoI::CMMRoI(uint32_t jetEtRoiWord,   uint32_t energyRoiWord0,
               uint32_t energyRoiWord1, uint32_t energyRoiWord2)
	       : m_jetEtRoiWord(jetEtRoiWord),
	         m_energyRoiWord0(energyRoiWord0),
	         m_energyRoiWord1(energyRoiWord1),
		 m_energyRoiWord2(energyRoiWord2)
{
}

CMMRoI::CMMRoI(int jetEtHits, int sumEtHits, int missingEtHits,
               int missingEtSigHits, int ex, int ey, int et,
	       int jetEtError, int sumEtError, int missingEtError,
	       int missingEtSigError, int exError, int eyError, int etError)
{
  m_jetEtRoiWord  =  s_jetEtWordIdVal                   << s_jetEtWordIdBit;
  m_jetEtRoiWord |= (jetEtHits       & s_jetEtHitsMask) << s_jetEtHitsBit;
  m_jetEtRoiWord |= ((jetEtError>>1) & 0x1)             << s_jetEtParityBit;

  m_energyRoiWord0  =  s_wordIdVal0                 << s_wordIdBit;
  m_energyRoiWord0 |= (ex & s_energyMask)           << s_energyBit;
  m_energyRoiWord0 |= (exError      & 0x1)          << s_overflowBit;
  m_energyRoiWord0 |= ((exError>>1) & 0x1)          << s_parityBit;
  m_energyRoiWord0 |= (missingEtSigHits & s_missingEtSigHitsMask)
                                                    << s_missingEtSigHitsBit;
  m_energyRoiWord0 |= ((missingEtSigError>>1) & 0x1)<< s_parityBit;

  m_energyRoiWord1  =  s_wordIdVal1                 << s_wordIdBit;
  m_energyRoiWord1 |= (ey & s_energyMask)           << s_energyBit;
  m_energyRoiWord1 |= (eyError      & 0x1)          << s_overflowBit;
  m_energyRoiWord1 |= ((eyError>>1) & 0x1)          << s_parityBit;
  m_energyRoiWord1 |= (sumEtHits & s_sumEtHitsMask) << s_sumEtHitsBit;
  m_energyRoiWord1 |= ((sumEtError>>1) & 0x1)       << s_parityBit;

  m_energyRoiWord2  =  s_wordIdVal2                 << s_wordIdBit;
  m_energyRoiWord2 |= (et & s_energyMask)           << s_energyBit;
  m_energyRoiWord2 |= (etError      & 0x1)          << s_overflowBit;
  m_energyRoiWord2 |= ((etError>>1) & 0x1)          << s_parityBit;
  m_energyRoiWord2 |= (missingEtHits & s_missingEtHitsMask)
                                                    << s_missingEtHitsBit;
  m_energyRoiWord2 |= ((missingEtError>>1) & 0x1)   << s_parityBit;
}

CMMRoI::~CMMRoI()
{
}

// Set RoI word with ID check

bool CMMRoI::setRoiWord(uint32_t roiWord)
{
  bool valid = true;
  int wordId = (roiWord >> s_jetEtWordIdBit) & s_jetEtWordIdMask;
  if (wordId == s_jetEtWordIdVal) m_jetEtRoiWord = roiWord;
  else {
    wordId = (roiWord >> s_wordIdBit) & s_wordIdMask;
    if      (wordId == s_wordIdVal0) m_energyRoiWord0 = roiWord;
    else if (wordId == s_wordIdVal1) m_energyRoiWord1 = roiWord;
    else if (wordId == s_wordIdVal2) m_energyRoiWord2 = roiWord;
    else valid = false;
  }
  return valid;
}

} // end namespace
