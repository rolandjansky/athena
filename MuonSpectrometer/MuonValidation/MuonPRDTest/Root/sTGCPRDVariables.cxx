/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/sTGCPRDVariables.h"

#include "MuonReadoutGeometry/sTgcReadoutElement.h"

namespace MuonPRDTest {
    sTGCPRDVariables::sTGCPRDVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "PRD_sTGC", true, msglvl), m_key{container_name} {}
    bool sTGCPRDVariables::declare_keys() { return declare_dependency(m_key); }

    bool sTGCPRDVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillsTGCPRDVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<Muon::sTgcPrepDataContainer> stgcprdContainer{m_key, ctx};
        if (!stgcprdContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve prd container " << m_key.fullKey());
            return false;
        }

        ATH_MSG_DEBUG("retrieved sTGC PRD Container with size " << stgcprdContainer->size());

        if (stgcprdContainer->size() == 0) ATH_MSG_DEBUG(" sTGC PRD Container empty ");
        unsigned int n_PRD{0};
        for(const Muon::sTgcPrepDataCollection* coll : *stgcprdContainer ) {
            for (unsigned int item=0; item<coll->size(); item++) {
                const Muon::sTgcPrepData* prd = coll->at(item);
                Identifier Id = prd->identify();

                m_NSWsTGC_PRD_id.push_back(Id);
                m_NSWsTGC_PRD_charge.push_back(prd->charge());
                m_NSWsTGC_PRD_bcTag.push_back(prd->getBcBitMap());

                const MuonGM::sTgcReadoutElement* det = prd->detectorElement();
                if (!det) {
                   ATH_MSG_ERROR("The sTGC hit "<<idHelperSvc()->toString(Id)<<" does not have a detector element attached. That should actually never happen");
                   return false;
                }

                Amg::Vector3D pos = prd->globalPosition();
                Amg::Vector2D loc_pos(0., 0.);
                det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

                double err_x = prd->localCovariance()(0,0);
                double err_y = ( prd->localCovariance().rows()==2)? prd->localCovariance()(1,1) : 0.;

                ATH_MSG_DEBUG( "sTgc PRD local pos.:  x=" << std::setw(6) << std::setprecision(2) << loc_pos[0]
                                               << ", ex=" << std::setw(6) << std::setprecision(2) << err_x
                                               << ",  y=" << std::setw(6) << std::setprecision(2) << loc_pos[1] 
                                               << ", ey=" << std::setw(6) << std::setprecision(2) << err_y );

                m_NSWsTGC_PRD_globalPos.push_back(pos);

                m_NSWsTGC_PRD_localPosX.push_back(loc_pos[0]);
                m_NSWsTGC_PRD_localPosY.push_back(loc_pos[1]);
                m_NSWsTGC_PRD_covMatrix_1_1.push_back(err_x);
                m_NSWsTGC_PRD_covMatrix_2_2.push_back(err_y);

                ++n_PRD;
            }
        }
        m_NSWsTGC_nPRD = n_PRD;
        ATH_MSG_DEBUG(" finished fillsTGCPRDVariables()");
        return true;
    }
}