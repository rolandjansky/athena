/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_CPMROI_H
#define TRIGT1CALO_CPMROI_H

#include <stdint.h>

namespace LVL1 {

/** CPM RoI data.
 *
 *  Contains a Cluster Processor Module RoI word.
 *  Used in analysis of RoI readout.
 *
 *  @author Peter Faulkner
 */

class CPMRoI {

 public:

   CPMRoI();
   CPMRoI(uint32_t roiWord);
   CPMRoI(int crate, int cpm, int chip, int location, int hits, int error);

   ~CPMRoI();

   /// Return crate number (0-1)
   int crate()    const;
   /// Return CPM number (1-14)
   int cpm()      const;
   /// Return CP chip number (0-7)
   int chip()     const;
   /// Return location (RoI local coords) (0-7)
   int location() const;
   /// Return hit thresholds map
   int hits()     const;
   /// Return error flags (bit 0 Saturation, bit 1 Parity)
   int error()    const;

   /// Return packed RoI word
   uint32_t roiWord() const;
   /// Set RoI word with ID check
   bool setRoiWord(uint32_t roiWord);

 private:

   /// RoI word ID
   static const int s_wordIdVal       = 0x0;
   //  Data locations
   static const int s_wordIdBit       = 30;
   static const int s_crateBit        = 28;
   static const int s_cpmBit          = 24;
   static const int s_chipBit         = 21;
   static const int s_locationBit     = 18;
   static const int s_parityBit       = 17;
   static const int s_saturationBit   = 16;
   static const int s_hitsBit         = 0;
   //  Data masks
   static const int s_wordIdMask      = 0x3;
   static const int s_crateMask       = 0x3;
   static const int s_cpmMask         = 0xf;
   static const int s_chipMask        = 0x7;
   static const int s_locationMask    = 0x7;
   static const int s_parityMask      = 0x1;
   static const int s_saturationMask  = 0x1;
   static const int s_hitsMask        = 0xffff;

   /// Return parity error flag (0/1)
   int parity()     const;
   /// Return saturation flag (0/1)
   int saturation() const;

   /// RoI word
   uint32_t m_roiWord;

};

inline int CPMRoI::crate() const
{
  return (m_roiWord >> s_crateBit) & s_crateMask;
}

inline int CPMRoI::cpm() const
{
  return (m_roiWord >> s_cpmBit) & s_cpmMask;
}

inline int CPMRoI::chip() const
{
  return (m_roiWord >> s_chipBit) & s_chipMask;
}

inline int CPMRoI::location() const
{
  return (m_roiWord >> s_locationBit) & s_locationMask;
}

inline int CPMRoI::hits() const
{
  return (m_roiWord >> s_hitsBit) & s_hitsMask;
}

inline int CPMRoI::error() const
{
  return (parity() << 1) | saturation();
}

inline uint32_t CPMRoI::roiWord() const
{
  return m_roiWord;
}

inline int CPMRoI::parity() const
{
  return (m_roiWord >> s_parityBit) & s_parityMask;
}

inline int CPMRoI::saturation() const
{
  return (m_roiWord >> s_saturationBit) & s_saturationMask;
}

} // end namespace

#ifndef CPMRoI_ClassDEF_H
#include "TrigT1CaloEvent/CPMRoI_ClassDEF.h"
#endif

#endif
