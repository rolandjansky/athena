/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/CSCPRDVariables.h"

#include "MuonReadoutGeometry/CscReadoutElement.h"

namespace MuonPRDTest {
    CSCPRDVariables::CSCPRDVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "PRD_CSC", true, msglvl), m_key{container_name} {}
    bool CSCPRDVariables::declare_keys() { return declare_dependency(m_key); }

    bool CSCPRDVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillCSCPRDVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<Muon::CscPrepDataContainer> cscprdContainer{m_key, ctx};
        if (!cscprdContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve prd container " << m_key.fullKey());
            return false;
        }

        ATH_MSG_DEBUG("retrieved CSC PRD Container with size " << cscprdContainer->size());

        if (cscprdContainer->size() == 0) ATH_MSG_DEBUG(" CSC PRD Container empty ");
        unsigned int n_PRD{0};
        for(auto it : *cscprdContainer ) {
            const Muon::CscPrepDataCollection*coll = it;
            for (auto prd: *coll) {
                Identifier Id = prd->identify();

                const MuonGM::CscReadoutElement* det = MuonDetMgr->getCscReadoutElement(Id);
                if (!det) {
                   ATH_MSG_ERROR("The CSC hit "<<idHelperSvc()->toString(Id)<<" does not have a detector element attached. That should actually never happen");
                   return false;
                }

                m_CSC_PRD_id.push_back(Id);
                m_CSC_PRD_charge.push_back(prd->charge());
                m_CSC_PRD_time.push_back(prd->time());

                Amg::Vector3D pos = prd->globalPosition();
                Amg::Vector2D loc_pos(0., 0.);

                det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

                m_CSC_PRD_globalPos.push_back(pos);

                m_CSC_PRD_localPosX.push_back(loc_pos[0]);
                m_CSC_PRD_localPosY.push_back(loc_pos[1]);

                ++n_PRD;
            }
        }
        m_CSC_nPRD = n_PRD;
        ATH_MSG_DEBUG(" finished fillCSCPRDVariables()");
        return true;
    }
}