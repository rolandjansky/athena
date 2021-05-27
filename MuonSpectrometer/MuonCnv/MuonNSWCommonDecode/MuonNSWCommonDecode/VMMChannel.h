/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MUON_VMM_CHANNEL_H_
#define _MUON_VMM_CHANNEL_H_

#include "MuonNSWCommonDecode/NSWResourceId.h"

namespace Muon
{
  namespace nsw
  {
    class NSWElink;

    enum channel_type
    {
      OFFLINE_CHANNEL_TYPE_PAD = 0,
      OFFLINE_CHANNEL_TYPE_STRIP = 1,
      OFFLINE_CHANNEL_TYPE_WIRE = 2
    };

    static const uint16_t s_VMM_per_MMFE8 = 8;
    static const uint16_t s_VMM_per_sFEB  = 3;
    static const uint16_t s_VMM_per_pFEB  = 3;
    static const uint16_t s_VMM_channels  = 64;

    class VMMChannel
    {
     private:
      uint32_t m_vmm_word;

      uint16_t m_vmm;          // vmm number
      uint16_t m_chan;         // vmm channel
      uint16_t m_pdo;          // adc amplitude
      uint16_t m_tdo;          // peaking time 
      uint16_t m_rel;          // relative BCID
      bool     m_neighbor;     // flag
      bool     m_parity;       // parity

      Muon::nsw::NSWElink *m_elink;

     private:
      uint8_t  calculate_word_parity (uint32_t, uint8_t p = 0);
      uint32_t fix_outward (uint32_t vmm_word);

     public:
      VMMChannel (uint32_t vmm_word, Muon::nsw::NSWElink *elink);
      virtual ~VMMChannel () {};

      bool calculate_parity ();

      // Hit word

      uint32_t vmm_word () {return m_vmm_word;};

      // Decoded components of hit wors

      bool     neighbor     () {return m_neighbor;};
      bool     parity       () {return m_parity;};
      uint16_t pdo          () {return m_pdo;};
      uint16_t tdo          () {return m_tdo;};
      uint16_t rel_bcid     () {return m_rel;};
      uint16_t vmm_channel  () {return m_chan;};
      uint16_t vmm          () {return m_vmm;};

      // Access to Detector logical ID

      const Muon::nsw::NSWElink *elink () const {return m_elink;};

      // Direct access to offline parameters from elinkId

      bool is_large_station  () {return m_elink->elinkId ()->is_large_station ();};

      int8_t  station_eta    () {return m_elink->elinkId ()->station_eta ();};
      uint8_t station_phi    () {return m_elink->elinkId ()->station_phi ();};
      uint8_t multi_layer    () {return m_elink->elinkId ()->multi_layer ();};
      uint8_t gas_gap        () {return m_elink->elinkId ()->gas_gap ();};

      // Offline parameters

      uint8_t  channel_type   ();
      uint16_t channel_number ();
    };
  }
}

inline uint8_t  Muon::nsw::VMMChannel::channel_type ()
{
  uint8_t det_id = m_elink->elinkId ()->detId ();
  uint8_t channel_type = Muon::nsw::OFFLINE_CHANNEL_TYPE_STRIP;

  if (det_id == eformat::MUON_STGC_ENDCAP_A_SIDE || det_id == eformat::MUON_STGC_ENDCAP_A_SIDE)
    if (m_elink->elinkId ()->resourceType () == Muon::nsw::NSW_RESOURCE_PAD)
      channel_type = m_vmm == 0 ? Muon::nsw::OFFLINE_CHANNEL_TYPE_WIRE : Muon::nsw::OFFLINE_CHANNEL_TYPE_PAD;

  return channel_type;
}

inline uint16_t Muon::nsw::VMMChannel::channel_number ()
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
  else
  {
    channel_number = 0;
  }

  return channel_number;
}

inline uint32_t Muon::nsw::VMMChannel::fix_outward (uint32_t hit_word)
{
  // To transform inward-counted VMM and channels into outward-counted.
  // Parity bit is also changed accordingy.

  return (hit_word ^ 0x87fc0000);
}

inline uint8_t Muon::nsw::VMMChannel::calculate_word_parity (uint32_t w, uint8_t p)
{
  while (w > 0)
  {
    p ^= w & 0x1;
    w = w >> 1;
  }

  return p;
}

inline bool Muon::nsw::VMMChannel::calculate_parity ()
{
  uint8_t p = 0 ^ (m_neighbor ? 1 : 0);
  uint16_t bcid = m_elink->bcId () + m_rel;

  p = this->calculate_word_parity (bcid, p);
  p = this->calculate_word_parity (m_pdo, p);
  p = this->calculate_word_parity (m_tdo, p);

  return (p > 0);
}

inline Muon::nsw::VMMChannel::VMMChannel (uint32_t vmm_word, Muon::nsw::NSWElink *elink)
  : m_vmm_word (vmm_word), m_elink (elink)
{
  m_tdo      = Muon::nsw::helper::get_bits (vmm_word, Muon::nsw::bitMaskVmmHitTDC, Muon::nsw::bitPosVmmHitTDC);  // should be 0 if noTDC is true
  m_pdo      = Muon::nsw::helper::get_bits (vmm_word, Muon::nsw::bitMaskVmmHitADC, Muon::nsw::bitPosVmmHitADC);
  m_chan     = Muon::nsw::helper::get_bits (vmm_word, Muon::nsw::bitMaskVmmHitCHANNEL, Muon::nsw::bitPosVmmHitCHANNEL);
  m_vmm      = Muon::nsw::helper::get_bits (vmm_word, Muon::nsw::bitMaskVmmHitVMMID, Muon::nsw::bitPosVmmHitVMMID);
  m_rel      = Muon::nsw::helper::get_bits (vmm_word, Muon::nsw::bitMaskVmmHitRELBCID, Muon::nsw::bitPosVmmHitRELBCID);
  m_neighbor = Muon::nsw::helper::get_bits (vmm_word, Muon::nsw::bitMaskVmmHitN, Muon::nsw::bitPosVmmHitN);
  m_parity   = Muon::nsw::helper::get_bits (vmm_word, Muon::nsw::bitMaskVmmHitP, Muon::nsw::bitPosVmmHitP);
}

#endif // _MUON_VMM_CHANNEL_H_

