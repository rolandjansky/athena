/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/sTGCSDOVariables.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

namespace MuonPRDTest {
    sTgcSDOVariables::sTgcSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "SDO_sTGC", false, msglvl), m_key{container_name} {}

    bool sTgcSDOVariables::declare_keys() { return declare_dependency(m_key); }

    bool sTgcSDOVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fill sTgcSDOVariables()");
        SG::ReadHandle<MuonSimDataCollection> stgcSdoContainer{m_key, ctx};
        if (!stgcSdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve digit container " << m_key.fullKey());
            return false;
        }
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        unsigned int n_sdo{0};
        for (const auto& coll : *stgcSdoContainer) {
            const Identifier& id = coll.first;
            const MuonSimData& stgc_sdo = coll.second;

            m_NSWsTGC_sdo_globaltime.push_back(stgc_sdo.getTime());
            m_NSWsTGC_sdo_word.push_back(stgc_sdo.word());

            ATH_MSG_DEBUG("sTGC SDO: " << idHelperSvc()->toString(id));

            ATH_MSG_DEBUG("Get the truth deposits from the SDO.");
            std::vector<MuonSimData::Deposit> deposits;
            stgc_sdo.deposits(deposits);

            const Amg::Vector3D hit_gpos = stgc_sdo.globalPosition();
            m_NSWsTGC_dig_globalPos.push_back(hit_gpos);

            // use the information of the first deposit
            int barcode = deposits[0].first.barcode();
            double MuonMCdata_firstentry = deposits[0].second.firstEntry();
            double MuonMCdata_secondentry = deposits[0].second.secondEntry();

            ATH_MSG_DEBUG("sTGC SDO barcode=" << barcode);
            ATH_MSG_DEBUG("sTGC SDO energy=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                                               << ", tof=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

            m_NSWsTGC_sdo_barcode.push_back(barcode);
            m_NSWsTGC_sdo_E.push_back(MuonMCdata_firstentry);
            m_NSWsTGC_sdo_tof.push_back(MuonMCdata_secondentry);

            // Retrieve the detector element and local SDO coordinates
            const MuonGM::sTgcReadoutElement* rdoEl = MuonDetMgr->getsTgcReadoutElement(id);
            if (!rdoEl) {
                ATH_MSG_ERROR("sTGCSDOVariables::fillVariables() - Failed to retrieve sTgcReadoutElement for " << idHelperSvc()->toString(id));
                return false;
            }

            m_NSWsTGC_sdo_id.push_back(id);
            Amg::Vector2D loc_pos(0., 0.);
            rdoEl->surface(id).globalToLocal(stgc_sdo.globalPosition(), Amg::Vector3D(0., 0., 0.), loc_pos);
            ATH_MSG_DEBUG("sTGC SDO local position X=" << std::setw(9) << std::setprecision(2) << loc_pos[0]
                          << ", local position Y=" << std::setw(9) << std::setprecision(2) << loc_pos[1]);
            m_NSWsTGC_sdo_localPosX.push_back( loc_pos[0] );
            m_NSWsTGC_sdo_localPosY.push_back( loc_pos[1] );

            ++n_sdo;
        }
        m_NSWsTGC_nsdo = n_sdo;

        ATH_MSG_DEBUG("Processed " << n_sdo << " sTGC SDOs");
        return true;
    }
}