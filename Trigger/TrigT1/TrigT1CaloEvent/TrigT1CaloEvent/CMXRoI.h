/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_CMXROI_H
#define TRIGT1CALO_CMXROI_H

#include <stdint.h>
#include <vector>

#include "CLIDSvc/CLASS_DEF.h"

namespace LVL1 {

/** CMX RoI data.
 *
 *  Contains six Energy RoI words.
 *  Used in analysis of RoI readout.
 *
 *  @author Peter Faulkner
 */

class CMXRoI {

 public:

   enum SumType { NORMAL = 0, MASKED = 1 };

   CMXRoI();
   CMXRoI(uint32_t roiWord0, uint32_t roiWord1, uint32_t roiWord2,
          uint32_t roiWord3, uint32_t roiWord4, uint32_t roiWord5);
   CMXRoI(unsigned int ex, unsigned int ey, unsigned int et,
          int exError, int eyError, int etError,
          unsigned int sumEtHits, unsigned int missingEtHits,
	  unsigned int missingEtSigHits,
	  unsigned int exM, unsigned int eyM, unsigned int etM,
	  int exErrorM, int eyErrorM, int etErrorM,
	  unsigned int sumEtHitsM, unsigned int missingEtHitsM);

   ~CMXRoI();

   /// Return Ex
   unsigned int ex(SumType type = NORMAL)            const;
   /// Return Ey
   unsigned int ey(SumType type = NORMAL)            const;
   /// Return Et
   unsigned int et(SumType type = NORMAL)            const;
   /// Return Ex error flags (bit 0 Overflow)
   int exError(SumType type = NORMAL)                const;
   /// Return Ey error flags (bit 0 Overflow)
   int eyError(SumType type = NORMAL)                const;
   /// Return Et error flags (bit 0 Overflow)
   int etError(SumType type = NORMAL)                const;
   /// Return Sum-ET hits
   unsigned int sumEtHits(SumType type = NORMAL)     const;
   /// Return Missing-ET hits
   unsigned int missingEtHits(SumType type = NORMAL) const;
   /// Return Missing-ET-Sig hits
   unsigned int missingEtSigHits()                   const;

   /// Return packed Energy RoI word (0-5)
   uint32_t roiWord(int word) const;
   /// Return Ex RoI word
   unsigned int exWord(SumType type = NORMAL)        const;
   /// Return Ey RoI word
   unsigned int eyWord(SumType type = NORMAL)        const;
   /// Return ET RoI word
   unsigned int etWord(SumType type = NORMAL)        const;
   /// Set RoI word with ID check
   bool setRoiWord(uint32_t roiWord);

 private:

   //  RoI word IDs
   static const int s_wordIdVal0           = 0x4;
   static const int s_wordIdVal1           = 0x6;
   static const int s_wordIdVal2           = 0x5;
   //  Data locations
   static const int s_wordIdBit            = 28;
   static const int s_sumEtHitsBit         = 16;
   static const int s_missingEtHitsBit     = 16;
   static const int s_missingEtSigHitsBit  = 16;
   static const int s_energyBit            = 0;
   static const int s_overflowBit          = 15;
   static const int s_sumTypeBit           = 26;
   //  Data masks
   static const int s_wordIdMask           = 0xf;
   static const int s_sumEtHitsMask        = 0xff;
   static const int s_missingEtHitsMask    = 0xff;
   static const int s_missingEtSigHitsMask = 0xff;
   static const int s_energyMask           = 0x7fff;
   static const int s_overflowMask         = 0x1;
   static const int s_sumTypeMask          = 0x1;
   //  RoI word array offsets
   static const int s_exOffset    = 0;
   static const int s_eyOffset    = 2;
   static const int s_etOffset    = 4;
   static const int s_maxRoiWords = 6;

   //  RoI words
   std::vector<uint32_t> m_roiWords;

};

inline unsigned int CMXRoI::ex(SumType type) const
{
  return (m_roiWords[s_exOffset+type] >> s_energyBit) & s_energyMask;
}

inline unsigned int CMXRoI::ey(SumType type) const
{
  return (m_roiWords[s_eyOffset+type] >> s_energyBit) & s_energyMask;
}

inline unsigned int CMXRoI::et(SumType type) const
{
  return (m_roiWords[s_etOffset+type] >> s_energyBit) & s_energyMask;
}

inline int CMXRoI::exError(SumType type) const
{
  return (m_roiWords[s_exOffset+type] >> s_overflowBit) & s_overflowMask;
}

inline int CMXRoI::eyError(SumType type) const
{
  return (m_roiWords[s_eyOffset+type] >> s_overflowBit) & s_overflowMask;
}

inline int CMXRoI::etError(SumType type) const
{
  return (m_roiWords[s_etOffset+type] >> s_overflowBit) & s_overflowMask;
}

inline unsigned int CMXRoI::sumEtHits(SumType type) const
{
  return (m_roiWords[s_etOffset+type] >> s_sumEtHitsBit) & s_sumEtHitsMask;
}

inline unsigned int CMXRoI::missingEtHits(SumType type) const
{
  return (m_roiWords[s_eyOffset+type] >> s_missingEtHitsBit) & s_missingEtHitsMask;
}

inline unsigned int CMXRoI::missingEtSigHits() const
{
  return (m_roiWords[s_exOffset+NORMAL] >> s_missingEtSigHitsBit) & s_missingEtSigHitsMask;
}

inline uint32_t CMXRoI::roiWord(int word) const
{
  return (word < s_maxRoiWords) ? m_roiWords[word] : 0;
}

inline unsigned int CMXRoI::exWord(SumType type) const
{
  return m_roiWords[s_exOffset+type];
}

inline unsigned int CMXRoI::eyWord(SumType type) const
{
  return m_roiWords[s_eyOffset+type];
}

inline unsigned int CMXRoI::etWord(SumType type) const
{
  return m_roiWords[s_etOffset+type];
}

} // end namespace

CLASS_DEF(LVL1::CMXRoI, 133156981, 1)

#endif
