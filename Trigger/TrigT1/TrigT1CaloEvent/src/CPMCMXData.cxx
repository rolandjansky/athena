/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CPMCMXData.h"

namespace LVL1 {

// Default constructor
CPMCMXData::CPMCMXData() : m_crate(0), m_module(0), m_type(0)
{
   m_DataWords.clear();
   m_DataWords.resize(4);
}

// Construct from pre-calculated data
CPMCMXData::CPMCMXData(int crate, int module, int type, 
                       const std::vector<unsigned int>& tobWords) :
  m_crate(crate),
  m_module(module),
  m_type(type),
  m_DataWords(tobWords)
{ 
}

// Construct from a vector of CPMTobRoIs
CPMCMXData::CPMCMXData(int crate, int module, int type, 
                       const DataVector<CPMTobRoI>* tobs) :
  m_crate(crate),
  m_module(module),
  m_type(type)
{
    
  m_DataWords.clear();
  m_DataWords.resize(4);
   
  /** Step through vector of RoIs. Find those whose crate, module, type match
   *  To make backplane data words we need to geographically sort TOBs        */
   
  std::vector<unsigned int> allTOBs(16,0);
   
  DataVector<CPMTobRoI>::const_iterator itRoI = tobs->begin();
  for ( ; itRoI != tobs->end(); ++itRoI) {
    if ( (*itRoI)->crate() != crate || (*itRoI)->cpm() != module || (*itRoI)->type() != type ) continue;
      
    int chip      = (*itRoI)->chip();
    int chipCoord = (*itRoI)->location();
    int et        = (*itRoI)->energy();
    int isol      = (*itRoI)->isolation();
      
    int side = (chipCoord >> 2);
    int lc   =  chipCoord & 3;
            
    // Set flag in presence map
    unsigned int presence = 2*chip + side;
    m_DataWords[0] |= ( 1 << presence );
      
    // Store TOB data to fill backplane words later
    unsigned int tobdata  = et + (isol<<8) + (lc<<13);
    allTOBs[presence] = tobdata;     
  }
   
  /** Now step though identified TOBs in correct geographical order and fill backplane words */
  int nTOB = 0;
  for (int i = 0; i < 16; ++i) {
    if (allTOBs[i] == 0 ) continue;
     
      unsigned int et   =  allTOBs[i] & 0xff;
      unsigned int isol = (allTOBs[i] >> 8) & 0x1f;
      unsigned int lc   = (allTOBs[i] >> 13) & 3;
      
      switch (nTOB) {
        case 0:
          m_DataWords[1] += et;
          m_DataWords[0] += (isol<<18);
          m_DataWords[0] += (lc<<16);
          break;
        case 1:
          m_DataWords[1] += (et<<8);
          m_DataWords[1] += (isol<<18);
          m_DataWords[1] += (lc<<16);
          break;
        case 2:
          m_DataWords[2] += et;
          m_DataWords[2] += (isol<<18);
          m_DataWords[2] += (lc<<16);
          break;
        case 3:
          m_DataWords[2] += (et<<8);
          m_DataWords[3] += (isol<<10);
          m_DataWords[3] += (lc<<8);
          break;
        case 4:
          m_DataWords[3] += et;
          m_DataWords[3] += (isol<<18);
          m_DataWords[3] += (lc<<16);
          break;
      }
      nTOB++;
     
  } // Loop over TOB locations
   
  // Finally set parity bits. Will assume we used odd parity here (flip initial assignment to change)
  for (unsigned int word = 0; word < 4; ++word) {
    unsigned int parity = 1;
    for (unsigned int bit = 0; bit < 24; ++bit) if ( ( (m_DataWords[word]>>bit) & 1) > 0 ) parity++;
    parity &= 1;
    m_DataWords[word] |= (parity<<23);
  }
  
}


CPMCMXData::~CPMCMXData()
{
}

/** Data access methods */

/** Crate number */
int LVL1::CPMCMXData::crate() const {
  return m_crate;
}

/** module number */
int LVL1::CPMCMXData::module() const {
  return m_module;
}

/** TOB type (EM/Tau) */
int LVL1::CPMCMXData::type() const {
  return m_type;
}

/** 16 bit presence map */
unsigned int LVL1::CPMCMXData::presenceMap() const {
  unsigned int value = (m_DataWords[0] & 0xffff);
  return value;
}

/** the 4 raw backplane data words */
std::vector<unsigned int> LVL1::CPMCMXData::DataWords() const {
  return m_DataWords;
}


/** Locations in Presence Map for up to 5 TOBs */
std::vector<unsigned int> LVL1::CPMCMXData::TOBPresenceBits() const {

  std::vector<unsigned int> bits;
  
  int ntob = 0;
  for (unsigned int i = 0; i < 16; ++i) {
    
    if ( (m_DataWords[0] & (1<<i)) > 0 ) {
      
      if (ntob < 5) bits.push_back(i);
      ntob++;
      
    } // bit set in presence map
    
  } // step through presence map
  
  return bits;
}


/** Data words (2b LC + 5b Isol + 8b ET) for up to 5 TOBs */
std::vector<unsigned int> LVL1::CPMCMXData::TOBWords() const {
  
  std::vector<unsigned int> data;

  /// If PresenceMap empty just return
  if ( (m_DataWords[0] & 0xffff) == 0 ) return data;
  
  /// Otherwise decode data words
  unsigned int ntob = 0;
  for (unsigned int i = 0; i < 16; ++i) {
    
    if ( (m_DataWords[0] & (1<<i)) > 0 ) {
      
      if (ntob < 5) {
        
        unsigned int word = 0;
        unsigned int coord = 0;
        unsigned int isol = 0;
        unsigned int et = 0;
      
        switch (ntob) {
          case 0:
            coord = (m_DataWords[0] >> 16) & 3;
            isol  = (m_DataWords[0] >> 18) & 0x1f;
            et    =  m_DataWords[1] & 0xff;
            break;
          case 1:
            coord = (m_DataWords[1] >> 16) & 3;
            isol  = (m_DataWords[1] >> 18) & 0x1f;
            et    = (m_DataWords[1] >> 8) & 0xff;
            break;
          case 2:
            coord = (m_DataWords[2] >> 16) & 3;
            isol  = (m_DataWords[2] >> 18) & 0x1f;
            et    =  m_DataWords[2] & 0xff;
            break;
          case 3:
            coord = (m_DataWords[3] >> 8) & 3;
            isol  = (m_DataWords[3] >> 10) & 0x1f;
            et    = (m_DataWords[2] >> 8) & 0xff;
            break;
          case 4:
            coord = (m_DataWords[3] >> 16) & 3;
            isol  = (m_DataWords[3] >> 18) & 0x1f;
            et    =  m_DataWords[3] & 0xff;
            break;
        }
        word = et + (isol<<8) + (coord<<13);
        data.push_back(word);
      }
      ntob++;
      
    } // bit set in presence map
    
  } // step through presence map
  
  return data;
}


/** L1Topo TOB words (1bRO + 4b CPM + 3b Chip + 3b LC + 5b Isol + 8b ET) for up to 5 TOBs */
std::vector<unsigned int> LVL1::CPMCMXData::TopoTOBs() const {

  std::vector<unsigned int> data;

  /// If PresenceMap empty just return
  if ( (m_DataWords[0] & 0xffff) == 0 ) return data;
  
  /// Otherwise decode data words
  unsigned int ntob = 0;
  for (unsigned int i = 0; i < 16; ++i) {
    
    if ( (m_DataWords[0] & (1<<i)) > 0 ) {
      
      if (ntob < 5) {
        
        unsigned int chip = i/2;
        unsigned int rl   = (i&1) << 2;
        
        unsigned int word;
        
        unsigned int coord = 0;
        unsigned int isol = 0;
        unsigned int et = 0;
      
        switch (ntob) {
          case 0:
            coord = ((m_DataWords[0] >> 16) & 3) + rl;
            isol  =  (m_DataWords[0] >> 18) & 0x1f;
            et    =   m_DataWords[1] & 0xff;
            break;
          case 1:
            coord = ((m_DataWords[1] >> 16) & 3) + rl;
            isol  =  (m_DataWords[1] >> 18) & 0x1f;
            et    =  (m_DataWords[1] >> 8) & 0xff;
            break;
          case 2:
            coord = ((m_DataWords[2] >> 16) & 3) + rl;
            isol  =  (m_DataWords[2] >> 18) & 0x1f;
            et    =   m_DataWords[2] & 0xff;
            break;
          case 3:
            coord = ((m_DataWords[3] >> 8) & 3) + rl;
            isol  =  (m_DataWords[3] >> 10) & 0x1f;
            et    =  (m_DataWords[2] >> 8) & 0xff;
            break;
          case 4:
            coord = ((m_DataWords[3] >> 16) & 3) + rl;
            isol  =  (m_DataWords[3] >> 18) & 0x1f;
            et    =   m_DataWords[3] & 0xff;
            break;
        }
        word = et + (isol<<8) + (coord<<13) + (chip<<16) + (m_module<<19);
        data.push_back(word);
      }
      ntob++;
      
    } // bit set in presence map
    
  } // step through presence map
  
  return data;
}


/** Report whether TOB overflow occurred */
bool LVL1::CPMCMXData::overflow() const {

  bool overflow = false;

  /// Don't waste time if PresenceMap empty 
  if ( (m_DataWords[0] & 0xffff) == 0 ) return overflow;
  
  /// Otherwise count non-zero bits in presence map
  int ntob = 0;
  for (unsigned int i = 0; i < 16; ++i) {
    
    if ( (m_DataWords[0] & (1<<i)) > 0 ) ntob++;
      
  } // step through presence map
  
  // Set overflow flags if needed
  if (ntob > 5) overflow = true;
  
  return overflow;
}


} // end namespace
