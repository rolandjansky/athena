/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MUON_NSW_OFFLINE_HELPER_H_
#define _MUON_NSW_OFFLINE_HELPER_H_

#include <memory>

#include "MuonNSWCommonDecode/NSWDecodeHelper.h"
#include "MuonNSWCommonDecode/NSWResourceId.h"
#include "MuonNSWCommonDecode/sTGCMapper.h"

namespace Muon
{
  namespace nsw
  {
    namespace helper
    {
      class NSWOfflineHelper
      {
       public:
        NSWOfflineHelper (Muon::nsw::NSWResourceId *res_id, uint16_t vmm_number, uint16_t vmm_channel_number)
          : m_elinkId (res_id), m_vmm (vmm_number), m_chan (vmm_channel_number) {};

        virtual ~NSWOfflineHelper () {};

        // Direct access to offline parameters from elinkId

        bool is_large_station  () {return m_elinkId->is_large_station ();};

        int8_t  station_eta    () {return m_elinkId->station_eta ();};
        uint8_t station_phi    () {return m_elinkId->station_phi ();};
        uint8_t multi_layer    () {return m_elinkId->multi_layer ();};
        uint8_t gas_gap        () {return m_elinkId->gas_gap ();};

        // Offline parameters

        uint8_t  channel_type   ();
        uint16_t channel_number ();

       private:
        Muon::nsw::NSWResourceId *m_elinkId;

        uint16_t m_vmm;
        uint16_t m_chan;
      };
    }
  }
}

#endif // _MUON_NSW_OFFLINE_HELPER_H_

