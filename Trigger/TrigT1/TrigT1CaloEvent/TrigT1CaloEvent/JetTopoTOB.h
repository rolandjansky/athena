/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_JETTOPOTOB_H
#define TRIGT1CALO_JETTOPOTOB_H

#include <stdint.h>

namespace LVL1 {

/** Jet TOB data for L1Topo.
 *
 *  Contains a Jet Processor TOB word and crate number.
 *  Plus functions to return individual data members
 *
 *  @author Alan Watson
 */

class JetTopoTOB {

 public:

   JetTopoTOB();
   JetTopoTOB(int crate, uint32_t tobWord);
   JetTopoTOB(int crate, int jem, int frame, int localCoord, int etSmall, int etLarge);
   JetTopoTOB(uint32_t roiWord);

   ~JetTopoTOB();

   /// Return crate number (0-3)
   int crate()    const;
   /// Return JEM number (0-15)
   int jem()      const;
   /// Return JEM frame number (0-7)
   int frame()     const;
   /// Return local coordinate (RoI local coords) (0-)
   unsigned int localCoord() const;
   /// Small cluster ET value
   unsigned int etSmall()     const;
   /// Large cluster ET value
   unsigned int etLarge()    const;
   
   // eta index
   int etaIndex() const;
   // phi index
   int phiIndex() const;
   // Integer eta coordinate
   int ieta() const;
   // Integer phi coordinate
   int iphi() const;
   int iphiSigned() const;
   // Floating eta coordinate
   float eta() const;
   // Floating phi coordinate
   float phi() const;

   /// Return packed TOB word
   uint32_t tobWord() const;
   
   /// Return corresponding RoI word (useful for Topo simulation)
   uint32_t roiWord() const;

 private:

   //  Data locations within word
   static const int s_jemBit          = 24;
   static const int s_frameBit        = 21;
   static const int s_lcBit           = 19;
   static const int s_etSmallBit      = 10;
   static const int s_etLargeBit      =  0;
   //  Data masks
   static const int s_jemMask         = 0xf;
   static const int s_frameMask       = 0x7;
   static const int s_lcMask          = 0x3;
   static const int s_etSmallMask     = 0x1ff;
   static const int s_etLargeMask     = 0x3ff;
   // For decoding coordinates
   static const int s_framePhiWidth   =  2; 
   static const int s_frameEtaWidth   =  2; 
   static const int s_jemEtaWidth     =  4; 
   static const int s_jemPhiWidth     =  8; 
   static const int s_jemEtaOffset    = 15; 
   static const int s_jemPhiOffset    =  1; 
   static const int s_jemFJEta        = 39;
   static const int s_jemEC1Eta       = 29;
   static const int s_jemEC2Eta       = 27;
   static const int s_jemEC3Eta       = 25;
   static const int s_nJemPerQuadrant =  8; 
   static const int s_jetElementWidth =  2; 

   /// crate
   int m_crate;
   
   /// RoI word
    uint32_t m_tobWord;

};

inline int JetTopoTOB::crate() const
{
  return m_crate;
}

inline int JetTopoTOB::jem() const
{
  return (m_tobWord >> s_jemBit) & s_jemMask;
}

inline int JetTopoTOB::frame() const
{
  return (m_tobWord >> s_frameBit) & s_frameMask;
}

inline unsigned int JetTopoTOB::localCoord() const
{
  return (m_tobWord >> s_lcBit) & s_lcMask;
}

inline unsigned int JetTopoTOB::etSmall() const
{
  return (m_tobWord >> s_etSmallBit) & s_etSmallMask;
}

inline unsigned int JetTopoTOB::etLarge() const
{
  return (m_tobWord >> s_etLargeBit) & s_etLargeMask;
}

inline uint32_t JetTopoTOB::tobWord() const
{
  return m_tobWord;
}


} // end namespace

#endif
