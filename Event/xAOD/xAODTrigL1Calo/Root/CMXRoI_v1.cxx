/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXRoI_v1.cxx 723918 2016-02-15 07:52:49Z amazurov $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMXRoI_v1.h"

namespace xAOD {

CMXRoI_v1::CMXRoI_v1() : SG::AuxElement() {}

AUXSTORE_OBJECT_SETTER_AND_GETTER(CMXRoI_v1, std::vector<uint32_t>, cmxRoIWords,
                                  setCmxRoIWords)

/// initialize
void CMXRoI_v1::initialize(uint32_t roiWord0, uint32_t roiWord1,
                           uint32_t roiWord2, uint32_t roiWord3,
                           uint32_t roiWord4, uint32_t roiWord5) {
  std::vector<uint32_t> myRoIWords(s_maxRoiWords);
  myRoIWords[0] = roiWord0;
  myRoIWords[1] = roiWord1;
  myRoIWords[2] = roiWord2;
  myRoIWords[3] = roiWord3;
  myRoIWords[4] = roiWord4;
  myRoIWords[5] = roiWord5;
  setCmxRoIWords(myRoIWords);
}

/// initialize
void CMXRoI_v1::initialize(unsigned int ex, unsigned int ey, unsigned int et,
                           int exError, int eyError, int etError,
                           unsigned int sumEtHits, unsigned int missingEtHits,
                           unsigned int missingEtSigHits, unsigned int exM,
                           unsigned int eyM, unsigned int etM, int exErrorM,
                           int eyErrorM, int etErrorM, unsigned int sumEtHitsM,
                           unsigned int missingEtHitsM) {
  std::vector<uint32_t> myRoIWords(s_maxRoiWords);

  int type = NORMAL;
  int ix = s_exOffset + type;
  myRoIWords[ix] = s_wordIdVal0 << s_wordIdBit;
  myRoIWords[ix] |= type << s_sumTypeBit;
  myRoIWords[ix] |= (ex & s_energyMask) << s_energyBit;
  myRoIWords[ix] |= (exError & s_overflowMask) << s_overflowBit;
  myRoIWords[ix] |= (missingEtSigHits & s_missingEtSigHitsMask)
                    << s_missingEtSigHitsBit;
  type = MASKED;
  ix = s_exOffset + type;
  myRoIWords[ix] = s_wordIdVal0 << s_wordIdBit;
  myRoIWords[ix] |= type << s_sumTypeBit;
  myRoIWords[ix] |= (exM & s_energyMask) << s_energyBit;
  myRoIWords[ix] |= (exErrorM & s_overflowMask) << s_overflowBit;

  type = NORMAL;
  ix = s_eyOffset + type;
  myRoIWords[ix] = s_wordIdVal1 << s_wordIdBit;
  myRoIWords[ix] |= type << s_sumTypeBit;
  myRoIWords[ix] |= (ey & s_energyMask) << s_energyBit;
  myRoIWords[ix] |= (eyError & s_overflowMask) << s_overflowBit;
  myRoIWords[ix] |= (missingEtHits & s_missingEtHitsMask) << s_missingEtHitsBit;
  type = MASKED;
  ix = s_eyOffset + type;
  myRoIWords[ix] = s_wordIdVal1 << s_wordIdBit;
  myRoIWords[ix] |= type << s_sumTypeBit;
  myRoIWords[ix] |= (eyM & s_energyMask) << s_energyBit;
  myRoIWords[ix] |= (eyErrorM & s_overflowMask) << s_overflowBit;
  myRoIWords[ix] |= (missingEtHitsM & s_missingEtHitsMask)
                    << s_missingEtHitsBit;
  type = NORMAL;
  ix = s_etOffset + type;
  myRoIWords[ix] = s_wordIdVal2 << s_wordIdBit;
  myRoIWords[ix] |= type << s_sumTypeBit;
  myRoIWords[ix] |= (et & s_energyMask) << s_energyBit;
  myRoIWords[ix] |= (etError & s_overflowMask) << s_overflowBit;
  myRoIWords[ix] |= (sumEtHits & s_sumEtHitsMask) << s_sumEtHitsBit;

  type = MASKED;
  ix = s_etOffset + type;
  myRoIWords[ix] = s_wordIdVal2 << s_wordIdBit;
  myRoIWords[ix] |= type << s_sumTypeBit;
  myRoIWords[ix] |= (etM & s_energyMask) << s_energyBit;
  myRoIWords[ix] |= (etErrorM & s_overflowMask) << s_overflowBit;
  myRoIWords[ix] |= (sumEtHitsM & s_sumEtHitsMask) << s_sumEtHitsBit;

  setCmxRoIWords(myRoIWords);
}

/// Return Ex
unsigned int CMXRoI_v1::ex(SumType type) const {
  return (cmxRoIWords()[s_exOffset + type] >> s_energyBit) & s_energyMask;
}
/// Return Ey
unsigned int CMXRoI_v1::ey(SumType type) const {
  return (cmxRoIWords()[s_eyOffset + type] >> s_energyBit) & s_energyMask;
}
/// Return Et
unsigned int CMXRoI_v1::et(SumType type) const {
  return (cmxRoIWords()[s_etOffset + type] >> s_energyBit) & s_energyMask;
}
/// Return Ex error flags (bit 0 Overflow)
int CMXRoI_v1::exError(SumType type) const {
  return (cmxRoIWords()[s_exOffset + type] >> s_overflowBit) & s_overflowMask;
}
/// Return Ey error flags (bit 0 Overflow)
int CMXRoI_v1::eyError(SumType type) const {
  return (cmxRoIWords()[s_eyOffset + type] >> s_overflowBit) & s_overflowMask;
}
/// Return Et error flags (bit 0 Overflow)
int CMXRoI_v1::etError(SumType type) const {
  return (cmxRoIWords()[s_etOffset + type] >> s_overflowBit) & s_overflowMask;
}
/// Return Sum-ET hits
unsigned int CMXRoI_v1::sumEtHits(SumType type) const {
  return (cmxRoIWords()[s_etOffset + type] >> s_sumEtHitsBit) & s_sumEtHitsMask;
}
/// Return Missing-ET hits
unsigned int CMXRoI_v1::missingEtHits(SumType type) const {
  return (cmxRoIWords()[s_eyOffset + type] >> s_missingEtHitsBit) &
         s_missingEtHitsMask;
}
/// Return Missing-ET-Sig hits
unsigned int CMXRoI_v1::missingEtSigHits() const {
  return (cmxRoIWords()[s_exOffset + NORMAL] >> s_missingEtSigHitsBit) &
         s_missingEtSigHitsMask;
}

/// Return packed Energy RoI word (0-5)
uint32_t CMXRoI_v1::roiWord(int word) const {
  return (word < s_maxRoiWords) ? cmxRoIWords()[word] : 0;
}
/// Return Ex RoI word
unsigned int CMXRoI_v1::exWord(SumType type) const {
  return cmxRoIWords()[s_exOffset + type];
}
/// Return Ey RoI word
unsigned int CMXRoI_v1::eyWord(SumType type) const {
  return cmxRoIWords()[s_eyOffset + type];
}
/// Return ET RoI word
unsigned int CMXRoI_v1::etWord(SumType type) const {
  return cmxRoIWords()[s_etOffset + type];
}

}  // namespace xAOD
