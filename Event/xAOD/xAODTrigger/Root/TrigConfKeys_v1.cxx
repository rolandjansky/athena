/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfKeys_v1.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/TrigConfKeys_v1.h"

namespace xAOD {

   TrigConfKeys_v1::TrigConfKeys_v1( uint32_t smk, uint32_t l1psk,
                                     uint32_t hltpsk )
      : m_smk( smk ), m_l1psk( l1psk ), m_hltpsk( hltpsk ) {

   }

   uint32_t TrigConfKeys_v1::smk() const {

      return m_smk;
   }

   void TrigConfKeys_v1::setSMK( uint32_t value ) {

      m_smk = value;
      return;
   }

   uint32_t TrigConfKeys_v1::l1psk() const {

      return m_l1psk;
   }

   void TrigConfKeys_v1::setL1psk( uint32_t value ) {

      m_l1psk = value;
      return;
   }

   uint32_t TrigConfKeys_v1::hltpsk() const {

      return m_hltpsk;
   }

   void TrigConfKeys_v1::setHLTpsk( uint32_t value ) {

      m_hltpsk = value;
      return;
   }

} // namespace xAOD
