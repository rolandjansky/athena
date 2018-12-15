/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


//Local includes
#include "TrigT1NSWSimTools/MMStripOfflineData.h"

#include <string>


namespace NSWL1 {

    MMStripOfflineData::MMStripOfflineData(Identifier id, const MmIdHelper* helper) : 
    MMStripData(),
    m_id(id), m_helper(helper)
    {}

    MMStripOfflineData::~MMStripOfflineData() {
        // MMStripOfflineData doesn't own the helper, so nothing to do here
    }

    void MMStripOfflineData::setTrigBCID(int bcid) {
      m_trig_bcid = bcid;
    }


    //! methods for retrieving the bare data
    uint32_t MMStripOfflineData::id()     const { return m_id.get_identifier32().get_compact(); }
    float MMStripOfflineData::charge()    const { return m_mmstrip_charge; }
    int MMStripOfflineData::trig_BCID()        const { return m_trig_bcid; }

    //! helper  methods decoding the hit fragment position within the NSW detector
    int MMStripOfflineData::sideId()      const { 
        if (m_helper)  return (m_helper->stationEta(m_id)<0)? 0 : 1;
        return -1;
    }

    int MMStripOfflineData::moduleId()    const {
        if (m_helper)  return std::abs(m_helper->stationEta(m_id))-1;
        return -1;
    }

    int MMStripOfflineData::sectorId()    const {
        if (m_helper) {
            std::string stName = m_helper->stationNameString(m_helper->stationName(m_id));
            bool isSmall = stName[2]=='S';
            return (isSmall)? m_helper->stationPhi(m_id)*2 - 1 : m_helper->stationPhi(m_id)*2 - 2;
        }
        return -1;
    }

    int MMStripOfflineData::multipletId() const {
        if (m_helper) return m_helper->multilayer(m_id) - 1;
        return -1;
    }

    int MMStripOfflineData::gasGapId()    const {
        if (m_helper) return m_helper->gasGap(m_id) - 1;
        return -1;
    }

    int MMStripOfflineData::channelId()   const {
        if (m_helper) return m_helper->channel(m_id) - 1;
        return -1;
    }

}
