/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTREADOUT_H
#define MUONBYTESTREAM_MDTREADOUT_H

#include <stdint.h>

// Base class for Mdt readout decoding
// adapted to Mdt from RpcReadOut 

class MdtReadOut
{
  
 public:
  
  MdtReadOut();
  ~MdtReadOut();

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

