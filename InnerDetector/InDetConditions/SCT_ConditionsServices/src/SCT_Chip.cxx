/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Chip.h"

#include <sstream>
#include <iostream>

// Default constructor: chip has id 0, config = 0 and all strips good.
SCT_Chip::SCT_Chip():
  m_id(0),
  m_config(0),m_in(0),m_out(0),m_end(false),m_master(false) {
  m_mask.set();
}

// Construct a chip from the id, config and masks
SCT_Chip::SCT_Chip(short id, short config, uint32_t mask0, uint32_t mask1, uint32_t mask2, uint32_t mask3):
  m_id(id),
  m_config(config) {
  m_in     = m_config.test(IN_BIT) ? 1 : 0;
  m_out    = m_config.test(OUT_BIT) ? 1 : 0;
  m_end    = m_config.test(END_BIT);
  m_master = !m_config.test(MASTER_BIT);        // Master bit is 0 if master
  initializeMaskFromInts(mask0, mask1, mask2, mask3);
}

// Initialise chip mask (int version)
bool SCT_Chip::initializeMaskFromInts(uint32_t mask0, uint32_t mask1, uint32_t mask2, uint32_t mask3){
  bool successfulInitialization(true);
  uint32_t subWords[nSubwords]={mask0,mask1,mask2,mask3};
  // Put the integers into 32 bit bitsets
  std::bitset<lenSubword> subBinary;
  for(unsigned int i{0}; i!=nSubwords; ++i) {
    subBinary = subWords[i];
    // Put the four bitsets together in one 128 bit bitset (private member data)
    for(unsigned int j{0}; j!=lenSubword; ++j) {
      m_mask[i*lenSubword+j]=subBinary[j];
    }
  }
  return successfulInitialization;
}

// Initialise chip mask from string in format "0xffffffff 0xffffffff 0xffffffff 0xffffffff"
// with the rightmost nibble being the least significant
bool SCT_Chip::initializeMaskFromString(const std::string &maskString){
  bool successfulInitialization(true);
  std::istringstream formatStream(maskString);
  uint32_t  subWords[nSubwords];

  // Parse into four 32-bit integers (there must be a better way!)
  // As a very basic check, we enable exceptions on the hex conversion.
  formatStream.exceptions(std::ios_base::badbit|std::ios_base::failbit);

  try{
    formatStream >> std::hex >> subWords[nSubwords-1] 
                 >> std::hex >> subWords[nSubwords-2] 
                 >> std::hex >> subWords[nSubwords-3] 
                 >> std::hex >> subWords[nSubwords-4];
  } catch(std::ios_base::failure) {
    std::cerr << "The SCT_ChipMask code has failed to convert the received string to a mask; the received string is: ";
    std::cerr << maskString << std::endl;
    successfulInitialization = false;
  }

  initializeMaskFromInts(subWords[0], subWords[1], subWords[2], subWords[3]);
  return successfulInitialization;
}

// Check if channel is masked
bool SCT_Chip::channelIsMasked(const unsigned int channelNumber) const{
  return !(m_mask.test(channelNumber));
}

// Number of masked channels
unsigned int SCT_Chip::numberOfMaskedChannels() const {
  return nBitsMask - m_mask.count();
}

// Add masked channels to the bad strip vector
void SCT_Chip::appendBadStripsToVector(std::vector<int> & maskedChannelVector) const{  
  for(unsigned int thisChann(0);thisChann != nBitsMask; ++thisChann){
    if (channelIsMasked(thisChann)) maskedChannelVector.push_back(thisChann);
  }
}

