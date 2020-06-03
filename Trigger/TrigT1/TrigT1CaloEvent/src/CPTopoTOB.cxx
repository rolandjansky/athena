/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CPTopoTOB.h"
#include <cmath>

namespace LVL1 {

// Static constants

const int CPTopoTOB::s_cpmBit;
const int CPTopoTOB::s_chipBit;
const int CPTopoTOB::s_locationBit;
const int CPTopoTOB::s_isolBit;
const int CPTopoTOB::s_etBit;

const int CPTopoTOB::s_cpmMask;
const int CPTopoTOB::s_chipMask;
const int CPTopoTOB::s_locationMask;
const int CPTopoTOB::s_isolMask;
const int CPTopoTOB::s_etMask;

const int CPTopoTOB::s_chipPhiWidth;
const int CPTopoTOB::s_cpmEtaWidth;
const int CPTopoTOB::s_cpmPhiWidth;
const int CPTopoTOB::s_cpmEtaOffset;
const int CPTopoTOB::s_cpmPhiOffset;
const int CPTopoTOB::s_cpmPhiBins;


CPTopoTOB::CPTopoTOB() : m_crate(0), m_cmx(0), m_tobWord(0)
{
}

CPTopoTOB::CPTopoTOB(int crate, int cmx, uint32_t tobWord) : m_crate(crate), m_cmx(cmx), m_tobWord(tobWord)
{
}

CPTopoTOB::CPTopoTOB(int crate, int cmx, int cpm, int chip, int location, int isolation, int et) : m_crate(crate), m_cmx(cmx)
{
  m_tobWord  =  0;
  m_tobWord |= (cpm        & s_cpmMask)        << s_cpmBit;
  m_tobWord |= (chip       & s_chipMask)       << s_chipBit;
  m_tobWord |= (location   & s_locationMask)   << s_locationBit;
  m_tobWord |= (isolation  & s_isolMask)       << s_isolBit;
  m_tobWord |= (et         & s_etMask)         << s_etBit;
}


CPTopoTOB::CPTopoTOB(uint32_t roiWord) 
{
   m_crate = (roiWord>>26) & 0x3; 
   if ((roiWord&0xf0000000) == 0xa0000000) m_cmx = 0;
   else                                    m_cmx = 1;
   
   uint32_t coord = (roiWord>>16) & 0x3ff;
   uint32_t data  = (roiWord&0x1fff);
   m_tobWord = data + (coord<<13);
}

CPTopoTOB::~CPTopoTOB()
{
}

/** Extract eta index from the TOB data (0-49) */
int CPTopoTOB::etaIndex() const
{
  int loc = location();
  return cpm()*s_cpmEtaWidth + (loc >> 2)*2 + (loc&1);
}

// Integer eta coordinate of RoI

int CPTopoTOB::ieta() const
{
  return etaIndex()  - s_cpmEtaOffset + 1;
}

// Extract eta coordinate from TOB data (RoI centre coordinate)

float CPTopoTOB::eta() const
{
  return ieta()*0.1;
}

/** Extract phi index from the TOB data (0-63) */
int CPTopoTOB::phiIndex() const
{
  return ((location()>>1)&1) + chip()*s_chipPhiWidth  + m_crate*s_cpmPhiWidth;  
}

// Extract integer phi coordinate from TOB data

int CPTopoTOB::iphi() const
{
  int iphi = phiIndex() + 1;
  if (iphi > 63) iphi -= 64;
  return iphi;
}

int CPTopoTOB::iphiSigned() const
{
  int iphi = phiIndex() + 1;
  if (iphi > 32) iphi -= 64;
  return iphi;
}

// Extract phi coordinate from TOB data

float CPTopoTOB::phi() const
{
  return iphiSigned()*M_PI/32;
}

// Construct RoI word and return
uint32_t CPTopoTOB::roiWord() const
{
  // ET, isolation
  uint32_t word = m_tobWord&0x1fff;
  // Coordinate within crate
  word += ((m_tobWord&0x7fe000)<<3);
  // Crate number
  word += (m_crate<<26);
  // Type
  if (m_cmx == 0) word += 0xa0000000;
  else            word += 0xb0000000;
  
  return word;
}

} // end namespace
