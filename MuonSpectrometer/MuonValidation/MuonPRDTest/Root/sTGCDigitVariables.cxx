/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/sTGCDigitVariables.h"

#include "MuonReadoutGeometry/sTgcReadoutElement.h"

namespace MuonPRDTest {
    sTgcDigitVariables::sTgcDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "Digits_sTGC", true, msglvl), m_key{container_name} {}

    bool sTgcDigitVariables::declare_keys() { return declare_dependency(m_key); }
    bool sTgcDigitVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillsTGCDigitHitVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<sTgcDigitContainer> sTgcDigitContainer{m_key, ctx};
        if (!sTgcDigitContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve digit container " << m_key.fullKey());
            return false;
        }

        ATH_MSG_DEBUG("retrieved sTGC Digit Container with size " << sTgcDigitContainer->digit_size());

        if (sTgcDigitContainer->size() == 0) ATH_MSG_DEBUG(" sTGC Digit Container empty ");
        unsigned int n_digits{0};
        for (const sTgcDigitCollection* coll : *sTgcDigitContainer) {
            ATH_MSG_DEBUG("processing collection with size " << coll->size());
            for (unsigned int digitNum = 0; digitNum < coll->size(); digitNum++) {
                const sTgcDigit* digit = coll->at(digitNum);
                Identifier Id = digit->identify();

                const MuonGM::sTgcReadoutElement* rdoEl = MuonDetMgr->getsTgcReadoutElement(Id);
                if (!rdoEl) {
                    ATH_MSG_ERROR("sTGCDigitVariables::fillVariables() - Failed to retrieve sTGCReadoutElement for "<<idHelperSvc()->stgcIdHelper().print_to_string(Id).c_str());
                    return false;
                }
                m_NSWsTGC_dig_id.push_back(Id);
                Amg::Vector3D gpos{0., 0., 0.};
                Amg::Vector2D lpos(0., 0.);

                rdoEl->stripPosition(Id, lpos);
                rdoEl->surface(Id).localToGlobal(lpos, gpos, gpos);

                m_NSWsTGC_dig_globalPos.push_back(gpos);
                m_NSWsTGC_dig_localPosX.push_back(lpos.x());
                m_NSWsTGC_dig_localPosY.push_back(lpos.y());
                
                m_NSWsTGC_dig_channelNumber.push_back(rdoEl->stripNumber(lpos, Id));

                std::vector<Amg::Vector2D> local_pad_corners;
                rdoEl->padCorners(Id,local_pad_corners);
                std::vector<Amg::Vector3D> global_pad_corners;

                for(auto& local_corner : local_pad_corners) {
                    Amg::Vector3D global_corner;
                    rdoEl->surface(Id).localToGlobal(local_corner, global_corner, global_corner);
                    global_pad_corners.push_back(global_corner);
                }
                for(auto corner : global_pad_corners) {
                    if(idHelperSvc()->stgcIdHelper().channelType(Id) ==0 ) {
                        m_NSWsTGC_dig_PadglobalCornerPos.push_back(corner);
                    }
                }

                m_NSWsTGC_dig_channelPosX.push_back( lpos.x() );
                m_NSWsTGC_dig_channelPosY.push_back( lpos.y() );

                m_NSWsTGC_dig_bctag.push_back(digit->bcTag());
                m_NSWsTGC_dig_time.push_back(digit->time());
                m_NSWsTGC_dig_charge.push_back(digit->charge());
                m_NSWsTGC_dig_isDead.push_back(digit->isDead());
                m_NSWsTGC_dig_isPileup.push_back(digit->isPileup());

                ++n_digits;
            }
        }
        m_NSWsTGC_nDigits = n_digits;
        ATH_MSG_DEBUG(" finished fillsTgcDigitVariables()");
        return true;
    }
}