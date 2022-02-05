/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonNSWCommonDecode/sTGCMapper.h"

#include <iostream>
//=====================================================================
uint16_t Muon::nsw::sTGCMapper::channel_number (uint8_t channel_type, uint8_t sector_type, uint8_t quadruplet, uint8_t layer, uint16_t vmm, uint16_t vmm_chan) const
{
  int counter{1};
  int chan = vmm*Muon::nsw::VMM_channels + vmm_chan;
  const auto& ranges = Muon::nsw::s_stgc_channel_map.at( sTGCMapper::private_id(channel_type, sector_type, quadruplet, layer) );

  for (const auto& range : ranges) {
    int chanFirst = range[0]*Muon::nsw::VMM_channels + range[1];
    int chanLast  = range[2]*Muon::nsw::VMM_channels + range[3];
    int increment = chanLast >= chanFirst ? 1 : -1;
      
    if ( (chan - chanFirst)*(chan - chanLast) <= 0 ) 
      return counter + increment*(chan - chanFirst);
      
    counter += increment*(chanLast - chanFirst) + 1;
  }
  
  return 0;
}
