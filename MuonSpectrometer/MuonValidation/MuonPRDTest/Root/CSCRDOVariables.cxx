/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/CSCRDOVariables.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"

using namespace Muon;
namespace MuonPRDTest {
    CSCRDOVariables::CSCRDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl, const MuonIdHelper* idhelper, const Muon::ICSC_RDO_Decoder* rdo_decoder) :
        PrdTesterModule(tree, "RDO_CSC", true, msglvl), m_key{container_name},  m_rdo_decoder{rdo_decoder} {setHelper(idhelper);}
    bool CSCRDOVariables::declare_keys() { return declare_dependency(m_key); }

    bool CSCRDOVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillCSCRDOVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<CscRawDataContainer> cscrdoContainer{m_key, ctx};
        if (!cscrdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve csc rdo container " << m_key.fullKey());
            return false;
        }
        ATH_MSG_DEBUG("retrieved CSC rdo Container with size " << cscrdoContainer->size());

        if (cscrdoContainer->size() == 0) ATH_MSG_DEBUG(" CSC rdo Container empty ");
        unsigned int n_rdo{0};
        for (const CscRawDataCollection* coll : *cscrdoContainer) {
            ATH_MSG_DEBUG("processing collection with size " << coll->size());
            int strip_num{0};
            for (const CscRawData* rdo: *coll) {
                const Identifier Id { m_rdo_decoder->channelIdentifier(rdo,m_CscIdHelper,strip_num) };
                ++strip_num;

                const MuonGM::CscReadoutElement* rdoEl = MuonDetMgr->getCscReadoutElement(Id);
                if (!rdoEl) {
                   ATH_MSG_ERROR("The CSC hit "<<idHelperSvc()->toString(Id)<<" does not have a detector element attached. That should actually never happen");
                   return false;
                }

                m_CSC_rdo_time.push_back(rdo->time());

                Amg::Vector3D gpos{0., 0., 0.};
                Amg::Vector2D lpos(0., 0.);

                rdoEl->surface(Id).localToGlobal(lpos, gpos, gpos);
                m_CSC_rdo_globalPos.push_back(gpos);
                m_CSC_rdo_id.push_back(Id);

                ++n_rdo;
            }
            // Local RDO position information loss after localToGlobal transformation, fill the local positions in another loop for retrieving the local positions
            for (const CscRawData* rdo: *coll) {
                const Identifier Id { m_rdo_decoder->channelIdentifier(rdo,m_CscIdHelper,strip_num) };
                ++strip_num;

                const MuonGM::CscReadoutElement* rdoEl = MuonDetMgr->getCscReadoutElement(Id);
                if (!rdoEl) return false;

                Amg::Vector2D loPos(0.,0.);
                Amg::Vector3D glPos(0., 0., 0.);

                rdoEl->surface(Id).globalToLocal(glPos,glPos,loPos);
                m_CSC_rdo_localPosX.push_back(loPos.x());
                m_CSC_rdo_localPosY.push_back(loPos.y());
            }
        }
        m_CSC_nRDO = n_rdo;
        ATH_MSG_DEBUG(" finished fillCSCRDOVariables()");
        return true;
    }
}