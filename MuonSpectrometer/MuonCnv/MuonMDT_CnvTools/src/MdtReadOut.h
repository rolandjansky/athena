/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTREADOUT_H
#define MUONBYTESTREAM_MDTREADOUT_H

#include <stdint.h>
#include <utility>
// Base class for Mdt readout decoding
// adapted to Mdt from RpcReadOut

class MdtReadOut {
public:
    MdtReadOut();
    ~MdtReadOut() =default;

protected:

    constexpr std::pair<uint32_t, uint16_t> getBitsWord(const uint16_t bstart, const uint16_t bstop)
    {
       uint32_t word = 0;
       for (uint16_t i = bstop; i < bstart; i++) word = ((word | 1) << 1);
       return std::make_pair(word, bstop);
    }

    // Decode a section of a 32-bits data word
    uint32_t getBits(std::pair<uint32_t, uint16_t> wordbstop) const
    {
       uint32_t result = (m_word >> wordbstop.second) & (wordbstop.first | 1);
       return result;
    }

    // Encode a 32-bits data word from fragments with size < 16 bits
    uint32_t setBits(uint16_t nData, const uint16_t* inputData, const uint16_t* inputPos);

    // Encode a 32-bits data word from fragments with size < 32 bits
    uint32_t setBits(uint16_t nData, const uint32_t* inputData, const uint16_t* inputPos);

    uint32_t m_word;
};

#endif
