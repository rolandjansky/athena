/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_CPTOPOTOB_H
#define TRIGT1CALO_CPTOPOTOB_H

#include <stdint.h>

namespace LVL1 {

/** CP TOB data for L1Topo.
 *
 *  Contains a Cluster Processor TOB word plus crate and CMX numbers.
 *  Plus functions to return individual data members
 *
 *  @author Alan Watson
 */

class CPTopoTOB {

 public:

   CPTopoTOB();
   CPTopoTOB(int crate, int cmx, uint32_t tobWord);
   CPTopoTOB(int crate, int cmx, int cpm, int chip, int location, int isolation, int et);
   CPTopoTOB(uint32_t roiWord);

   ~CPTopoTOB();

   /// Return crate number (0-3)
   int crate()    const;
   /// Return CMX number (0-1)
   int cmx()      const;
   /// Return CPM number (1-14)
   int cpm()      const;
   /// Return CP chip number (0-7)
   int chip()     const;
   /// Return location (RoI local coords) (0-7)
   unsigned int location() const;
   /// Return isolation mask
   unsigned int isolation()     const;
   /// ET value
   unsigned int et()    const;
   
   // eta index
   int etaIndex() const;
   // phi index
   int phiIndex() const;
   // Integer eta index
   int ieta() const;
   // Integer phi index
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
   static const int s_cpmBit          = 19;
   static const int s_chipBit         = 16;
   static const int s_locationBit     = 13;
   static const int s_isolBit         =  8;
   static const int s_etBit           =  0;
   //  Data masks
   static const int s_cpmMask         = 0xf;
   static const int s_chipMask        = 0x7;
   static const int s_locationMask    = 0x7;
   static const int s_isolMask        = 0x1f;
   static const int s_etMask          = 0xff;
   // For decoding coordinates
   static const int s_chipPhiWidth    =  2;
   static const int s_cpmEtaWidth     =  4;
   static const int s_cpmPhiWidth     = 16;
   static const int s_cpmEtaOffset    = 32;
   static const int s_cpmPhiOffset    =  0;
   static const int s_cpmPhiBins      = 64;

   /// crate
   int m_crate;
   
   /// CMX (EM or Tau)
   int m_cmx;

   /// RoI word
   uint32_t m_tobWord;
  
};

inline int CPTopoTOB::crate() const
{
  return m_crate;
}

inline int CPTopoTOB::cmx() const
{
  return m_cmx;
}

inline int CPTopoTOB::cpm() const
{
  return (m_tobWord >> s_cpmBit) & s_cpmMask;
}

inline int CPTopoTOB::chip() const
{
  return (m_tobWord >> s_chipBit) & s_chipMask;
}

inline unsigned int CPTopoTOB::location() const
{
  return (m_tobWord >> s_locationBit) & s_locationMask;
}

inline unsigned int CPTopoTOB::isolation() const
{
  return (m_tobWord >> s_isolBit) & s_isolMask;
}

inline unsigned int CPTopoTOB::et() const
{
  return (m_tobWord >> s_etBit) & s_etMask;
}

inline uint32_t CPTopoTOB::tobWord() const
{
  return m_tobWord;
}


} // end namespace

#endif
