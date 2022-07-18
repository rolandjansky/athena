/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/RPCDigitVariables.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"
namespace MuonPRDTest {
    RpcDigitVariables::RpcDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "Digits_RPC", true, msglvl), m_key{container_name} {}

    bool RpcDigitVariables::declare_keys() { return declare_dependency(m_key); }
    bool RpcDigitVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillMDTSimHitVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<RpcDigitContainer> RpcDigitContainer{m_key, ctx};
        if (!RpcDigitContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve digit container " << m_key.fullKey());
            return false;
        }

        ATH_MSG_DEBUG("retrieved RPC Digit Container with size " << RpcDigitContainer->digit_size());

        if (RpcDigitContainer->size() == 0) ATH_MSG_DEBUG(" RPC Digit Container empty ");
        unsigned int n_digits{0};
        for (const RpcDigitCollection* coll : *RpcDigitContainer) {
            ATH_MSG_DEBUG("processing collection with size " << coll->size());
            for (unsigned int digitNum = 0; digitNum < coll->size(); digitNum++) {
                const RpcDigit* digit = coll->at(digitNum);
                Identifier Id = digit->identify();

                ATH_MSG_DEBUG("RPC Digit Offline id:  " << idHelperSvc()->toString(Id));

                const MuonGM::RpcReadoutElement* rdoEl = MuonDetMgr->getRpcReadoutElement(Id);
                if (!rdoEl) {
                    ATH_MSG_ERROR("RPCDigitVariables::fillVariables() - Failed to retrieve PRCReadoutElement for "<<idHelperSvc()->rpcIdHelper().print_to_string(Id).c_str());
                    return false;
                }

                Amg::Vector3D gpos{0., 0., 0.};
                Amg::Vector2D lpos(0., 0.);

                const bool stripPosition = rdoEl->stripPosition(Id, lpos);
                int stripNumber = rdoEl->stripNumber(lpos, Id);

                if (!stripPosition) {
                    ATH_MSG_WARNING("RPCDigitVariables: failed to associate a valid local position for strip n. "
                                    << stripNumber << "; associated positions will be set to 0.0.");
                    continue;
                }

                rdoEl->surface(Id).localToGlobal(lpos, gpos, gpos);
                m_RPC_dig_globalPos.push_back(gpos);
                m_RPC_dig_localPosX.push_back(lpos.x());
                m_RPC_dig_localPosY.push_back(lpos.y());
                m_RPC_dig_time.push_back(digit->time());
                m_RPC_dig_id.push_back(Id);
                m_RPC_dig_stripNumber.push_back(stripNumber);

                ++n_digits;
            }
        }
        m_RPC_nDigits = n_digits;
        ATH_MSG_DEBUG(" finished fillRpcDigitVariables()");
        return true;
    }
}  // namespace MuonPRDTest
