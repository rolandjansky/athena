/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFAREADOUT_H
#define ALFAREADOUT_H

#include <stdint.h>
#include <utility>

// Base class for ALFA readout decoding

class ALFA_ReadOut
{
  
 public:
  
  ALFA_ReadOut();

 protected:
  constexpr std::pair<uint32_t, uint16_t> getBitsWord(const uint16_t bstart, const uint16_t bstop){
     uint32_t word = 0;
     for (uint16_t i = bstop; i<bstart ; i++  ) 
       word = ((word | 1)<<1);
     return std::make_pair(word, bstop);
  }

  // Decode a section of a 32-bits data word
  uint32_t getBits(std::pair<uint32_t, uint16_t> in){
    return (m_word >> in.second) & (in.first|1);
  }

  // Encode a 32-bits data word from fragments with size < 16 bits
  uint32_t setBits(uint16_t nData, uint16_t* inputData, 
		   uint16_t* inputPos); 

  // Encode a 32-bits data word from fragments with size < 32 bits  
  uint32_t setBits(uint16_t nData, uint32_t* inputData, uint16_t* inputPos); 

  uint32_t m_word;

};

#endif

