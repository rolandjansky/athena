/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/CSCSDOVariables.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
namespace MuonPRDTest {
    CscSDOVariables::CscSDOVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "CSC_SDO", false, msglvl), m_key{container_name} {}

    bool CscSDOVariables::declare_keys() { return declare_dependency(m_key); }

    bool CscSDOVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fill CscSDOVariable()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<CscSimDataCollection> cscSdoContainer{m_key, ctx};
        if (!cscSdoContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve digit container " << m_key.fullKey());
            return false;
        }
        unsigned int n_sdo{0};
        for (const auto& coll : *cscSdoContainer) {
            const Identifier& id = coll.first;
            const CscSimData& csc_sdo = coll.second;
            m_csc_sdo_id.push_back(id);

            ATH_MSG_DEBUG("CSC SDO: " << idHelperSvc()->toString(id));

            ATH_MSG_DEBUG( "Get the truth deposits from the SDO." );
            std::vector<CscSimData::Deposit> deposits;
            csc_sdo.deposits(deposits);

            int    truth_barcode   = deposits[0].first.barcode();
            double truth_localPosX = deposits[0].second.zpos();
            double truth_localPosY = deposits[0].second.ypos();
            double truth_charge    = deposits[0].second.charge();

            m_csc_sdo_word.push_back( csc_sdo.word() );

            const MuonGM::CscReadoutElement* rdoEl = MuonDetMgr->getCscReadoutElement(id);
            if (!rdoEl) {
                ATH_MSG_ERROR("CSCSDOVariables::fillVariables() - Failed to retrieve CscReadoutElement for " << idHelperSvc()->toString(id));
                return false;
            }

            Amg::Vector2D hit_on_surface(truth_localPosX, truth_localPosY);
            Amg::Vector3D hit_gpos(0., 0., 0.);
            rdoEl->surface(id).localToGlobal(hit_on_surface, Amg::Vector3D(0., 0., 0.), hit_gpos);

            if ( rdoEl->surface(id).insideBounds(hit_on_surface,0.,0.) ){
                m_csc_sdo_localPosX.push_back(truth_localPosX);
                m_csc_sdo_localPosY.push_back(truth_localPosY);
            }

            if ( rdoEl->surface(id).isOnSurface(hit_gpos,true,0.,0.) ){
                m_CSC_dig_globalPos.push_back(hit_gpos);
            }

            ATH_MSG_DEBUG("CSC SDO barcode=" << truth_barcode);
            ATH_MSG_DEBUG("CSC SDO localPosX=" << std::setw(9) << std::setprecision(2) << truth_localPosX
                                               << ", localPosY=" << std::setw(9) << std::setprecision(2) << truth_localPosY
                                               << ", truth charge=" << std::setw(8) << std::setprecision(5) << truth_charge);

            m_csc_sdo_barcode.push_back(truth_barcode);
            m_csc_sdo_charge.push_back( truth_charge );

            ++n_sdo;
        }
        m_csc_nsdo = n_sdo;

        ATH_MSG_DEBUG("Processed " << n_sdo << " CSC SDOs");
        return true;
    }
}