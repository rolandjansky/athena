
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

CPTopoTOB::~CPTopoTOB()
{
}

// Extract eta index from TOB data (reference tower index)

int CPTopoTOB::ieta() const
{
  int loc = location();
  int locEta  = (loc >> 2)*2 + (loc&1);
  int ieta = locEta + cpm()*s_cpmEtaWidth  - s_cpmEtaOffset;
  return ieta;
}

// Extract eta coordinate from TOB data (RoI centre coordinate)

float CPTopoTOB::eta() const
{
  return (ieta()+1)*0.1;
}

// Extract phi index from TOB data

int CPTopoTOB::iphi() const
{
  int loc = location();
  int locPhi = (loc >> 1)&1;
  int iphi = locPhi + chip()*s_chipPhiWidth  + m_crate*s_cpmPhiWidth + s_cpmPhiOffset;
  if (iphi >= s_cpmPhiBins/2) iphi -= s_cpmPhiBins;
  return iphi;
}

// Extract phi coordinate from TOB data

float CPTopoTOB::phi() const
{
  return (iphi()+1)*M_PI/32;
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
