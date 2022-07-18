/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <MuonPRDTest/MDTSDOVariables.h>
#include <StoreGate/ReadHandle.h>
namespace MuonPRDTest {
    MdtSDOVariables::MdtSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "SDO_MDT", false, msglvl), m_sdo_key{container_name} {}

    bool MdtSDOVariables::fill(const EventContext& ctx) {
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        ATH_MSG_DEBUG(" do fillMdtSDOVariables()");
        ATH_MSG_VERBOSE("MuonDetectorManager from Conditions Store accessed" << MuonDetMgr);

        ATH_MSG_DEBUG("Retrieve MDT SDO container with name = " << m_sdo_key.fullKey());

        SG::ReadHandle<MuonSimDataCollection> mdtSdoContainer{m_sdo_key, ctx};
        if (!mdtSdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve SDO container " << m_sdo_key.fullKey());
            return false;
        }

        for (const auto& coll : *mdtSdoContainer) {
            const Identifier id = coll.first;
            const MuonSimData& mdt_sdo = coll.second;
            m_mdt_sdo_id.push_back(id);
            // Get information on the SDO
            ATH_MSG_DEBUG("MDT SDO:  " << idHelperSvc()->toString(id));
            ATH_MSG_DEBUG("Get the truth deposits from the SDO.");
            std::vector<MuonSimData::Deposit> deposits;
            mdt_sdo.deposits(deposits);

            m_mdt_sdo_globalPos.push_back(mdt_sdo.globalPosition());
            m_mdt_sdo_globalTime.push_back(mdt_sdo.getTime());
            m_mdt_sdo_word.push_back(mdt_sdo.word());

            // use the information of the first deposit
            int barcode = deposits[0].first.barcode();
            double MuonMCdata_firstentry = deposits[0].second.firstEntry();
            double MuonMCdata_secondentry = deposits[0].second.secondEntry();

            ATH_MSG_DEBUG("MDT SDO barcode=" << barcode);
            ATH_MSG_DEBUG("MDT SDO localPosX=" << std::setw(9) << std::setprecision(2) << MuonMCdata_firstentry
                                               << ", localPosY=" << std::setw(9) << std::setprecision(2) << MuonMCdata_secondentry);

            m_mdt_sdo_barcode.push_back(barcode);
            m_mdt_sdo_localPosX.push_back(MuonMCdata_firstentry);
            m_mdt_sdo_localPosY.push_back(MuonMCdata_secondentry);
        }
        m_mdt_nsdo = mdtSdoContainer.cptr()->size();
        ATH_MSG_DEBUG("Processed " << m_mdt_nsdo << " MDT SDOs");
        return true;
    }

    bool MdtSDOVariables::declare_keys() { return declare_dependency(m_sdo_key); }
}  // namespace MuonPRDTest