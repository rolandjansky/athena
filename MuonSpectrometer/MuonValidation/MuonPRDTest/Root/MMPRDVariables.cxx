/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/MMPRDVariables.h"
#include "MuonPRDTest/MMRDOVariables.h"

#include "MuonReadoutGeometry/MMReadoutElement.h"

namespace MuonPRDTest {
    MMPRDVariables::MMPRDVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "PRD_MM", true, msglvl), m_key{container_name} {}
    bool MMPRDVariables::declare_keys() { return m_key.initialize().isSuccess(); }

    bool MMPRDVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillMMPRDVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<Muon::MMPrepDataContainer> mmprdContainer{m_key, ctx};
        if (!mmprdContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve prd container " << m_key.fullKey());
            return false;
        }
        ATH_MSG_DEBUG("retrieved MM PRD Container with size " << mmprdContainer->size());

        if (mmprdContainer->size() == 0) ATH_MSG_DEBUG(" MM PRD Container empty ");

        SG::ReadHandle<Muon::MM_RawDataContainer> mmrdoContainer{m_rdokey, ctx};
        if (!mmrdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve rdo container " << m_rdokey.fullKey());
            return false;
        }
        ATH_MSG_DEBUG("retrieved MM RDO Container with size " << mmrdoContainer->size());

        if (mmrdoContainer->size() == 0) ATH_MSG_DEBUG(" MM RDO Container empty ");

        unsigned int n_PRD{0};
        for(const Muon::MMPrepDataCollection* coll : *mmprdContainer) {
            const Muon::MM_RawDataCollection* rdo_coll = mmrdoContainer->indexFindPtr(coll->identifyHash());

            if(rdo_coll==nullptr){
                ATH_MSG_ERROR("Did not find rdo collection " << coll->identifyHash());
                return false;
            }
            ATH_MSG_DEBUG("Found rdo_coll at "<<rdo_coll);
            for (unsigned int item=0; item<coll->size(); item++) {
                const Muon::MMPrepData* prd = coll->at(item);
                Identifier Id = prd->identify();

                m_NSWMM_PRD_time.push_back(prd->time());

                const MuonGM::MMReadoutElement* det = prd->detectorElement();;
                if (!det) {
                   ATH_MSG_ERROR("The micromega hit "<<idHelperSvc()->toString(Id)<<" does not have a detector element attached. That should actually never happen");
                   return false;
                }

                m_NSWMM_PRD_id.push_back(Id);
                Amg::Vector3D pos = prd->globalPosition();
                const Amg::MatrixX & cov = prd->localCovariance();
                Amg::Vector2D loc_pos(0., 0.);
                det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

                ATH_MSG_DEBUG(     "MicroMegas PRD local pos.:  x=" << std::setw(6) << std::setprecision(2) << loc_pos[0]
                                                                    << ", ex=" << std::setw(6) << std::setprecision(2) << cov(0,0)
                                                                    << ",  y=" << std::setw(6) << std::setprecision(2) << loc_pos[1] );

                m_NSWMM_PRD_globalPos.push_back(pos);

                m_NSWMM_PRD_localPosX.push_back(loc_pos[0]);
                m_NSWMM_PRD_localPosY.push_back(loc_pos[1]);
                m_NSWMM_PRD_covMatrix_1_1.push_back(cov(0,0));
                m_NSWMM_PRD_nRdos.push_back((prd->rdoList()).size());

                for(const Identifier &id_rdo:prd->rdoList()){
                    const Muon::MM_RawData* rdo=nullptr;
                    for(auto it :*rdo_coll){if(it->identify() == id_rdo) rdo=it;}
                    if(rdo==nullptr){
                        ATH_MSG_ERROR("Did not find rdo for identifier in rdo list");
                        return false;
                    }
                    m_NSWMM_PRD_rdos_charge.push_back(rdo->charge());
                    m_NSWMM_PRD_rdos_time.push_back(rdo->time());
                    m_NSWMM_PRD_rdos_channel.push_back(idHelperSvc()->mmIdHelper().channel(id_rdo));      
                }
                m_NSWMM_PRD_uTPCAngle.push_back(prd->angle());  
                m_NSWMM_PRD_uTPCChiSqProb.push_back(prd->chisqProb());
                
                ++n_PRD;
            }
        }
        m_NSWMM_nPRD = n_PRD;
        ATH_MSG_DEBUG(" finished fillMMPRDVariables()");
        return true;
    }
}