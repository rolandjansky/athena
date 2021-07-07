/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_RawEvent/LUCID_RawData.h"

LUCID_RawData::LUCID_RawData():
  m_word0 (0),
  m_word1 (0),
  m_word2 (0),
  m_word3 (0),
  m_word0p(0),
  m_word1p(0),
  m_word2p(0),
  m_word3p(0),
  m_word0n(0),
  m_word1n(0),
  m_word2n(0),
  m_word3n(0),
  m_status(0)
{}

LUCID_RawData::LUCID_RawData(const unsigned int word0, 
			     const unsigned int word1, 
 			     const unsigned int word2,
 			     const unsigned int word3,
			     const unsigned int word0p, 
			     const unsigned int word1p, 
 			     const unsigned int word2p,
 			     const unsigned int word3p,
			     const unsigned int word0n, 
			     const unsigned int word1n, 
 			     const unsigned int word2n,
 			     const unsigned int word3n,
 			     const unsigned int status) :
  m_word0 (word0),
  m_word1 (word1),
  m_word2 (word2),
  m_word3 (word3),
  m_word0p(word0p),
  m_word1p(word1p),
  m_word2p(word2p),
  m_word3p(word3p),
  m_word0n(word0n),
  m_word1n(word1n),
  m_word2n(word2n),
  m_word3n(word3n),
  m_status(status) 
{}

LUCID_RawData::LUCID_RawData(std::vector<uint32_t>& data_block) {
  
  m_word0p = 0;
  m_word1p = 0;
  m_word2p = 0;
  m_word3p = 0;
  
  m_word0  = 0;
  m_word1  = 0;
  m_word2  = 0;
  m_word3  = 0;
  
  m_word0n = 0;
  m_word1n = 0;
  m_word2n = 0;
  m_word3n = 0;

  m_status = 0; 
  
  unsigned int size = data_block.size();
  
  if (size == 5) {

    m_word0  = data_block[0];
    m_word1  = data_block[1];
    m_word2  = data_block[2];
    m_word3  = data_block[3];

    m_word0p = 0;
    m_word1p = 0;
    m_word2p = 0;
    m_word3p = 0;

    m_word0n = 0; 
    m_word1n = 0;
    m_word2n = 0;
    m_word3n = 0;

    m_status = data_block[4]; 
  }
  else if (size == 13) {

    m_word0p = data_block[0];
    m_word1p = data_block[1];
    m_word2p = data_block[2];
    m_word3p = data_block[3];

    m_word0  = data_block[4];
    m_word1  = data_block[5];
    m_word2  = data_block[6];
    m_word3  = data_block[7];

    m_word0n = data_block[8];
    m_word1n = data_block[9];
    m_word2n = data_block[10];
    m_word3n = data_block[11];

    m_status = data_block[12]; 
  }
  else std::cout << " ERROR! LUCID_RawData::LUCID_RawData, unexpected datablock size: " << size << std::endl;
}

LUCID_RawData::~LUCID_RawData() {}

bool LUCID_RawData::isTubeFired(unsigned int tubeNumber, unsigned int bxID) const {

  bool isTubeFired = 0;
  
  if (tubeNumber < 16) { // word0

    int bit = tubeNumber;
    
    if      (bxID == 0) isTubeFired = (m_word0 >>bit)&1;
    else if (bxID == 1) isTubeFired = (m_word0p>>bit)&1;
    else if (bxID == 2) isTubeFired = (m_word0n>>bit)&1;
    else { std::cout << " ERROR! LUCID_RawData::isTubeFired, unknown operation for bxID: " << bxID << std::endl; return 0; }
  }
  else if (tubeNumber < 20) { // word2

    int bit = tubeNumber - 16;
    
    if      (bxID == 0) isTubeFired = (m_word2 >>bit)&1;
    else if (bxID == 1) isTubeFired = (m_word2p>>bit)&1;
    else if (bxID == 2) isTubeFired = (m_word2n>>bit)&1;
    else { std::cout << " ERROR! LUCID_RawData::isTubeFired, unknown operation for bxID: " << bxID << std::endl; return 0; }
  }
  else if (tubeNumber < 36) { // word1

    int bit = tubeNumber - 20;
    
    if      (bxID == 0) isTubeFired = (m_word1 >>bit)&1;
    else if (bxID == 1) isTubeFired = (m_word1p>>bit)&1;
    else if (bxID == 2) isTubeFired = (m_word1n>>bit)&1;
    else { std::cout << " ERROR! LUCID_RawData::isTubeFired, unknown operation for bxID: " << bxID << std::endl; return 0; }
  }
  else if (tubeNumber < 40) { // word3
    
    int bit = tubeNumber - 36;

    if      (bxID == 0) isTubeFired = (m_word3 >>bit)&1;
    else if (bxID == 1) isTubeFired = (m_word3p>>bit)&1;
    else if (bxID == 2) isTubeFired = (m_word3n>>bit)&1;
    else { std::cout << " ERROR! LUCID_RawData::isTubeFired, unknown operation for bxID: " << bxID << std::endl; return 0; }
  }
  else { std::cout << " ERROR! LUCID_RawData::isTubeFired, unknown operation for tubeNumber: " << tubeNumber << std::endl; return 0; }

  return isTubeFired;
}

