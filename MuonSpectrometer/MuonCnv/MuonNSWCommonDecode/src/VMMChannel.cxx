/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonNSWCommonDecode/VMMChannel.h"
#include "MuonNSWCommonDecode/NSWElink.h"
#include "MuonNSWCommonDecode/sTGCMapper.h"

uint16_t Muon::nsw::VMMChannel::channel_number ()
{
  uint8_t  det_id = m_elink->elinkId ()->detId ();
  uint8_t  position = m_elink->elinkId ()->radius ();
  uint16_t channel_number;

  if (det_id == eformat::MUON_MMEGA_ENDCAP_A_SIDE || det_id == eformat::MUON_MMEGA_ENDCAP_C_SIDE)
  {
    // Layers with ID (0, 2, 4, 6) have odd MMFE8 on the left side, even on the right
    // Layers with ID (1, 3, 5, 7) have even MMFE8 on the left side, odd on the right

    uint32_t word     = ((m_elink->elinkId ()->layer () % 2) + (position % 2)) == 1 ? this->fix_outward (m_vmm_word) : m_vmm_word;
    uint8_t outw_chan = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskVmmHitCHANNEL, Muon::nsw::bitPosVmmHitCHANNEL);
    uint8_t outw_vmm  = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskVmmHitVMMID, Muon::nsw::bitPosVmmHitVMMID);

    channel_number = ((position < 10 ? position : position - 10) * Muon::nsw::s_VMM_per_MMFE8 + outw_vmm)
                   * Muon::nsw::s_VMM_channels + outw_chan;
  }
  else if (det_id == eformat::MUON_STGC_ENDCAP_A_SIDE || det_id == eformat::MUON_STGC_ENDCAP_C_SIDE)
  {
    static const Muon::nsw::sTGCMapper m;

    // Configuration of channels in a quadruplets is the same for 0, 3 and 1, 2

    uint8_t quad_layer = m_elink->elinkId ()->layer () % 4;
    uint8_t layer_type = (quad_layer & 0x1) ^ (quad_layer & 0x2);

    // In the offline convention (sectors 1-16) odd sectors are large, even sectors are small;
    // in the online convention (sectors 0-15) odd sectors are small, even sectors are large

    uint8_t is_large = m_elink->elinkId ()->is_large_station () ? 1 : 0;

    // Decode according to channel type

    switch (this->channel_type ())
    {
      case Muon::nsw::OFFLINE_CHANNEL_TYPE_STRIP:
        channel_number = m.channel_number (is_large, position, layer_type, m_vmm, m_chan);
        break;
      case Muon::nsw::OFFLINE_CHANNEL_TYPE_WIRE:
      case Muon::nsw::OFFLINE_CHANNEL_TYPE_PAD:
      default:
        channel_number = 0;
        break;
    }
  }
  else
  {
    channel_number = 0;
  }

  return channel_number;
}

