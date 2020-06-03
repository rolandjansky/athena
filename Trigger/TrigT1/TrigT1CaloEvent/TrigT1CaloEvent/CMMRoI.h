/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_CMMROI_H
#define TRIGT1CALO_CMMROI_H

#include <stdint.h>

#include "CLIDSvc/CLASS_DEF.h"

namespace LVL1 {

/** CMM RoI data.
 *
 *  Contains a Jet-ET RoI word and three Energy RoI words.
 *  Used in analysis of RoI readout.
 *
 *  @author Peter Faulkner
 */

class CMMRoI {

 public:

   CMMRoI();
   CMMRoI(uint32_t jetEtRoiWord,   uint32_t energyRoiWord0,
          uint32_t energyRoiWord1, uint32_t energyRoiWord2);
   CMMRoI(int jetEtHits, int sumEtHits, int missingEtHits,
          int missingEtSigHits, int ex, int ey, int et,
          int jetEtError, int sumEtError, int missingEtError,
          int missingEtSigError, int exError, int eyError, int etError);

   ~CMMRoI();

   /// Return Jet-ET hits
   int jetEtHits()         const;
   /// Return Sum-ET hits
   int sumEtHits()         const;
   /// Return Missing-ET hits
   int missingEtHits()     const;
   /// Return Missing-ET-Sig hits
   int missingEtSigHits()  const;
   /// Return Ex
   int ex()                const;
   /// Return Ey
   int ey()                const;
   /// Return Et
   int et()                const;
   /// Return Jet-ET error flag (bit 1 Parity)
   int jetEtError()        const;
   /// Return Sum-ET error flag (bit 1 Parity)
   int sumEtError()        const;
   /// Return Missing-ET error flag (bit 1 Parity)
   int missingEtError()    const;
   /// Return Missing-ET-Sig error flag (bit 1 Parity)
   int missingEtSigError() const;
   /// Return Ex error flags (bit 0 Overflow, bit 1 Parity)
   int exError()           const;
   /// Return Ey error flags (bit 0 Overflow, bit 1 Parity)
   int eyError()           const;
   /// Return Et error flags (bit 0 Overflow, bit 1 Parity)
   int etError()           const;

   /// Return packed Jet-Et RoI word
   uint32_t jetEtRoiWord()   const;
   /// Return packed Energy RoI word 0
   uint32_t energyRoiWord0() const;
   /// Return packed Energy RoI word 1
   uint32_t energyRoiWord1() const;
   /// Return packed Energy RoI word 2
   uint32_t energyRoiWord2() const;
   /// Set RoI word with ID check
   bool setRoiWord(uint32_t roiWord);

 private:

   //  RoI word IDs
   static const int s_jetEtWordIdVal       = 0x5;
   static const int s_wordIdVal0           = 0x4;
   static const int s_wordIdVal1           = 0x6;
   static const int s_wordIdVal2           = 0x5;
   //  Data locations
   static const int s_jetEtWordIdBit       = 29;
   static const int s_wordIdBit            = 28;
   static const int s_jetEtHitsBit         = 0;
   static const int s_sumEtHitsBit         = 16;
   static const int s_missingEtHitsBit     = 16;
   static const int s_missingEtSigHitsBit  = 16;
   static const int s_energyBit            = 0;
   static const int s_jetEtParityBit       = 28;
   static const int s_parityBit            = 27;
   static const int s_overflowBit          = 15;
   //  Data masks
   static const int s_jetEtWordIdMask      = 0x7;
   static const int s_wordIdMask           = 0xf;
   static const int s_jetEtHitsMask        = 0xf;
   static const int s_sumEtHitsMask        = 0xff;
   static const int s_missingEtHitsMask    = 0xff;
   static const int s_missingEtSigHitsMask = 0xff;
   static const int s_energyMask           = 0x7fff;

   /// Return Energy parity error (0/1)
   int parity(uint32_t roiWord) const;
   /// Return Energy overflow flag (0/1)
   int overflow(uint32_t roiWord) const;

   //  RoI words
   uint32_t m_jetEtRoiWord;
   uint32_t m_energyRoiWord0;
   uint32_t m_energyRoiWord1;
   uint32_t m_energyRoiWord2;

};

inline int CMMRoI::jetEtHits() const
{
  return (m_jetEtRoiWord >> s_jetEtHitsBit) & s_jetEtHitsMask;
}

inline int CMMRoI::sumEtHits() const
{
  return (m_energyRoiWord1 >> s_sumEtHitsBit) & s_sumEtHitsMask;
}

inline int CMMRoI::missingEtHits() const
{
  return (m_energyRoiWord2 >> s_missingEtHitsBit) & s_missingEtHitsMask;
}

inline int CMMRoI::missingEtSigHits() const
{
  return (m_energyRoiWord0 >> s_missingEtSigHitsBit) & s_missingEtSigHitsMask;
}

inline int CMMRoI::ex() const
{
  return (m_energyRoiWord0 >> s_energyBit) & s_energyMask;
}

inline int CMMRoI::ey() const
{
  return (m_energyRoiWord1 >> s_energyBit) & s_energyMask;
}

inline int CMMRoI::et() const
{
  return (m_energyRoiWord2 >> s_energyBit) & s_energyMask;
}

inline int CMMRoI::jetEtError() const
{
  return ((m_jetEtRoiWord >> s_jetEtParityBit) & 0x1) << 1;
}

inline int CMMRoI::sumEtError() const
{
  return parity(m_energyRoiWord1) << 1;
}

inline int CMMRoI::missingEtError() const
{
  return parity(m_energyRoiWord2) << 1;
}

inline int CMMRoI::missingEtSigError() const
{
  return parity(m_energyRoiWord0) << 1;
}

inline int CMMRoI::exError() const
{
  return (parity(m_energyRoiWord0) << 1) | overflow(m_energyRoiWord0);
}

inline int CMMRoI::eyError() const
{
  return (parity(m_energyRoiWord1) << 1) | overflow(m_energyRoiWord1);
}

inline int CMMRoI::etError() const
{
  return (parity(m_energyRoiWord2) << 1) | overflow(m_energyRoiWord2);
}

inline uint32_t CMMRoI::jetEtRoiWord() const
{
  return m_jetEtRoiWord;
}

inline uint32_t CMMRoI::energyRoiWord0() const
{
  return m_energyRoiWord0;
}

inline uint32_t CMMRoI::energyRoiWord1() const
{
  return m_energyRoiWord1;
}

inline uint32_t CMMRoI::energyRoiWord2() const
{
  return m_energyRoiWord2;
}

inline int CMMRoI::parity(uint32_t roiWord) const
{
  return (roiWord >> s_parityBit) & 0x1;
}

inline int CMMRoI::overflow(uint32_t roiWord) const
{
  return (roiWord >> s_overflowBit) & 0x1;
}

} // end namespace

CLASS_DEF(LVL1::CMMRoI, 187997318, 1)

#endif
