/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MUON_NSW_STGC_MAPPER_H_
#define _MUON_NSW_STGC_MAPPER_H_

#include <cstdint>
#include <map>
#include <vector>

namespace Muon
{
  namespace nsw
  {
    enum ChannelNumber
    {
      sTGCQS1ChannelNumber = 406,
      sTGCQS2ChannelNumber = 365,
      sTGCQS3ChannelNumber = 307,
      sTGCQL1ChannelNumber = 408,
      sTGCQL2ChannelNumber = 366,
      sTGCQL3ChannelNumber = 353
    };

    // Sector type = 0 for small, 1 for large

    static const std::vector <uint8_t> s_stgc_sector_type = {0, 1};

    // Layer type: 0 for layers 0 and 4 and 1 for layers 2 and 4 in the quadruplet

    static const std::vector <uint8_t> s_stgc_layer_type = {0, 1};

    // Number of quadruplets per sector (in R)

    static const uint8_t s_stgc_nquadruplets = 3;

    // Number of channels as function of sector type and quadruplet number
    // Sector type = 0 for small, 1 for large

    static const std::map <std::pair <uint8_t, uint8_t>, uint16_t> s_stgc_nchan_map =
      {{{0, 0}, Muon::nsw::sTGCQS1ChannelNumber}, {{0, 1}, Muon::nsw::sTGCQS2ChannelNumber},
       {{0, 2}, Muon::nsw::sTGCQS3ChannelNumber}, {{1, 0}, Muon::nsw::sTGCQL1ChannelNumber},
       {{1, 1}, Muon::nsw::sTGCQS2ChannelNumber}, {{1, 2}, Muon::nsw::sTGCQS3ChannelNumber}};

    // First VMM (number, channel) used as function of {sector type, layer_type, quadruplet}

    static const std::map <std::vector <uint8_t>, std::pair <uint8_t, uint8_t>> s_stgc_first_channel =
    {
      {{0, 0, 0}, {0, 0}}, {{0, 0, 1}, {2, 19}}, {{0, 0, 2}, {3, 13}},
      {{0, 1, 0}, {0, 42}}, {{0, 1, 1}, {2, 19}}, {{0, 1, 2}, {3, 13}},
      {{1, 0, 0}, {0, 0}}, {{1, 0, 1}, {2, 18}}, {{1, 0, 2}, {2, 31}},
      {{1, 1, 0}, {0, 40}}, {{1, 1, 1}, {2, 18}}, {{1, 1, 2}, {2, 31}}
    };

    // Last VMM (number, channel) used as function of {sector type, layer_type, quadruplet}

    static const std::map <std::vector <uint8_t>, std::pair <uint8_t, uint8_t>> s_stgc_last_channel =
    {
      {{0, 0, 0}, {7, 21}}, {{0, 0, 1}, {7, 63}}, {{0, 0, 2}, {7, 63}},
      {{0, 1, 0}, {7, 63}}, {{0, 1, 1}, {7, 63}}, {{0, 1, 2}, {7, 63}},
      {{1, 0, 0}, {7, 23}}, {{1, 0, 1}, {7, 63}}, {{1, 0, 2}, {7, 63}},
      {{1, 1, 0}, {7, 63}}, {{1, 1, 1}, {7, 63}}, {{1, 1, 2}, {7, 63}}
    };

    class sTGCMapper
    {
     public:
      sTGCMapper ();
      virtual ~sTGCMapper () {};

      uint16_t channel_number (uint8_t sector_type, uint8_t quadruplet, uint8_t layer_type,
				     uint16_t vmm, uint16_t vmm_chan) const;

     private:
      uint32_t private_id (uint8_t is_large, uint8_t quadruplet, uint8_t layer_type, unsigned int vmm_chan) const;

     private:
      std::map <uint32_t, uint16_t> m_channel_map;
    };
  }
}

inline uint16_t Muon::nsw::sTGCMapper::channel_number (uint8_t sector_type, uint8_t quadruplet, 
						       uint8_t layer_type, uint16_t vmm,
						       uint16_t vmm_chan) const
{
  unsigned int c = vmm * Muon::nsw::s_VMM_channels + vmm_chan;
  uint32_t id = private_id (sector_type, quadruplet, layer_type, c);
  const uint16_t channel_number = m_channel_map.find (id)->second;
  return channel_number;
}

inline uint32_t Muon::nsw::sTGCMapper::private_id (uint8_t is_large, uint8_t quadruplet,
						   uint8_t layer_type, unsigned int vmm_chan) const
{
  return (is_large & 0xf) << 28 | (quadruplet & 0xf) << 24 | (layer_type & 0xff) << 16 | (vmm_chan & 0xffff);
}

#endif // _MUON_NSW_STGC_MAPPER_H_


