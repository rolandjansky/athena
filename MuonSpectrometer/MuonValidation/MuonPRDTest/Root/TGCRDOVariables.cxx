/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/TGCRDOVariables.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonRDO/TgcRdo.h"

using namespace Muon;
namespace MuonPRDTest {
    TGCRDOVariables::TGCRDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl, const ITGCcablingSvc* cabling_svc) :
        PrdTesterModule(tree, "RDO_TGC", true, msglvl), m_key{container_name}, m_tgcCabling{cabling_svc} {}
    bool TGCRDOVariables::declare_keys() { return declare_dependency(m_key); }

    bool TGCRDOVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillTGCRDOVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<TgcRdoContainer> tgcrdoContainer{m_key, ctx};
        if (!tgcrdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve tgc rdo container " << m_key.fullKey());
            return false;
        }
        ATH_MSG_DEBUG("retrieved TGC rdo Container with size " << tgcrdoContainer->size());

        if (tgcrdoContainer->size() == 0) ATH_MSG_DEBUG(" TGC rdo Container empty ");
        unsigned int n_rdo{0};
        for (const TgcRdo* coll : *tgcrdoContainer) {
            ATH_MSG_DEBUG("processing collection with size " << coll->size());
            for (const TgcRawData* rdo: *coll) {

                bool orFlag = m_tgcCabling->isOredChannel(rdo->subDetectorId(),
                                                          rdo->rodId(),
                                                          rdo->sswId(),
                                                          rdo->slbId(),
                                                          rdo->bitpos());

                Identifier Id;
                bool e_found = m_tgcCabling->getElementIDfromReadoutID(Id,
                                             rdo->subDetectorId(),
                                             rdo->rodId(),
                                             rdo->sswId(),
                                             rdo->slbId(),
                                             rdo->bitpos(), 
                                             orFlag);
                if (!e_found) {
                    ATH_MSG_ERROR("could not find Identifier from TgcRawData");
                    return false;
                }

                const MuonGM::TgcReadoutElement* rdoEl = MuonDetMgr->getTgcReadoutElement(Id);
                if (!rdoEl) {
                   ATH_MSG_ERROR("The TGC hit "<<idHelperSvc()->toString(Id)<<" does not have a detector element attached. That should actually never happen");
                   return false;
                }

                m_TGC_rdo_id.push_back(Id);
                Amg::Vector2D localStripPos(0.,0.);
                if ( rdoEl->stripPosition(Id,localStripPos) )  {
                    m_TGC_rdo_localPosX.push_back(localStripPos.x());
                    m_TGC_rdo_localPosY.push_back(localStripPos.y());
                ATH_MSG_DEBUG("TGC RDO: local pos.:  x=" << localStripPos[0] << ",  y=" << localStripPos[1]);
                } else { 
                    ATH_MSG_WARNING("TGC RDO: local Strip position not defined"); 
                }
      
                // asking the detector element to transform this local to the global position
                Amg::Vector3D globalStripPos(0., 0., 0.);
                rdoEl->surface(Id).localToGlobal(localStripPos,Amg::Vector3D(0.,0.,0.),globalStripPos);
                m_TGC_rdo_globalPos.push_back(globalStripPos);

                ++n_rdo;
            }
        }
        m_TGC_nRDO = n_rdo;
        ATH_MSG_DEBUG(" finished fillTGCRDOVariables()");
        return true;
    }
}