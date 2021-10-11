/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include <cstdint>
#include <map>
#include <vector>

#include "MuonNSWCommonDecode/VMMChannel.h"
#include "MuonNSWCommonDecode/NSWElink.h"

#include "MuonNSWCommonDecode/sTGCMapper.h"

Muon::nsw::sTGCMapper::sTGCMapper ()
{
  std::cout << "==============> Init map" << std::endl;
  for (auto s_type : Muon::nsw::s_stgc_sector_type)
    for (auto l_type : Muon::nsw::s_stgc_layer_type)
    {
      for (uint8_t q = 0; q < s_stgc_nquadruplets; ++q)
      {
        const std::vector <uint8_t> v = {s_type, l_type, q};
        const std::pair <uint8_t, uint8_t> f  = Muon::nsw::s_stgc_first_channel.find (v)->second;
        const std::pair <uint8_t, uint8_t> l  = Muon::nsw::s_stgc_last_channel.find (v)->second;
        const std::pair <uint8_t, uint8_t> sq = {s_type, q};
        const unsigned int nchan = Muon::nsw::s_stgc_nchan_map.find (sq)->second;
        unsigned int imin = f.first * Muon::nsw::s_VMM_channels + f.second;
        unsigned int jmax = l.first * Muon::nsw::s_VMM_channels + l.second;

        unsigned int i, j, c;
        for (i = imin, j = jmax, c = nchan; c > 0; ++i, --j, --c)
        {
          if (q == 0 && l_type == 0 && (i == imin + 336 || i == imin + 400)) i += 32;
          else if (q == 0 && l_type == 1 && (j == jmax - 336 || j == jmax - 400)) j -= 32;

          unsigned int vmm_chan = l_type == 0 ? i : j;
          uint32_t id = private_id (s_type, q, l_type, vmm_chan);
          std::pair <uint32_t, uint16_t> p (id, c);
          m_channel_map.insert (p);
        }
      }
    }
}
