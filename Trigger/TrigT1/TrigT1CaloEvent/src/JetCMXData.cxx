/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/JetCMXData.h"

namespace LVL1 {

// Default constructor
JetCMXData::JetCMXData() : m_crate(0), m_module(0)
{
   m_DataWords.clear();
   m_DataWords.resize(4);
}

// Construct from pre-calculated data
JetCMXData::JetCMXData(int crate, int module,
                       const std::vector<unsigned int>& tobWords) :
  m_crate(crate),
  m_module(module),
  m_DataWords(tobWords)
{ 
}

// Construct from a vector of CPMTobRoIs
JetCMXData::JetCMXData(int crate, int module, 
                       const DataVector<JEMTobRoI>* tobs) :
  m_crate(crate),
  m_module(module)
{
    
  m_DataWords.clear();
  m_DataWords.resize(4);
   
  /** Step through vector of RoIs. Find those whose crate, module, type match
   *  To make backplane data words we need to geographically sort TOBs        */
   
  std::vector<unsigned int> allTOBs(8,0);
   
  DataVector<JEMTobRoI>::const_iterator itRoI = tobs->begin();
  for ( ; itRoI != tobs->end(); ++itRoI) {
    if ( (*itRoI)->crate() != crate || (*itRoI)->jem() != module ) continue;
      
    int frame     = (*itRoI)->frame();
    int lc        = (*itRoI)->location();
    int etL       = (*itRoI)->energyLarge();
    int etS       = (*itRoI)->energySmall();
      
    // Set flag in presence map
    m_DataWords[0] |= ( 1 << frame );
      
    // Store TOB data to fill backplane words later
    unsigned int tobdata  = etL + (etS<<10) + (lc<<19);
    allTOBs[frame] = tobdata;     
  }
   
  /** Now step though identified TOBs in correct geographical order and fill backplane words */
  int nTOB = 0;
  for (int i = 0; i < 8; ++i) {
    if (allTOBs[i] == 0 ) continue;
     
      unsigned int etL =  allTOBs[i] & 0x3ff;
      unsigned int etS = (allTOBs[i] >> 10) & 0x1ff;
      unsigned int lc  = (allTOBs[i] >> 19) & 3;
      
      switch (nTOB) {
        case 0:
          m_DataWords[0] += ( etL << 13 );
          m_DataWords[0] += ( (etS & 7) << 8);
          m_DataWords[1] += ( etS >> 3 );
          m_DataWords[0] += ( lc << 11 );
          break;
        case 1:
          m_DataWords[1] += ( etL << 13 );
          m_DataWords[1] += ( (etS & 0x1f) << 6);
          m_DataWords[2] += ( etS >> 5 );
          m_DataWords[1] += ( lc << 11 );
          break;
        case 2:
          m_DataWords[2] += ( etL << 13 );
          m_DataWords[2] += ( (etS & 0x7f) << 4);
          m_DataWords[3] += ( etS >> 7 );
          m_DataWords[2] += ( lc << 11 );
          break;
        case 3:
          m_DataWords[3] += ( etL << 13 );
          m_DataWords[3] += ( etS << 2);
          m_DataWords[3] += ( lc << 11 );
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


JetCMXData::~JetCMXData()
{
}


/** Data access methods */

/** Crate number */
int LVL1::JetCMXData::crate() const {
  return m_crate;
}

/** module number */
int LVL1::JetCMXData::module() const {
  return m_module;
}


/** 8 bit presence map */
unsigned int LVL1::JetCMXData::presenceMap() const {
  unsigned int value = (m_DataWords[0] & 0xff);
  return value;
}

/** Report whether TOB overflow occurred */
bool LVL1::JetCMXData::overflow() const {

  bool overflow = false;

  /// Don't waste time if PresenceMap empty 
  if ( (m_DataWords[0] & 0xff) == 0 ) return overflow;
  
  /// Otherwise count non-zero bits in presence map
  int ntob = 0;
  for (unsigned int i = 0; i < 8; ++i) {
    
    if ( (m_DataWords[0] & (1<<i)) > 0 ) ntob++;
      
  } // step through presence map
  
  // Set overflow flags if needed
  if (ntob > 4) overflow = true;
  
  return overflow;
}

/** the 4 raw backplane data words */
std::vector<unsigned int> LVL1::JetCMXData::DataWords() const {
  return m_DataWords;
}

/** Locations in Presence Map for up to 4 TOBs */
std::vector<unsigned int> LVL1::JetCMXData::TOBPresenceBits() const {

  std::vector<unsigned int> bits;
  
  int ntob = 0;
  for (unsigned int i = 0; i < 8; ++i) {
    
    if ( (m_DataWords[0] & (1<<i)) > 0 ) {
      
      if (ntob < 4) bits.push_back(i);
      ntob++;
      
    } // bit set in presence map
    
  } // step through presence map
  
  return bits;
}


/** Data words (2b LC + 9b Small ET + 10b Large ET) for up to 4 TOBs */
std::vector<unsigned int> LVL1::JetCMXData::TOBWords() const {
  
  std::vector<unsigned int> data;

  /// If PresenceMap empty just return
  if ( (m_DataWords[0] & 0xff) == 0 ) return data;
  
  /// Otherwise decode data words
  unsigned int ntob = 0;
  for (unsigned int i = 0; i < 8; ++i) {
    
    if ( (m_DataWords[0] & (1<<i)) > 0 ) {
      
      if (ntob < 4) {
        
        unsigned int word = 0;
        unsigned int coord = 0;
        unsigned int etSmall = 0;
        unsigned int etLarge = 0;
      
        switch (ntob) {
          case 0:
            coord   =  (m_DataWords[0] >> 11) & 0x3;
            etSmall = ((m_DataWords[0] >> 8)  & 0x7) + ((m_DataWords[1] & 0x3f) << 3);
            etLarge =  (m_DataWords[0] >> 13) & 0x3ff;
            break;
          case 1:
            coord   =  (m_DataWords[1] >> 11) & 0x3;
            etSmall = ((m_DataWords[1] >> 6)  & 0x1f) + ((m_DataWords[2] & 0xf) << 5);
            etLarge =  (m_DataWords[1] >> 13) & 0x3ff;
            break;
          case 2:
            coord   =  (m_DataWords[2] >> 11) & 0x3;
            etSmall = ((m_DataWords[2] >> 4)  & 0x7f) + ((m_DataWords[3] & 0x3) << 7);
            etLarge =  (m_DataWords[2] >> 13) & 0x3ff;
            break;
          case 3:
            coord   =  (m_DataWords[3] >> 11) & 0x3;
            etSmall =  (m_DataWords[3] >> 2)  & 0x1ff;
            etLarge =  (m_DataWords[3] >> 13) & 0x3ff;
            break;
        }

        word = etLarge + (etSmall<<10) + (coord<<19);
        data.push_back(word);
      }
      ntob++;
      
    } // bit set in presence map
    
  } // step through presence map
  
  return data;
}


/** L1Topo TOB words (1bRO + 4b JEM + 3b Frame + 2b LC + 9b Small ET + 10b Large ET) for up to 4 TOBs */
std::vector<unsigned int> LVL1::JetCMXData::TopoTOBs() const {

  std::vector<unsigned int> data;

  /// If PresenceMap empty just return
  if ( (m_DataWords[0] & 0xff) == 0 ) return data;
  
  /// Otherwise decode data words
  unsigned int ntob = 0;
  for (unsigned int i = 0; i < 8; ++i) {
    
    if ( (m_DataWords[0] & (1<<i)) > 0 ) {
      
      if (ntob < 4) {
        
        unsigned int frame = i;
        
        unsigned int word;
        
        unsigned int coord = 0;
        unsigned int etSmall = 0;
        unsigned int etLarge = 0;
      
        switch (ntob) {
          case 0:
            coord   =  (m_DataWords[0] >> 11) & 0x3;
            etSmall = ((m_DataWords[0] >> 8)  & 0x7) + ((m_DataWords[1] & 0x3f) << 3);
            etLarge =  (m_DataWords[0] >> 13) & 0x3ff;
            break;
          case 1:
            coord   =  (m_DataWords[1] >> 11) & 0x3;
            etSmall = ((m_DataWords[1] >> 6)  & 0x1f) + ((m_DataWords[2] & 0xf) << 5);
            etLarge =  (m_DataWords[1] >> 13) & 0x3ff;
            break;
          case 2:
            coord   =  (m_DataWords[2] >> 11) & 0x3;
            etSmall = ((m_DataWords[2] >> 4)  & 0x7f) + ((m_DataWords[3] & 0x3) << 7);
            etLarge =  (m_DataWords[2] >> 13) & 0x3ff;
            break;
          case 3:
            coord   =  (m_DataWords[3] >> 11) & 0x3;
            etSmall =  (m_DataWords[3] >> 2)  & 0x1ff;
            etLarge =  (m_DataWords[3] >> 13) & 0x3ff;
            break;
        }
        
        word = etLarge + (etSmall<<10) + (coord<<19) + (frame<<21) + (m_module<<24);
        data.push_back(word);
      }
      ntob++;
      
    } // bit set in presence map
    
  } // step through presence map
  
  return data;
}


} // end namespace
