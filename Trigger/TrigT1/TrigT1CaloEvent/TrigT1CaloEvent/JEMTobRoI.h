/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_JEMTOBROI_H
#define TRIGT1CALO_JEMTOBROI_H

#include <stdint.h>

namespace LVL1 {

/** JEM RoI data.
 *
 *  Contains a Jet RoI word post LS1.
 *  Used in analysis of RoI readout.
 *
 *  @author Peter Faulkner
 */

class JEMTobRoI {

 public:

   JEMTobRoI();
   JEMTobRoI(uint32_t roiWord);
   JEMTobRoI(int crate, int jem, int frame, int location, int energyLarge,
             int energySmall);

   ~JEMTobRoI();

   /// Return crate number (0-1)
   int crate()    const;
   /// Return JEM number (0-15)
   int jem()      const;
   /// Return RoI frame number (0-7)
   int frame()    const;
   /// Return location (RoI local coords) (0-3)
   int location() const;
   /// Return energy large window size
   int energyLarge() const;
   /// Return energy small window size
   int energySmall() const;

   /// Return packed RoI word
   uint32_t roiWord() const;
   /// Set RoI word with ID check
   bool setRoiWord(uint32_t roiWord);

 private:

   /// RoI word ID
   static const int s_wordIdVal       = 0;
   //  Data locations
   static const int s_wordIdBit       = 29;
   static const int s_crateBit        = 28;
   static const int s_jemBit          = 24;
   static const int s_frameBit        = 21;
   static const int s_locationBit     = 19;
   static const int s_energySmallBit  = 10;
   static const int s_energyLargeBit  = 0;
   //  Data masks
   static const int s_wordIdMask      = 0x7;
   static const int s_crateMask       = 0x1;
   static const int s_jemMask         = 0xf;
   static const int s_frameMask       = 0x7;
   static const int s_locationMask    = 0x3;
   static const int s_energySmallMask = 0x1ff;
   static const int s_energyLargeMask = 0x3ff;

   /// RoI word
   uint32_t m_roiWord;

};

inline int JEMTobRoI::crate() const
{
  return (m_roiWord >> s_crateBit) & s_crateMask;
}

inline int JEMTobRoI::jem() const
{
  return (m_roiWord >> s_jemBit) & s_jemMask;
}

inline int JEMTobRoI::frame() const
{
  return (m_roiWord >> s_frameBit) & s_frameMask;
}

inline int JEMTobRoI::location() const
{
  return (m_roiWord >> s_locationBit) & s_locationMask;
}

inline int JEMTobRoI::energyLarge() const
{
  return (m_roiWord >> s_energyLargeBit) & s_energyLargeMask;
}

inline int JEMTobRoI::energySmall() const
{
  return (m_roiWord >> s_energySmallBit) & s_energySmallMask;
}

inline uint32_t JEMTobRoI::roiWord() const
{
  return m_roiWord;
}

} // end namespace

#ifndef JEMTobRoI_ClassDEF_H
#include "TrigT1CaloEvent/JEMTobRoI_ClassDEF.h"
#endif

#endif
