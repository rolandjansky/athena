/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/TGCSDOVariables.h"
namespace MuonPRDTest {
    TgcSDOVariables::TgcSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "TGC_SDO", false, msglvl), m_key{container_name} {}

    bool TgcSDOVariables::declare_keys() { return declare_dependency(m_key); }

    bool TgcSDOVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fill TgcSDOVariable()");
        SG::ReadHandle<MuonSimDataCollection> tgcSdoContainer{m_key, ctx};
        if (!tgcSdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrive digit container " << m_key.fullKey());
            return false;
        }
        unsigned int n_sdo{0};
        for (const auto& coll : *tgcSdoContainer) {
            const Identifier& id = coll.first;
            const MuonSimData& tgc_sdo = coll.second;
            m_tgc_sdo_id.push_back(id);
            m_tgc_sdo_globaltime.push_back(tgc_sdo.getTime());
            m_tgc_sdo_word.push_back(tgc_sdo.word());

            ATH_MSG_DEBUG("TGC SDO: " << idHelperSvc()->toString(id));

            ATH_MSG_DEBUG("Get the truth deposits from the SDO.");
            std::vector<MuonSimData::Deposit> deposits;
            tgc_sdo.deposits(deposits);
            m_TGC_dig_globalPos.push_back(tgc_sdo.globalPosition());

            // use the information of the first deposit
            int barcode = deposits[0].first.barcode();
            double MuonMCdata_firstentry = deposits[0].second.firstEntry();
            double MuonMCdata_secondentry = deposits[0].second.secondEntry();

            ATH_MSG_DEBUG("TGC SDO barcode=" << barcode);
            ATH_MSG_DEBUG("TGC SDO localPosX=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                                               << ", localPosY=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

            m_tgc_sdo_barcode.push_back(barcode);
            m_tgc_sdo_localPosX.push_back(MuonMCdata_firstentry);
            m_tgc_sdo_localPosY.push_back(MuonMCdata_secondentry);
            ++n_sdo;
        }
        m_tgc_nsdo = n_sdo;

        ATH_MSG_DEBUG("Processed " << n_sdo << " TGC SDOs");
        return true;
    }
}