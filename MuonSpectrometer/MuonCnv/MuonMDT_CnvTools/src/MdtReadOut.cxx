/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtReadOut.h"

#include <string>

// using std::cout;

MdtReadOut::MdtReadOut() : m_word(0) {}


// Encode a 32-bits data word from fragments with size < 16 bits
uint32_t MdtReadOut::setBits(uint16_t nData, const uint16_t* inputData, const uint16_t* inputPos) {
    uint32_t result = 0;
    uint32_t input = 0;
    uint16_t pos = 0;
    for (uint16_t i = 0; i < nData; i++) {
        input = *(inputData + i);
        pos = *(inputPos + i);
        result = result | (input << pos);
    }
    return result;
}

// Encode a 32-bits data word from fragments with size < 32 bits
uint32_t MdtReadOut::setBits(uint16_t nData, const uint32_t* inputData, const uint16_t* inputPos) {
    uint32_t result = 0;
    uint32_t input = 0;
    uint16_t pos = 0;
    for (uint16_t i = 0; i < nData; i++) {
        input = *(inputData + i);
        pos = *(inputPos + i);
        result = result | (input << pos);
    }
    return result;
}
