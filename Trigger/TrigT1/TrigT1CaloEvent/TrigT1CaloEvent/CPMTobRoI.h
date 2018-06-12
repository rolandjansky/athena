/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_CPMTOBROI_H
#define TRIGT1CALO_CPMTOBROI_H

#include <stdint.h>

namespace LVL1 {

/** CPM RoI data.
 *
 *  Contains a Cluster Processor Module RoI word post LS1.
 *  Used in analysis of RoI readout.
 *
 *  @author Peter Faulkner
 */

class CPMTobRoI {

 public:

   CPMTobRoI();
   CPMTobRoI(uint32_t roiWord);
   CPMTobRoI(int crate, int cpm, int chip, int location, int type,
                                           int energy, int isol);

   ~CPMTobRoI();

   /// Return crate number (0-1)
   int crate()    const;
   /// Return CPM number (1-14)
   int cpm()      const;
   /// Return CP chip number (0-7)
   int chip()     const;
   /// Return location (RoI local coords) (0-7)
   int location() const;
   /// Return type em/tau (0/1)
   int type()     const;
   /// Return energy
   int energy()   const;
   /// Return isolation
   int isolation() const;

   /// Return packed RoI word
   uint32_t roiWord() const;
   /// Set RoI word with ID check
   bool setRoiWord(uint32_t roiWord);

 private:

   /// RoI word ID
   static const int s_wordIdVal       = 0xa;
   //  Data locations
   static const int s_wordIdBit       = 28;
   static const int s_crateBit        = 26;
   static const int s_cpmBit          = 22;
   static const int s_chipBit         = 19;
   static const int s_locationBit     = 16;
   static const int s_isolBit         = 8;
   static const int s_energyBit       = 0;
   //  Data masks
   static const int s_wordIdMask      = 0xf;
   static const int s_crateMask       = 0x3;
   static const int s_cpmMask         = 0xf;
   static const int s_chipMask        = 0x7;
   static const int s_locationMask    = 0x7;
   static const int s_isolMask        = 0x1f;
   static const int s_energyMask      = 0xff;
   static const int s_typeMask        = 0x1;

   /// RoI word
   uint32_t m_roiWord;

};

inline int CPMTobRoI::crate() const
{
  return (m_roiWord >> s_crateBit) & s_crateMask;
}

inline int CPMTobRoI::cpm() const
{
  return (m_roiWord >> s_cpmBit) & s_cpmMask;
}

inline int CPMTobRoI::chip() const
{
  return (m_roiWord >> s_chipBit) & s_chipMask;
}

inline int CPMTobRoI::location() const
{
  return (m_roiWord >> s_locationBit) & s_locationMask;
}

inline int CPMTobRoI::type() const
{
  return (m_roiWord >> s_wordIdBit) & s_typeMask;
}

inline int CPMTobRoI::energy() const
{
  return (m_roiWord >> s_energyBit) & s_energyMask;
}

inline int CPMTobRoI::isolation() const
{
  return (m_roiWord >> s_isolBit) & s_isolMask;
}

inline uint32_t CPMTobRoI::roiWord() const
{
  return m_roiWord;
}

} // end namespace

#ifndef CPMTobRoI_ClassDEF_H
#include "TrigT1CaloEvent/CPMTobRoI_ClassDEF.h"
#endif

#endif
