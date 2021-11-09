/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TileByteStream/TileDigits2Bytes.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileDigits.h"

#include <iostream>


int TileDigits2Bytes::getBytes(const TileDigits* digi, const TileHWID* tileHWID, std::vector<unsigned int>& v) {  // Salukvadze
  int chID = tileHWID->channel(digi->adc_HWID());
  int gain = (tileHWID->is_low_gain(digi->adc_HWID()) ? 0 : 1);
  std::vector<float> digits = digi->samples();
  unsigned int w;
  w = (gain << 15) + (digits.size() << 8) + chID;
  for (unsigned int i=0; i+1<digits.size(); i+=2) {
    w += (int)digits[i] << 16;
    v.push_back(w);
    w = (int)digits[i+1];
    }
  if (digits.size() % 2 != 0) {
    w += (int)digits[digits.size()-1] << 16;
    v.push_back(w);
  } else {
    v.push_back(w);
  }
  return digits.size() / 2 + 1;
      
}


std::array< std::vector<float>, 3 >
TileDigits2Bytes::getDigits(const uint32_t* data, int dataWordsPerChip) const {
  // create new containers on the heap
  std::array< std::vector<float>, 3 > digiVec;

  digiVec[0].reserve(dataWordsPerChip);
  digiVec[1].reserve(dataWordsPerChip);
  digiVec[2].reserve(dataWordsPerChip);

  for(int sampl=0;sampl<dataWordsPerChip;++sampl) {
    int word = (*data); ++data;
    digiVec[0].push_back((float)(word & 0x3ff));
    digiVec[1].push_back((float)((word>>10) & 0x3ff));
    digiVec[2].push_back((float)((word>>20) & 0x3ff));
  }
  return digiVec;
}

/**
 * Gain bits are bit 12-14 in chip header. Bit 12 corresponds to chan with lowest #.
 */
int 
TileDigits2Bytes::getGain(const uint32_t *data, int chan) const {
  return ((*data)>>(12+chan)) & 0x1;
}

/**
 * Go through all chip headers, check their parity
 * and extract BCID from them (bits 0-11)
 */
int 
TileDigits2Bytes::getBCID(const uint32_t *data, int chipCount, int wordsPerChip) const {
  int BCID = 0, n;
  int modeFound = 0;
  const uint32_t *wordPtr = data;

  for(n=0; modeFound<5 && n<chipCount;++n) {
    if(checkWordParity(wordPtr)) {
      BCID = (*wordPtr) & 0xfff;
      // this is correct if header has MSB 1 and next word has MSB 0
      if (((wordPtr[0]>>31) == 1) && ((wordPtr[1]>>31) == 0)) ++modeFound; 
    }
    wordPtr += wordsPerChip;
  }
  // if no valid mode found, take BCID from first chip
  if(0 == modeFound) BCID = (*data & 0xfff);
    
  return BCID;
}

/**
 * Go through all chip headers, check their parity
 * and extract digi mode from them (bit 15,16)
 */
int 
TileDigits2Bytes::getDigiMode(const uint32_t *data, int chipCount, int wordsPerChip) const {
  int digiMode = -1, n;
  bool modeFound = false;
  const uint32_t *wordPtr = data;

  for(n=0;!modeFound && n<chipCount;++n) {
    if(checkWordParity(wordPtr)) {
      digiMode = ((*wordPtr)>>15) & 0x3;
      // this is correct if header has MSB 1 and next word has MSB 0
      modeFound = ((wordPtr[0]>>31) == 1) && ((wordPtr[1]>>31) == 0); 
    }
    wordPtr += wordsPerChip;
  }
  // if no valid mode found, ignore parity
  if(!modeFound) {
    wordPtr = data;
    for(n=0;!modeFound && n<chipCount;++n) {
      digiMode = ((*wordPtr)>>15) & 0x3;
      // this is correct if header has MSB 1 and next word has MSB 0
      modeFound = ((wordPtr[0]>>31) == 1) && ((wordPtr[1]>>31) == 0); 
      wordPtr += wordsPerChip;
    }
  }
  // if still no valid mode found
  if(!modeFound) {
    if(m_verbose) std::cout<<"<TileD2B> No valid header found, assuming normal mode!"<<std::endl;
    digiMode = -1;
  }
  else if(m_verbose) {
    if(digiMode > 0) n <<= 1;
    if(checkWordParity(wordPtr)) std::cout<<"<TileD2B> DigiMode "<<digiMode<<" found in chip header "<<n<<std::endl;
    else std::cout<<"<TileD2B> DigiMode "<<digiMode<<" found in chip header "<<n<<" with BAD PARITY"<<std::endl;
    if(digiMode > 0) std::cout<<"<TileD2B> Calibration mode selected!"<<std::endl;
  }
  
  return digiMode;
}

inline bool 
TileDigits2Bytes::checkWordParity(const uint32_t *data) const {
  return (calculateParity(data) == 1);
} 

inline uint32_t 
TileDigits2Bytes::calculateParity(const uint32_t *data) const {
  uint32_t res = *data;
  res ^= ((*data) >> 1);
  res ^= (res >> 2);
  res ^= (res >> 4);
  res ^= (res >> 8);
  res ^= (res >> 16);
  
  return (res & 0x1);
}

inline uint32_t 
TileDigits2Bytes::checkParity(const uint32_t *data, int length) const {
  uint32_t result = 0, tmp;
  for(int n=0;n<length;++n) {
    tmp = data[n] ^ (data[n]>> 1);
    tmp ^= (tmp >> 2);
    tmp ^= (tmp >> 4);
    tmp ^= (tmp >> 8);
    tmp ^= (tmp >> 16);
    result |= (tmp & 0x1)<<n;
  }
  return result;
}

/**
 * MSB of datawords should is checked
 */
inline bool
TileDigits2Bytes::checkStartBit(const uint32_t *data, int length, uint32_t startbit) const {
  bool result = true;
  for(int i=0;result && (i<length);++i) {
    result = (data[i]>>31 == startbit);
  }
  return result;
}
