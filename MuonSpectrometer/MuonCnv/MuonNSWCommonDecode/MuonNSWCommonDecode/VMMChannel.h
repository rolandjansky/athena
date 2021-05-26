/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MUON_VMM_CHANNEL_H_
#define _MUON_VMM_CHANNEL_H_

namespace Muon
{
  namespace nsw
  {
    class NSWElink;

    class VMMChannel
    {
     private:
      uint16_t m_vmm;          // vmm number
      uint16_t m_chan;         // vmm channel
      uint16_t m_pdo;          // adc amplitude
      uint16_t m_tdo;          // peaking time 
      uint16_t m_rel;          // relative BCID
      bool     m_neighbor;     // flag
      bool     m_parity;       // parity

      Muon::nsw::NSWElink *m_elink;

     private:
      uint32_t fix_outward (uint32_t vmm_word);

     public:
      VMMChannel (uint32_t vmm_word, Muon::nsw::NSWElink *elink, bool inward = false);
      virtual ~VMMChannel () {};

      //bool calculate_parity (uint32_t word);

      bool     neighbor () {return m_neighbor;};
      bool     parity   () {return m_parity;};
      uint16_t pdo      () {return m_pdo;};
      uint16_t tdo      () {return m_tdo;};
      uint16_t rel_bcid () {return m_rel;};
      uint16_t channel  () {return m_chan;};
      uint16_t vmm      () {return m_vmm;};

      // Access to Detector logical ID

      const Muon::nsw::NSWElink *elink () const {return m_elink;};
    };
  }
}

inline uint32_t Muon::nsw::VMMChannel::fix_outward (uint32_t vmm_word)
{
  // To transform inward-counted VMM and channels into outward-counted.
  // Parity bit is also changed accordingy.

  return (vmm_word ^ 0x87fc0000);
}

//inline bool Muon::nsw::VMMChannel::calculate_parity (uint32_t word)
//{
//  return true;
//}

inline Muon::nsw::VMMChannel::VMMChannel (uint32_t vmm_word, Muon::nsw::NSWElink *elink, bool inward)
  : m_elink (elink)
{
  uint32_t word = inward ? this->fix_outward (vmm_word) : vmm_word;

  m_tdo      = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskVmmHitTDC, Muon::nsw::bitPosVmmHitTDC);  // should be 0 if noTDC is true
  m_pdo      = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskVmmHitADC, Muon::nsw::bitPosVmmHitADC);
  m_chan     = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskVmmHitCHANNEL, Muon::nsw::bitPosVmmHitCHANNEL);
  m_vmm      = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskVmmHitVMMID, Muon::nsw::bitPosVmmHitVMMID);
  m_rel      = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskVmmHitRELBCID, Muon::nsw::bitPosVmmHitRELBCID);
  m_neighbor = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskVmmHitN, Muon::nsw::bitPosVmmHitN);
  m_parity   = Muon::nsw::helper::get_bits (word, Muon::nsw::bitMaskVmmHitP, Muon::nsw::bitPosVmmHitP);
}

#endif // _MUON_VMM_CHANNEL_H_

