/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


//Local includes
#include "TrigT1NSWSimTools/PadOfflineData.h"
#include "TrigT1NSWSimTools/sTGCenumeration.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "TVector3.h"

#include <string>


namespace NSWL1 {

    PadOfflineData::PadOfflineData(Identifier id, float time, uint16_t bc_tag, const sTgcIdHelper* helper) :
    PadData(),
    m_id(id), m_helper(helper),m_time(time),m_bc_tag(bc_tag) 
    {}

    PadOfflineData::~PadOfflineData() {
        // PaddOfflineData doesn't own the helper, so nothing to do here
    }
 
    //! methods for retrieving the bare data
    uint32_t   PadOfflineData::id()       const { return m_id.get_identifier32().get_compact(); }
    Identifier PadOfflineData::Id()     const {return m_id;}
    uint16_t   PadOfflineData::BC_Tag()   const { return m_bc_tag; }
    float      PadOfflineData::time()     const { return m_time; }

    //! helper  methods decoding the hit fragment position within the NSW detector
    int PadOfflineData::sideId()      const { 
        if (m_helper)  return (m_helper->stationEta(m_id)<0)? 0 : 1;
        return -1;
    }

    int PadOfflineData::moduleId()    const {
        if (m_helper)  return std::abs(m_helper->stationEta(m_id));
        return -1;
    }

    int PadOfflineData::sectorId()    const {
        if(m_helper) { return m_helper->stationPhi(m_id); }
        return -1;
    }

    int PadOfflineData::sectorType()    const {
        if(m_helper) { return m_helper->isSmall(m_id) ? 0 : 1; }
        return -1;
    }

    int PadOfflineData::multipletId() const {
        if (m_helper) return m_helper->multilayer(m_id);
        return -1;
    }

    int PadOfflineData::gasGapId()    const {
        if (m_helper) return m_helper->gasGap(m_id);
        return -1;
    }

    int PadOfflineData::channelId()   const {
        if (m_helper) return m_helper->channel(m_id);
        return -1;
    }

    int PadOfflineData::padEtaId()    const {
        if (m_helper) return m_helper->padEta(m_id);
        return -1;
    }

    int PadOfflineData::padPhiId()    const {
        if (m_helper) return m_helper->padPhi(m_id);
        return -1;
    }

}
