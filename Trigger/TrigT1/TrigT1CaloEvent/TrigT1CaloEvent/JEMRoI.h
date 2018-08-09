/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_JEMROI_H
#define TRIGT1CALO_JEMROI_H

#include <stdint.h>

namespace LVL1 {

/** JEM RoI data.
 *
 *  Contains a Main Jet RoI word or a Forward Jet RoI word.
 *  Used in analysis of RoI readout.
 *
 *  @author Peter Faulkner
 */

class JEMRoI {

 public:

   JEMRoI();
   JEMRoI(uint32_t roiWord);
   JEMRoI(int crate, int jem, int frame, int location, int forward,
          int hits, int error);

   ~JEMRoI();

   /// Return crate number (0-1)
   int crate()    const;
   /// Return JEM number (0-15)
   int jem()      const;
   /// Return RoI frame number (0-7)
   int frame()    const;
   /// Return location (RoI local coords) (0-3)
   int location() const;
   /// Return forward jet flag (0/1)
   int forward()  const;
   /// Return Jet hit map (8 bits Main or 4 bits Forward)
   int hits()     const;
   /// Return error flags (bit 0 Saturation, bit 1 Parity)
   int error()    const;

   /// Return packed RoI word
   uint32_t roiWord() const;
   /// Set RoI word with ID check
   bool setRoiWord(uint32_t roiWord);

 private:

   /// RoI word ID
   static const int s_wordIdVal       = 0x4;
   //  Data locations
   static const int s_wordIdBit       = 29;
   static const int s_crateBit        = 28;
   static const int s_jemBit          = 24;
   static const int s_frameBit        = 21;
   static const int s_locationBit     = 19;
   static const int s_forwardBit      = 18;
   static const int s_parityBit       = 17;
   static const int s_saturationBit   = 16;
   static const int s_forwardHitsBit  = 8;
   static const int s_mainHitsBit     = 0;
   //  Data masks
   static const int s_wordIdMask      = 0x7;
   static const int s_crateMask       = 0x1;
   static const int s_jemMask         = 0xf;
   static const int s_frameMask       = 0x7;
   static const int s_locationMask    = 0x3;
   static const int s_forwardMask     = 0x1;
   static const int s_parityMask      = 0x1;
   static const int s_saturationMask  = 0x1;
   static const int s_forwardHitsMask = 0xf;
   static const int s_mainHitsMask    = 0xff;

   /// Return parity error flag (0/1)
   int parity()     const;
   /// Return saturation flag (0/1)
   int saturation() const;

   /// RoI word
   uint32_t m_roiWord;

};

inline int JEMRoI::crate() const
{
  return (m_roiWord >> s_crateBit) & s_crateMask;
}

inline int JEMRoI::jem() const
{
  return (m_roiWord >> s_jemBit) & s_jemMask;
}

inline int JEMRoI::frame() const
{
  return (m_roiWord >> s_frameBit) & s_frameMask;
}

inline int JEMRoI::location() const
{
  return (m_roiWord >> s_locationBit) & s_locationMask;
}

inline int JEMRoI::forward() const
{
  return (m_roiWord >> s_forwardBit) & s_forwardMask;
}

inline int JEMRoI::error() const
{
  return (parity() << 1) | saturation();
}

inline uint32_t JEMRoI::roiWord() const
{
  return m_roiWord;
}

inline int JEMRoI::parity() const
{
  return (m_roiWord >> s_parityBit) & s_parityMask;
}

inline int JEMRoI::saturation() const
{
  return (m_roiWord >> s_saturationBit) & s_saturationMask;
}

} // end namespace

#ifndef JEMRoI_ClassDEF_H
#include "TrigT1CaloEvent/JEMRoI_ClassDEF.h"
#endif

#endif
