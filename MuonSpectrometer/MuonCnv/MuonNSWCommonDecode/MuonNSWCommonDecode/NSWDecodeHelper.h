/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MUON_NSW_DECODE_HELPER_H_
#define _MUON_NSW_DECODE_HELPER_H_

#include <stdint.h>

namespace Muon
{
  namespace nsw
  {
    namespace helper
    {
      uint32_t get_bits (uint32_t word, uint32_t mask, uint8_t position);
      //uint32_t set_bits (uint32_t word, uint32_t mask, uint8_t position);
    }
  }
}

inline uint32_t Muon::nsw::helper::get_bits (uint32_t word, uint32_t mask, uint8_t position)
{
  return (word >> position) & mask;
}

//inline uint32_t Muon::nsw::helper::set_bits (uint32_t word, uint32_t setbits, uint32_t mask)
//{
//  return word; //TODO 
//}

#endif // _MUON_NSW_DECODE_HELPER_H_


