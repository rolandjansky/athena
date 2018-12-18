/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/EnergyTopoData.h"

namespace LVL1 {

// Default constructor
EnergyTopoData::EnergyTopoData() : m_word0(0), m_word1(0), m_word2(0)
{
}

// Construct from pre-calculated data words
EnergyTopoData::EnergyTopoData(unsigned int word0, unsigned int word1, unsigned int word2) :
  m_word0(word0),
  m_word1(word1),
  m_word2(word2)
{
}

/** Adding to or modifying data */

void EnergyTopoData::addEx(unsigned int Ex, unsigned int overflow, int type) {
   unsigned int word = (Ex&0x7fff) + (overflow<<15);
   
   if (type == LVL1::EnergyTopoData::Normal) 
      m_word0 = (m_word0&0xffff0000) + (word&0xffff);
   else if (type == LVL1::EnergyTopoData::Restricted) 
      m_word0 = (m_word0&0xffff) + ( (word&0xffff)<<16 );      
}

void EnergyTopoData::addEy(unsigned int Ey, unsigned int overflow, int type) {
   unsigned int word = (Ey&0x7fff) + (overflow<<15);
   
   if (type == LVL1::EnergyTopoData::Normal) 
      m_word1 = (m_word1&0xffff0000) + (word&0xffff);
   else if (type == LVL1::EnergyTopoData::Restricted) 
      m_word1 = (m_word1&0xffff) + ( (word&0xffff)<<16 );      
}

void EnergyTopoData::addEt(unsigned int Et, unsigned int overflow, int type) {
   unsigned int word = (Et&0x7fff) + (overflow<<15);
   
   if (type == LVL1::EnergyTopoData::Normal) 
      m_word2 = (m_word2&0xffff0000) + (word&0xffff);
   else if (type == LVL1::EnergyTopoData::Restricted) 
      m_word2 = (m_word2&0xffff) + ( (word&0xffff)<<16 );      
}

void EnergyTopoData::addRoI(uint32_t roiWord) {
   uint8_t header = (roiWord>>28);
   uint8_t type   = (roiWord>>26)&1;
   uint32_t payload = (roiWord&0xffff);
   
   if (header == 4) {
      if (type == LVL1::EnergyTopoData::Normal)          m_word0 = (m_word0&0xffff0000) + payload;
      else if (type == LVL1::EnergyTopoData::Restricted) m_word0 = (m_word0&0xffff) + (payload<<16);
   }
   else if (header == 6) {
      if (type == LVL1::EnergyTopoData::Normal)          m_word1 = (m_word1&0xffff0000) + payload;
      else if (type == LVL1::EnergyTopoData::Restricted) m_word1 = (m_word1&0xffff) + (payload<<16);
   }
   else if (header == 5) {
      if (type == LVL1::EnergyTopoData::Normal)          m_word2 = (m_word2&0xffff0000) + payload;
      else if (type == LVL1::EnergyTopoData::Restricted) m_word2 = (m_word2&0xffff) + (payload<<16);
   }
}

void EnergyTopoData::addRoIs(const std::vector<uint32_t>& roiWords) {

  for (std::vector<uint32_t>::const_iterator it = roiWords.begin(); it != roiWords.end(); ++it) addRoI((*it)) ;

}

/** Data access methods */

/** Ex (signed) */
int EnergyTopoData::Ex( SumTypes type ) const {

  int value = 0;
  if      (type == EnergyTopoData::Normal)     value = m_word0 & 0x7fff;
  else if (type == EnergyTopoData::Restricted) value = (m_word0>>16) & 0x7fff;
  
  return decodeTC(value);
}

/** Ey (signed) */
int EnergyTopoData::Ey( SumTypes type ) const {

  int value = 0;
  if      (type == EnergyTopoData::Normal)     value = m_word1 & 0x7fff;
  else if (type == EnergyTopoData::Restricted) value = (m_word1>>16) & 0x7fff;
  
  return decodeTC(value);
}

/** ET Sum */
int EnergyTopoData::Et( SumTypes type ) const {

  int value = 0;
  if      (type == EnergyTopoData::Normal)     value = m_word2 & 0x7fff;
  else if (type == EnergyTopoData::Restricted) value = (m_word2>>16) & 0x7fff;
  
  return value;
}

/** Ex (twos complement) */
int EnergyTopoData::ExTC( SumTypes type ) const {

  int value = 0;
  if      (type == EnergyTopoData::Normal)     value = m_word0 & 0x7fff;
  else if (type == EnergyTopoData::Restricted) value = (m_word0>>16) & 0x7fff;
  
  return value;
}

/** Ey (twos complement) */
int EnergyTopoData::EyTC( SumTypes type ) const {

  int value = 0;
  if      (type == EnergyTopoData::Normal)     value = m_word1 & 0x7fff;
  else if (type == EnergyTopoData::Restricted) value = (m_word1>>16) & 0x7fff;
  
  return value;
}


/** Overflow flags */
unsigned int EnergyTopoData::ExOverflow( SumTypes type ) const {
  unsigned int overflow = 0; 
  if      (type == EnergyTopoData::Normal)     overflow = (m_word0 >> 15) & 1;
  else if (type == EnergyTopoData::Restricted) overflow = (m_word0 >> 31) & 1; 
  return overflow = 0;
}
  
unsigned int EnergyTopoData::EyOverflow( SumTypes type ) const {
  unsigned int overflow = 0; 
  if      (type == EnergyTopoData::Normal)     overflow = (m_word1 >> 15) & 1;
  else if (type == EnergyTopoData::Restricted) overflow = (m_word1 >> 31) & 1; 
  return overflow = 0;
}

unsigned int EnergyTopoData::EtOverflow( SumTypes type ) const {
  unsigned int overflow = 0; 
  if      (type == EnergyTopoData::Normal)     overflow = (m_word2 >> 15) & 1;
  else if (type == EnergyTopoData::Restricted) overflow = (m_word2 >> 31) & 1; 
  return overflow = 0;
}

/** Decode 15-bit twos-complement values */
int EnergyTopoData::decodeTC( unsigned int word ) const {
   
  word = word & 0x7fff; /// Should be redundant
  int value = word;
  
  int sign  = (word >> 14) & 1;
  if (sign != 0) {
    int complement = ~word;
    value = -( (complement+1) & 0x7fff );
  }
  
  return value;
}

} // end namespace
