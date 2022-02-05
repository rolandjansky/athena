/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MUON_NSW_STGC_MAPPER_H_
#define _MUON_NSW_STGC_MAPPER_H_

#include <cstdint>
#include <map>
#include <vector>

#include "MuonNSWCommonDecode/NSWDecodeHelper.h"

namespace Muon
{
  namespace nsw
  {

    class sTGCMapper
    {
     public:
      sTGCMapper() {};
      virtual ~sTGCMapper () {};

      uint16_t channel_number (uint8_t channel_type, uint8_t sector_type, uint8_t quadruplet, uint8_t layer, uint16_t vmm, uint16_t vmm_chan) const;

      static uint16_t private_id (uint8_t channel_type, uint8_t sector_type, uint8_t quadruplet, uint8_t layer);
    };
  
    static const std::map <uint16_t, std::vector<std::vector<uint8_t>>> s_stgc_channel_map =
    {
      // channel range: {vmm0, vmm0_channel, vmm1, vmm1_channel}
      // as a function of channel_type, sector_type, quadruplet, layer

      // note: in bytestream data from pulser runs we see that pads
      // are connected to vmms = 2, 3 and not to 1, 2. 
      
      //**** PADS
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 0, 0),  { {3, 39, 2, 36} }}, // QS1C
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 0, 1),  { {2, 24, 3, 27} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 0, 2),  { {2, 24, 3, 31} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 0, 3),  { {3, 39, 2, 32} }}, 
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 0, 4),  { {3, 39, 2, 36} }}, // QS1P
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 0, 5),  { {2, 24, 3, 27} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 0, 6),  { {2, 24, 3, 10} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 0, 7),  { {3, 39, 2, 53} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 1, 0),  { {3, 23, 2, 40} }}, // QS2C
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 1, 1),  { {2, 40, 3, 23} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 1, 2),  { {2, 42, 3, 22} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 1, 3),  { {3, 21, 2, 41} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 1, 4),  { {3, 14, 2, 49} }}, // QS2P
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 1, 5),  { {2, 49, 3, 14} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 1, 6),  { {2, 42, 3, 22} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 1, 7),  { {3, 21, 2, 41} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 2, 0),  { {3, 40, 3,  2} }}, // QS3C
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 2, 1),  { {2,  1, 2, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 2, 2),  { {2,  0, 2, 41} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 2, 3),  { {3, 41, 3,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 2, 4),  { {3, 32, 3,  9} }}, // QS3P
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 2, 5),  { {3,  9, 3, 32} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 2, 6),  { {3,  2, 3, 40} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 0, 2, 7),  { {3, 39, 3,  1} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 0, 0),  { {3, 39, 2,  2} }}, // QL1P
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 0, 1),  { {2,  8, 3, 45} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 0, 2),  { {2,  8, 3, 55} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 0, 3),  { {3, 39, 2,  0}, {3, 40, 3, 47} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 0, 4),  { {3, 39, 2,  8} }}, // QL1C
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 0, 5),  { {2,  8, 3, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 0, 6),  { {2, 13, 3, 44} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 0, 7),  { {3, 34, 2,  3} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 1, 0),  { {3, 29, 2, 38} }}, // QL2P
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 1, 1),  { {2, 34, 3, 25} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 1, 2),  { {2, 24, 3, 34} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 1, 3),  { {3, 39, 2, 29} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 1, 4),  { {3, 29, 2, 38} }}, // QL2C
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 1, 5),  { {2, 34, 3, 25} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 1, 6),  { {2, 34, 3, 25} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 1, 7),  { {3, 29, 2, 38} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 2, 0),  { {3, 21, 2, 26} }}, // QL3P
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 2, 1),  { {2, 44, 3, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 2, 2),  { {2, 34, 3, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 2, 3),  { {3, 31, 2, 26} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 2, 4),  { {3, 13, 2, 26} }}, // QL3C
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 2, 5),  { {2, 52, 3, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 2, 6),  { {2, 48, 3, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_PAD, 1, 2, 7),  { {3, 17, 2, 26} }},

      //**** STRIPS
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 0, 0),  { {7, 21, 6, 48}, {6, 15, 5, 48}, {5, 15, 0,  0} }}, 
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 0, 1),  { {0, 42, 1, 15}, {1, 48, 2, 15}, {2, 48, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 0, 2),  { {0, 42, 1, 15}, {1, 48, 2, 15}, {2, 48, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 0, 3),  { {7, 21, 6, 48}, {6, 15, 5, 48}, {5, 15, 0,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 0, 4),  { {7, 21, 6, 48}, {6, 15, 5, 48}, {5, 15, 0,  0} }}, 
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 0, 5),  { {0, 42, 1, 15}, {1, 48, 2, 15}, {2, 48, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 0, 6),  { {0, 42, 1, 15}, {1, 48, 2, 15}, {2, 48, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 0, 7),  { {7, 21, 6, 48}, {6, 15, 5, 48}, {5, 15, 0,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 1, 0),  { {7, 63, 2, 19} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 1, 1),  { {2, 19, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 1, 2),  { {2, 19, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 1, 3),  { {7, 63, 2, 19} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 1, 4),  { {7, 63, 2, 19} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 1, 5),  { {2, 19, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 1, 6),  { {2, 19, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 1, 7),  { {7, 63, 2, 19} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 2, 0),  { {7, 63, 3, 13} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 2, 1),  { {3, 13, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 2, 2),  { {3, 13, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 2, 3),  { {7, 63, 3, 13} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 2, 4),  { {7, 63, 3, 13} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 2, 5),  { {3, 13, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 2, 6),  { {3, 13, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 0, 2, 7),  { {7, 63, 3, 13} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 0, 0),  { {7, 23, 6, 48}, {6, 15, 5, 48}, {5, 15, 0,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 0, 1),  { {0, 40, 1, 15}, {1, 48, 2, 15}, {2, 48, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 0, 2),  { {0, 40, 1, 15}, {1, 48, 2, 15}, {2, 48, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 0, 3),  { {7, 23, 6, 48}, {6, 15, 5, 48}, {5, 15, 0,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 0, 4),  { {7, 23, 6, 48}, {6, 15, 5, 48}, {5, 15, 0,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 0, 5),  { {0, 40, 1, 15}, {1, 48, 2, 15}, {2, 48, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 0, 6),  { {0, 40, 1, 15}, {1, 48, 2, 15}, {2, 48, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 0, 7),  { {7, 23, 6, 48}, {6, 15, 5, 48}, {5, 15, 0,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 1, 0),  { {7, 63, 2, 18} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 1, 1),  { {2, 18, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 1, 2),  { {2, 18, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 1, 3),  { {7, 63, 2, 18} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 1, 4),  { {7, 63, 2, 18} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 1, 5),  { {2, 18, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 1, 6),  { {2, 18, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 1, 7),  { {7, 63, 2, 18} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 2, 0),  { {7, 63, 2, 31} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 2, 1),  { {2, 31, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 2, 2),  { {2, 31, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 2, 3),  { {7, 63, 2, 31} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 2, 4),  { {7, 63, 2, 31} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 2, 5),  { {2, 31, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 2, 6),  { {2, 31, 7, 63} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_STRIP, 1, 2, 7),  { {7, 63, 2, 31} }},

      //**** WIRES
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 0, 0),  { {0, 57, 0, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 0, 1),  { {0, 57, 0, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 0, 2),  { {0, 57, 0, 38} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 0, 3),  { {0, 57, 0, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 0, 4),  { {0, 57, 0, 39} }}, 
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 0, 5),  { {0, 57, 0, 38} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 0, 6),  { {0, 57, 0, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 0, 7),  { {0, 57, 0, 39} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 1, 0),  { {0, 57, 0, 29} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 1, 1),  { {0, 57, 0, 29} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 1, 2),  { {0, 57, 0, 28} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 1, 3),  { {0, 57, 0, 29} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 1, 4),  { {0, 57, 0, 29} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 1, 5),  { {0, 57, 0, 28} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 1, 6),  { {0, 57, 0, 29} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 1, 7),  { {0, 57, 0, 29} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 2, 0),  { {0, 57, 0, 20} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 2, 1),  { {0, 57, 0, 20} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 2, 2),  { {0, 57, 0, 20} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 2, 3),  { {0, 57, 0, 21} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 2, 4),  { {0, 57, 0, 21} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 2, 5),  { {0, 57, 0, 20} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 2, 6),  { {0, 57, 0, 20} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 0, 2, 7),  { {0, 57, 0, 20} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 0, 0),  { {0, 57, 0, 26} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 0, 1),  { {0, 57, 0, 26} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 0, 2),  { {0, 57, 0, 26} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 0, 3),  { {0, 57, 0, 26} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 0, 4),  { {0, 57, 0, 26} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 0, 5),  { {0, 57, 0, 26} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 0, 6),  { {0, 57, 0, 26} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 0, 7),  { {0, 57, 0, 26} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 1, 0),  { {0, 57, 0, 10} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 1, 1),  { {0, 57, 0,  9} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 1, 2),  { {0, 57, 0,  9} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 1, 3),  { {0, 57, 0, 10} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 1, 4),  { {0, 57, 0, 10} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 1, 5),  { {0, 57, 0,  9} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 1, 6),  { {0, 57, 0,  9} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 1, 7),  { {0, 57, 0, 10} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 2, 0),  { {0, 57, 0,  1} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 2, 1),  { {0, 57, 0,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 2, 2),  { {0, 57, 0,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 2, 3),  { {0, 57, 0,  1} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 2, 4),  { {0, 57, 0,  1} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 2, 5),  { {0, 57, 0,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 2, 6),  { {0, 57, 0,  0} }},
      {sTGCMapper::private_id(OFFLINE_CHANNEL_TYPE_WIRE, 1, 2, 7),  { {0, 57, 0,  1} }}
    };
  }
}



//=====================================================================
inline uint16_t Muon::nsw::sTGCMapper::private_id (uint8_t channel_type, uint8_t sector_type, uint8_t quadruplet, uint8_t layer)
{
  // an internal unique ID for every VMM channel
  return (channel_type & 0xf) << 12 | (sector_type & 0xf) << 8 | (quadruplet & 0xf) << 4 | (layer & 0xf);
}

#endif // _MUON_NSW_STGC_MAPPER_H_


