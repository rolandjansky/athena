/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFAREADOUT_H
#define ALFAREADOUT_H

#include <stdint.h>

// Base class for ALFA readout decoding

class ALFA_ReadOut
{
  
 public:
  
  ALFA_ReadOut();
  ~ALFA_ReadOut();

 protected:
  // Decode a section of a 32-bits data word
  uint32_t getBits(const uint16_t bstart, const uint16_t bstop);

  // Encode a 32-bits data word from fragments with size < 16 bits
  uint32_t setBits(uint16_t nData, uint16_t* inputData, 
		   uint16_t* inputPos); 

  // Encode a 32-bits data word from fragments with size < 32 bits  
  uint32_t setBits(uint16_t nData, uint32_t* inputData, uint16_t* inputPos); 

  uint32_t m_word;

};

#endif

