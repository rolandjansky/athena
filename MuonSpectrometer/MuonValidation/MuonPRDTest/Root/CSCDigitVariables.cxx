/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/CSCDigitVariables.h"

#include "MuonReadoutGeometry/CscReadoutElement.h"
namespace MuonPRDTest {
    CscDigitVariables::CscDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "Digits_CSC", true, msglvl), m_key{container_name} {}
    bool CscDigitVariables::declare_keys() { return declare_dependency(m_key); }

    bool CscDigitVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillCSCDigitVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<CscDigitContainer> CscDigitContainer{m_key, ctx};
        if (!CscDigitContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve digit container " << m_key.fullKey());
            return false;
        }

        ATH_MSG_DEBUG("retrieved CSC Digit Container with size " << CscDigitContainer->digit_size());

        if (CscDigitContainer->size() == 0) ATH_MSG_DEBUG(" CSC Digit Container empty ");
        unsigned int n_digits{0};
        for (const CscDigitCollection* coll : *CscDigitContainer) {
            ATH_MSG_DEBUG("processing collection with size " << coll->size());
            for (unsigned int digitNum = 0; digitNum < coll->size(); digitNum++) {
                const CscDigit* digit = coll->at(digitNum);

                Identifier Id = digit->identify();
                ATH_MSG_DEBUG("CSC Digit Offline id:  " << idHelperSvc()->toString(Id));

                const MuonGM::CscReadoutElement* rdoEl = MuonDetMgr->getCscReadoutElement(Id);
                if (!rdoEl) {
                    ATH_MSG_ERROR("CSCDigitVariables::fillVariables() - Failed to retrieve CSCReadoutElement for "<<idHelperSvc()->cscIdHelper().print_to_string(Id).c_str());
                    return false;
                }

                Amg::Vector3D gpos{0., 0., 0.};
                Amg::Vector2D lpos(0., 0.);

                rdoEl->surface(Id).localToGlobal(lpos, gpos, gpos);
                m_CSC_dig_globalPos.push_back(gpos);
                m_CSC_dig_id.push_back(Id);

                ++n_digits;
            }
            // Local digit position information loss after localToGlobal transformation, fill the local positions in another loop for retrieving the local positions
            for (unsigned int digitNum = 0; digitNum < coll->size(); digitNum++) {
                const CscDigit* digit = coll->at(digitNum);
                Identifier Id = digit->identify();
                ATH_MSG_DEBUG("CSC Digit Offline id:  " << idHelperSvc()->toString(Id));

                const MuonGM::CscReadoutElement* rdoEl = MuonDetMgr->getCscReadoutElement(Id);
                if (!rdoEl) return false;

                Amg::Vector3D glpos{0., 0., 0.};
                Amg::Vector2D lopos(0., 0.);

                rdoEl->surface(Id).globalToLocal(glpos, glpos, lopos);
                m_CSC_dig_localPosX.push_back(lopos.x());
                m_CSC_dig_localPosX.push_back(lopos.y());
            }
        }
        m_CSC_nDigits = n_digits;
        ATH_MSG_DEBUG(" finished fillCscDigitVariables()");
        return true;
    }
}