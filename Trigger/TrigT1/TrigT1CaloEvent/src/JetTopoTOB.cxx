/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/JetTopoTOB.h"
#include <cmath>
#include <stdlib.h>

namespace LVL1 {

// Static constants

const int JetTopoTOB::s_jemBit;
const int JetTopoTOB::s_frameBit;
const int JetTopoTOB::s_lcBit;
const int JetTopoTOB::s_etSmallBit;
const int JetTopoTOB::s_etLargeBit;

const int JetTopoTOB::s_jemMask;
const int JetTopoTOB::s_frameMask;
const int JetTopoTOB::s_lcMask;
const int JetTopoTOB::s_etSmallMask;
const int JetTopoTOB::s_etLargeMask;

JetTopoTOB::JetTopoTOB() : m_crate(0), m_tobWord(0)
{
}

JetTopoTOB::JetTopoTOB(int crate, uint32_t tobWord) : m_crate(crate), m_tobWord(tobWord)
{
}

JetTopoTOB::JetTopoTOB(int crate, int jem, int frame, int localCoord, int etSmall, int etLarge) : m_crate(crate)
{
  m_tobWord  =  0;
  m_tobWord |= (jem        & s_jemMask)        << s_jemBit;
  m_tobWord |= (frame      & s_frameMask)      << s_frameBit;
  m_tobWord |= (localCoord & s_lcMask)         << s_lcBit;
  m_tobWord |= (etSmall    & s_etSmallMask)    << s_etSmallBit;
  m_tobWord |= (etLarge    & s_etLargeMask)    << s_etLargeBit;
}

JetTopoTOB::JetTopoTOB(uint32_t roiWord) 
{
  m_crate = (roiWord>>28) & 0x1; 
  m_tobWord  = (roiWord&0xfffffff);
}

JetTopoTOB::~JetTopoTOB()
{
}

/** Extract eta index from the TOB data (0-31) */
int JetTopoTOB::etaIndex() const
{
  return (localCoord()&1) + (frame()>>2)*s_frameEtaWidth + (jem()&7)*s_jemEtaWidth;
}

/** Extract eta coordinate from TOB data
    The index is an integer eta coordinate in multiples of 0.1 (TT eta granularity) */

int JetTopoTOB::ieta() const
{
  
  int index = etaIndex();
  
  int ieta = 0;
  if (index < 4 || index > 26) {
    if (index==0)       ieta = -s_jemFJEta;
    else if (index==1)  ieta = -s_jemEC1Eta;
    else if (index==2)  ieta = -s_jemEC2Eta;
    else if (index==3)  ieta = -s_jemEC3Eta;
    else if (index==27) ieta =  s_jemEC3Eta;
    else if (index==28) ieta =  s_jemEC2Eta;
    else if (index==29) ieta =  s_jemEC1Eta;
    else if (index==30) ieta =  s_jemFJEta;
    else if (index==31) ieta =  s_jemFJEta;
  }
  else {
    ieta = (index - s_jemEtaOffset)*s_jetElementWidth;
  }
  
  return ieta;
}

// Extract eta coordinate from TOB data

float JetTopoTOB::eta() const
{
  
  float eta = ieta()*0.1;

  /*  
  int absEta = abs(ieta());
  int sign = (eta > 0 ? 1 : -1);

  switch (absEta) {
    case s_jemEC3Eta:
      eta = sign*2.45;
      break;
    case s_jemEC2Eta:
      eta = sign*2.65;
      break;
    case s_jemEC1Eta:
      eta = sign*2.95;
      break;
  }
  */
  
  return eta;
}


/** Extract phi index from the TOB data (0-31) */
int JetTopoTOB::phiIndex() const
{
  int index = (localCoord() >> 1) + (frame()&3)*s_framePhiWidth + m_crate*s_jemPhiWidth;
  if (jem() >= s_nJemPerQuadrant) index += 2*s_jemPhiWidth;
  
  return index;
}


/** Extract integer phi coordinate from TOB data.
    The coordinate has the range 0 -> 62 in steps of 2*/

int JetTopoTOB::iphi() const
{
  int index = phiIndex();
  
  int iphi = (index+1)*s_jetElementWidth;
  if (iphi > 63) iphi -= 64;
  return iphi;
}

/** Signed iphi (-30 -> 32) */

int JetTopoTOB::iphiSigned() const
{
  int index = phiIndex();
  
  int iphi = (index+1)*s_jetElementWidth;
  if (iphi > 32) iphi -= 64;
  return iphi;
}

// Extract phi coordinate from TOB data

float JetTopoTOB::phi() const
{
  float phi = iphiSigned()*M_PI/32;
  return phi;
}

// Construct RoI word and return
uint32_t JetTopoTOB::roiWord() const
{
  // ET values and coordinate within crate
  uint32_t word = m_tobWord&0xfffffff;
  // Crate number
  word += (m_crate<<28);
  // Type is 000 anyway, so nothing more to do
  
  return word;
}

} // end namespace
