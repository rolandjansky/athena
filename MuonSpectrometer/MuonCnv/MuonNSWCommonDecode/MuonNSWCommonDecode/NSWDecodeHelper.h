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
    enum channel_type
    {
      OFFLINE_CHANNEL_TYPE_PAD = 0,
      OFFLINE_CHANNEL_TYPE_STRIP = 1,
      OFFLINE_CHANNEL_TYPE_WIRE = 2
    };

    enum vmm_channels
    {
      VMM_per_MMFE8 = 8,
      VMM_per_sFEB  = 3,
      VMM_per_pFEB  = 3,
      VMM_channels  = 64
    };

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


