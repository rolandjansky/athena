/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/MMSDOVariables.h"

namespace MuonPRDTest {
    MMSDOVariables::MMSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "SDO_MM", false, msglvl), m_key{container_name} {}

    bool MMSDOVariables::declare_keys() { return declare_dependency(m_key); }

    bool MMSDOVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fill MMSDOVariables()");
        SG::ReadHandle<MuonSimDataCollection> mmSdoContainer{m_key, ctx};
        if (!mmSdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve digit container " << m_key.fullKey());
            return false;
        }
        unsigned int n_sdo{0};
        for (const auto& coll : *mmSdoContainer) {
            const Identifier& id = coll.first;
            const MuonSimData& mm_sdo = coll.second;

            m_NSWMM_sdo_globaltime.push_back(mm_sdo.getTime());
            m_NSWMM_sdo_word.push_back(mm_sdo.word());

            ATH_MSG_DEBUG("Get the truth deposits from the SDO.");
            std::vector<MuonSimData::Deposit> deposits;
            mm_sdo.deposits(deposits);

            m_NSWMM_sdo_id.push_back(id);
            const Amg::Vector3D hit_gpos = mm_sdo.globalPosition();
            m_NSWMM_dig_globalPos.push_back(hit_gpos);

            // use the information of the first deposit
            int barcode = deposits[0].first.barcode();
            double MuonMCdata_firstentry = deposits[0].second.firstEntry();
            double MuonMCdata_secondentry = deposits[0].second.secondEntry();

            ATH_MSG_DEBUG("MM SDO barcode=" << barcode);
            ATH_MSG_DEBUG("MM SDO local position X=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                                               << ", local position Y=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

            m_NSWMM_sdo_barcode.push_back(barcode);

            m_NSWMM_sdo_localPosX.push_back(MuonMCdata_firstentry);
            m_NSWMM_sdo_localPosY.push_back(MuonMCdata_secondentry);

            ++n_sdo;
        }
        m_NSWMM_nsdo = n_sdo;

        ATH_MSG_DEBUG("Processed " << n_sdo << "MM SDOs");
        return true;
    }
}