int LUCID_RawData::getLumatBitPosition(unsigned int tubeNumber) {

  if      (tubeNumber ==  0) return 13; // word0
  else if (tubeNumber ==  1) return  0;
  else if (tubeNumber ==  2) return  1;
  else if (tubeNumber ==  3) return  5;
  else if (tubeNumber ==  4) return  6;
  else if (tubeNumber ==  5) return  8;
  else if (tubeNumber ==  6) return  9;
  else if (tubeNumber ==  7) return 14;
  else if (tubeNumber ==  8) return 15;
  else if (tubeNumber ==  9) return  2;
  else if (tubeNumber == 10) return  3;
  else if (tubeNumber == 11) return  4;
  else if (tubeNumber == 12) return  7;
  else if (tubeNumber == 13) return 10;
  else if (tubeNumber == 14) return 11;
  else if (tubeNumber == 15) return 12;
  else if (tubeNumber == 16) return  0; // word2
  else if (tubeNumber == 17) return  1;
  else if (tubeNumber == 18) return  2;
  else if (tubeNumber == 19) return  3;
  else if (tubeNumber == 20) return 21; // word1
  else if (tubeNumber == 21) return 16;
  else if (tubeNumber == 22) return 17;
  else if (tubeNumber == 23) return 13;
  else if (tubeNumber == 24) return 14;
  else if (tubeNumber == 25) return  8;
  else if (tubeNumber == 26) return  9;
  else if (tubeNumber == 27) return 22;
  else if (tubeNumber == 28) return 23;
  else if (tubeNumber == 29) return 18;
  else if (tubeNumber == 30) return 19;
  else if (tubeNumber == 31) return 20;
  else if (tubeNumber == 32) return 15;
  else if (tubeNumber == 33) return 10;
  else if (tubeNumber == 34) return 11;
  else if (tubeNumber == 35) return 12;
  else if (tubeNumber == 36) return  0; // word3 
  else if (tubeNumber == 37) return  1; 
  else if (tubeNumber == 38) return  2; 
  else if (tubeNumber == 39) return  3; 
  else { std::cout << " ERROR! LUCID_RawData::getLumatBitPosition, unknown operation for tubeNumber: " << tubeNumber << std::endl; return -1; }
}

