/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/sTGCRDOVariables.h"

#include "MuonReadoutGeometry/sTgcReadoutElement.h"

using namespace Muon;
namespace MuonPRDTest {
    sTGCRDOVariables::sTGCRDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "RDO_sTGC", true, msglvl), m_key{container_name} {}
    bool sTGCRDOVariables::declare_keys() { return declare_dependency(m_key); }

    bool sTGCRDOVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillsTGCRDOVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<STGC_RawDataContainer> stgcrdoContainer{m_key, ctx};
        if (!stgcrdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve stgc rdo container " << m_key.fullKey());
            return false;
        }
        ATH_MSG_DEBUG("retrieved sTGC rdo Container with size " << stgcrdoContainer->size());

        if (stgcrdoContainer->size() == 0) ATH_MSG_DEBUG(" sTGC rdo Container empty ");
        unsigned int n_rdo{0};
        for (const STGC_RawDataCollection* coll : *stgcrdoContainer) {
            for (unsigned int item=0; item<coll->size(); item++) {
                const STGC_RawData* rdo = coll->at(item);
                Identifier Id = rdo->identify();

                m_NSWsTGC_rdo_time.push_back(rdo->time());
                m_NSWsTGC_rdo_tdo.push_back(rdo->tdo());
                m_NSWsTGC_rdo_charge.push_back(rdo->charge());
                m_NSWsTGC_rdo_bcTag.push_back(rdo->bcTag());
                m_NSWsTGC_rdo_isDead.push_back(rdo->isDead());

                const MuonGM::sTgcReadoutElement* rdoEl = MuonDetMgr->getsTgcReadoutElement(Id);
                if (!rdoEl) {
                   ATH_MSG_ERROR("The sTGC hit "<<idHelperSvc()->toString(Id)<<" does not have a detector element attached. That should actually never happen");
                   return false;
                }

                m_NSWsTGC_rdo_id.push_back(Id);
                Amg::Vector2D localStripPos(0.,0.);
                if ( rdoEl->stripPosition(Id,localStripPos) )  {
                    m_NSWsTGC_rdo_localPosX.push_back(localStripPos.x());
                    m_NSWsTGC_rdo_localPosY.push_back(localStripPos.y());
                    ATH_MSG_DEBUG("sTGC RDO: local pos.:  x=" << localStripPos[0] << ",  y=" << localStripPos[1]);
                } else { 
                    ATH_MSG_WARNING("sTGC RDO: local Strip position not defined"); 
                }

                Amg::Vector3D globalStripPos(0., 0., 0.);
                rdoEl->surface(Id).localToGlobal(localStripPos,Amg::Vector3D(0.,0.,0.),globalStripPos);
                m_NSWsTGC_rdo_globalPos.push_back(globalStripPos);

                ++n_rdo;
            }
        }
        m_NSWsTGC_nRDO = n_rdo;
        ATH_MSG_DEBUG(" finished fillsTGCRDOVariables()");
        return true;
    }
}