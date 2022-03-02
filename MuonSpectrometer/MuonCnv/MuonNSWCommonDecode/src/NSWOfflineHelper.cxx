/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonNSWCommonDecode/NSWOfflineHelper.h"
#include "MuonNSWCommonDecode/NSWResourceId.h"
#include "MuonNSWCommonDecode/sTGCMapper.h"

//=====================================================================
uint8_t Muon::nsw::helper::NSWOfflineHelper::channel_type ()
{
  uint8_t det_id = m_elinkId->detId ();
  uint8_t chan_type = Muon::nsw::OFFLINE_CHANNEL_TYPE_STRIP;

  if (det_id == eformat::MUON_STGC_ENDCAP_A_SIDE || det_id == eformat::MUON_STGC_ENDCAP_C_SIDE)
    if (m_elinkId->resourceType () == Muon::nsw::NSW_RESOURCE_PAD)
      chan_type = m_vmm == 0 ? Muon::nsw::OFFLINE_CHANNEL_TYPE_WIRE : Muon::nsw::OFFLINE_CHANNEL_TYPE_PAD;

  return chan_type;
}


//=====================================================================
uint16_t Muon::nsw::helper::NSWOfflineHelper::channel_number ()
{
  uint8_t  det_id   = m_elinkId->detId ();
  uint8_t  position = m_elinkId->radius ();

  uint16_t chan_number = 0;

  if (det_id == eformat::MUON_MMEGA_ENDCAP_A_SIDE || det_id == eformat::MUON_MMEGA_ENDCAP_C_SIDE)
  {
    // Layers with ID (0, 2, 4, 6) have odd MMFE8 on the left side, even on the right
    // Layers with ID (1, 3, 5, 7) have even MMFE8 on the left side, odd on the right

    uint16_t outw_vmm  = m_vmm;
    uint16_t outw_chan = m_chan;

    if (((m_elinkId->layer () % 2) + (position % 2)) == 1)
    {
      outw_vmm = Muon::nsw::VMM_per_MMFE8 - m_vmm  - 1;
      outw_chan = Muon::nsw::VMM_channels - m_chan - 1;
    }

    chan_number = ((position < 10 ? position : position - 10) * Muon::nsw::VMM_per_MMFE8 + outw_vmm) * Muon::nsw::VMM_channels + outw_chan;
  }
  else if (det_id == eformat::MUON_STGC_ENDCAP_A_SIDE || det_id == eformat::MUON_STGC_ENDCAP_C_SIDE)
  {
    static const Muon::nsw::sTGCMapper m;

    // In the offline convention (sectors 1-16) odd sectors are large, even sectors are small;
    // in the online convention (sectors 0-15) odd sectors are small, even sectors are large
    uint8_t is_large   = m_elinkId->is_large_station () ? 1 : 0;
    uint8_t quad_layer = m_elinkId->layer (); // in [0, 7]

    chan_number = m.channel_number(this->channel_type(), is_large, position, quad_layer, m_vmm, m_chan);
  }

  return chan_number;
}