void LUCID_RawData::decodeLumatMapping() {
 
  uint32_t data_word0  = 0;
  uint32_t data_word1  = 0;
  uint32_t data_word2  = 0;
  uint32_t data_word3  = 0;
  uint32_t data_word0p = 0;
  uint32_t data_word1p = 0;
  uint32_t data_word2p = 0;
  uint32_t data_word3p = 0;
  uint32_t data_word0n = 0;
  uint32_t data_word1n = 0;
  uint32_t data_word2n = 0;
  uint32_t data_word3n = 0;

  int hitcounter0  = 0;
  int hitcounter1  = 0;
  int hitcounter2  = 0;
  int hitcounter3  = 0;
  int hitcounter0p = 0;
  int hitcounter1p = 0;
  int hitcounter2p = 0;
  int hitcounter3p = 0;
  int hitcounter0n = 0;
  int hitcounter1n = 0;
  int hitcounter2n = 0;
  int hitcounter3n = 0;

  for (int tubeID=0; tubeID<40; tubeID++) {

    bool isHit = 0;

    int bit = getLumatBitPosition(tubeID);

    if (tubeID < 16) {
    
      isHit = (m_word0 >>bit)&1; if (isHit) { data_word0  |= (1 << tubeID); hitcounter0++; }
      isHit = (m_word0p>>bit)&1; if (isHit) { data_word0p |= (1 << tubeID); hitcounter0p++; }
      isHit = (m_word0n>>bit)&1; if (isHit) { data_word0n |= (1 << tubeID); hitcounter0n++; }
    }
    else if (tubeID < 20) {
      
      isHit = (m_word2 >>bit)&1; if (isHit) { data_word2  |= (1 << (tubeID-16)); hitcounter2++; }
      isHit = (m_word2p>>bit)&1; if (isHit) { data_word2p |= (1 << (tubeID-16)); hitcounter2p++; }
      isHit = (m_word2n>>bit)&1; if (isHit) { data_word2n |= (1 << (tubeID-16)); hitcounter2n++; }
    }
    else if (tubeID < 36) {

      isHit = (m_word1 >>bit)&1; if (isHit) { data_word1  |= (1 << (tubeID-20)); hitcounter1++; }
      isHit = (m_word1p>>bit)&1; if (isHit) { data_word1p |= (1 << (tubeID-20)); hitcounter1p++; }
      isHit = (m_word1n>>bit)&1; if (isHit) { data_word1n |= (1 << (tubeID-20)); hitcounter1n++; }
    }
    else {
      
      isHit = (m_word3 >>bit)&1; if (isHit) { data_word3  |= (1 << (tubeID-36)); hitcounter3++; }
      isHit = (m_word3p>>bit)&1; if (isHit) { data_word3p |= (1 << (tubeID-36)); hitcounter3p++; }
      isHit = (m_word3n>>bit)&1; if (isHit) { data_word3n |= (1 << (tubeID-36)); hitcounter3n++; }
    }
  }
  
  setWord0(data_word0 |= (hitcounter0 << 24));
  setWord1(data_word1 |= (hitcounter1 << 24));
  setWord2(data_word2 |= (hitcounter2 << 24));
  setWord3(data_word3 |= (hitcounter3 << 24));

  setWord0p(data_word0p |= (hitcounter0p << 24));
  setWord1p(data_word1p |= (hitcounter1p << 24));
  setWord2p(data_word2p |= (hitcounter2p << 24));
  setWord3p(data_word3p |= (hitcounter3p << 24));

  setWord0n(data_word0n |= (hitcounter0n << 24));
  setWord1n(data_word1n |= (hitcounter1n << 24));
  setWord2n(data_word2n |= (hitcounter2n << 24));
  setWord3n(data_word3n |= (hitcounter3n << 24));
}

void LUCID_RawData::encodeLumatMapping() {

  uint32_t data_word0 = 0;
  uint32_t data_word1 = 0;
  uint32_t data_word2 = 0;
  uint32_t data_word3 = 0;
  
  int hitcounter0 = 0;
  int hitcounter1 = 0;
  int hitcounter2 = 0;
  int hitcounter3 = 0;
  
  std::cout << std::hex
	    << " m_word0: " << m_word0 << std::endl
	    << " m_word1: " << m_word1 << std::endl
	    << " m_word2: " << m_word2 << std::endl
	    << " m_word3: " << m_word3 << std::endl
	    << std::dec;

  for (int tubeID=0; tubeID<40; tubeID++) {

    if (!isTubeFired(tubeID, 0)) continue;

    int bit = getLumatBitPosition(tubeID);
    
    if      (tubeID < 16) { data_word0 |= (1 << bit); hitcounter0++; }
    else if (tubeID < 20) { data_word2 |= (1 << bit); hitcounter2++; }
    else if (tubeID < 36) { data_word1 |= (1 << bit); hitcounter1++; }
    else if (tubeID < 40) { data_word3 |= (1 << bit); hitcounter3++; }
  }
  
  std::cout << " hitcounter0: " << std::setw(10) << hitcounter0 << std::endl
	    << " hitcounter1: " << std::setw(10) << hitcounter1 << std::endl
	    << " hitcounter2: " << std::setw(10) << hitcounter2 << std::endl
	    << " hitcounter3: " << std::setw(10) << hitcounter3 << std::endl;

  data_word0 |= (hitcounter0 << 24);
  data_word1 |= (hitcounter1 << 24);
  data_word2 |= (hitcounter2 << 24);
  data_word3 |= (hitcounter3 << 24);
  
  std::cout << std::hex
	    << " data_word0: " << data_word0 << std::endl
	    << " data_word1: " << data_word1 << std::endl
	    << " data_word2: " << data_word2 << std::endl
	    << " data_word3: " << data_word3 << std::endl
	    << std::dec;
    
  setWord0(data_word0);
  setWord1(data_word1);
  setWord2(data_word2);
  setWord3(data_word3);
}
