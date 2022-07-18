/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/MMRDOVariables.h"

#include "MuonReadoutGeometry/MMReadoutElement.h"

using namespace Muon;
namespace MuonPRDTest {
    MMRDOVariables::MMRDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "RDO_MM", true, msglvl), m_rdokey{container_name} {}
    bool MMRDOVariables::declare_keys() { return declare_dependency(m_rdokey); }

    bool MMRDOVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillMMRDOVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<MM_RawDataContainer> mmrdoContainer{m_rdokey, ctx};
        if (!mmrdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve MM rdo container " << m_rdokey.fullKey());
            return false;
        }
        ATH_MSG_DEBUG("retrieved MM rdo Container with size " << mmrdoContainer->size());

        if (mmrdoContainer->size() == 0) ATH_MSG_DEBUG(" MM rdo Container empty ");
        unsigned int n_rdo{0};
        for (const MM_RawDataCollection* coll : *mmrdoContainer) {
            for (unsigned int item=0; item<coll->size(); item++) {
                const MM_RawData* rdo = coll->at(item);
                Identifier Id = rdo->identify();

                m_NSWMM_rdo_time.push_back(rdo->time());
                m_NSWMM_rdo_relBcid.push_back(rdo->relBcid());
                m_NSWMM_rdo_charge.push_back(rdo->charge());

                const MuonGM::MMReadoutElement* rdoEl = MuonDetMgr->getMMReadoutElement(Id);
                if (!rdoEl) {
                   ATH_MSG_ERROR("The micromega hit "<<idHelperSvc()->toString(Id)<<" does not have a detector element attached. That should actually never happen");
                   return false;
                }

                m_NSWMM_rdo_id.push_back(Id);
                Amg::Vector2D localStripPos(0.,0.);
                if ( rdoEl->stripPosition(Id,localStripPos) )  {
                    m_NSWMM_rdo_localPosX.push_back(localStripPos.x());
                    m_NSWMM_rdo_localPosY.push_back(localStripPos.y());
                    ATH_MSG_DEBUG("MM RDO: local pos.:  x=" << localStripPos[0] << ",  y=" << localStripPos[1]);
                } else { 
                    ATH_MSG_WARNING("MM RDO: local Strip position not defined"); 
                }

                Amg::Vector3D globalStripPos(0., 0., 0.);
                rdoEl->surface(Id).localToGlobal(localStripPos,Amg::Vector3D(0.,0.,0.),globalStripPos);
                m_NSWMM_rdo_globalPos.push_back(globalStripPos);

                ++n_rdo;
            }
        }
        m_NSWMM_nRDO = n_rdo;
        ATH_MSG_DEBUG(" finished fillMMRDOVariables()");
        return true;
    }
}