/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/TGCPRDVariables.h"

#include "MuonReadoutGeometry/TgcReadoutElement.h"

namespace MuonPRDTest {
    TGCPRDVariables::TGCPRDVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "PRD_TGC", true, msglvl), m_key{container_name} {}
    bool TGCPRDVariables::declare_keys() { return declare_dependency(m_key); }

    bool TGCPRDVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillTGCPRDVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<Muon::TgcPrepDataContainer> tgcprdContainer{m_key, ctx};
        if (!tgcprdContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve prd container " << m_key.fullKey());
            return false;
        }

        ATH_MSG_DEBUG("retrieved TGC PRD Container with size " << tgcprdContainer->size());

        if (tgcprdContainer->size() == 0) ATH_MSG_DEBUG(" TGC PRD Container empty ");
        unsigned int n_PRD{0};
        for(auto it : *tgcprdContainer ) {
            const Muon::TgcPrepDataCollection*coll = it;
            for (auto prd: *coll) {
                Identifier Id = prd->identify();

                const MuonGM::TgcReadoutElement* det = MuonDetMgr->getTgcReadoutElement(Id);
                if (!det) {
                   ATH_MSG_ERROR("The TGC hit "<<idHelperSvc()->toString(Id)<<" does not have a detector element attached. That should actually never happen");
                   return false;
                }

                m_TGC_PRD_id.push_back(Id);
                Amg::Vector3D pos = prd->globalPosition();
                Amg::Vector2D loc_pos(0., 0.);

                det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

                m_TGC_PRD_globalPos.push_back(pos);

                m_TGC_PRD_localPosX.push_back(loc_pos[0]);
                m_TGC_PRD_localPosY.push_back(loc_pos[1]);

                ++n_PRD;
            }
        }
        m_TGC_nPRD = n_PRD;
        ATH_MSG_DEBUG(" finished fillTGCPRDVariables()");
        return true;
    }
}