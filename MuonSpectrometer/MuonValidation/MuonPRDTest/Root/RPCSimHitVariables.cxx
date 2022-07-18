/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonPRDTest/RPCSimHitVariables.h"

#include "AtlasHepMC/GenParticle.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonSimEvent/RpcHitIdHelper.h"

namespace MuonPRDTest {
    RPCSimHitVariables::RPCSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "SIM_RPC", false, msglvl), m_key{container_name} {}

    bool RPCSimHitVariables::declare_keys() { return declare_dependency(m_key); }

    bool RPCSimHitVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fill RpcSDOVariable()");
        SG::ReadHandle<RPCSimHitCollection> rpcContainer{m_key, ctx};
        if (!rpcContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrive digit container " << m_key.fullKey());
            return false;
        }
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        unsigned int n_hits{0};
        // Get the RPC Id hit helper
        const RpcHitIdHelper* rpchhelper = RpcHitIdHelper::GetHelper();
        if (!rpcContainer->size()) ATH_MSG_DEBUG("RPC sdo container is empty");
        for (const RPCSimHit& hit : *rpcContainer) {
            HitID hitid = hit.RPCid();

            std::string stname = rpchhelper->GetStationName(hitid);
            int steta = rpchhelper->GetZSector(hitid);
            int stphi = rpchhelper->GetPhiSector(hitid);
            int dbr = rpchhelper->GetDoubletR(hitid);
            int dbz = rpchhelper->GetDoubletZ(hitid);
            int dbp = rpchhelper->GetDoubletPhi(hitid);
            int gg = rpchhelper->GetGasGapLayer(hitid);
            int mfi = rpchhelper->GetMeasuresPhi(hitid);

            bool isValid = false;
            // the last 2 arguments are:
            // bool check -> set to true for validity check of identifier
            // &bool isValid -> reference to boolean, which will be false in case of invalid identifier
            // the channelID() function of the RpcIdHelper needs also a stripNumber, but the RpcHitIdHelper
            // does not seem to return this, so we just give stripNumber=1 for now
            Identifier offid = idHelperSvc()->rpcIdHelper().channelID(stname, steta, stphi, dbr, dbz, dbp, gg, mfi, 1, isValid);
            if (!isValid) {
                ATH_MSG_WARNING("Cannot build a valid Identifier for RPC stationName="
                                << stname << ", eta=" << steta << ", phi=" << stphi << ", doubletR=" << dbr << ", doubletZ=" << dbz
                                << ", doubletPhi=" << dbp << ", gasGap=" << gg << ", measuresPhi=" << mfi << "; skipping...");
                continue;
            }

            const MuonGM::RpcReadoutElement* rpcdet = MuonDetMgr->getRpcReadoutElement(offid);
            if (!rpcdet) {
                ATH_MSG_ERROR("RPCSimHitVariables::fillVariables() - Failed to retrieve RpcReadoutElement for "
                              << idHelperSvc()->toString(offid));
                return false;
            }

            m_rpc_Id.push_back(offid);
            m_RPC_globalTime.push_back(hit.globalTime());
            const Amg::Vector3D& localPosition = hit.localPosition();
            m_RPC_hitLocalPositionX.push_back(localPosition.x());
            m_RPC_hitLocalPositionY.push_back(localPosition.y());
            Amg::Vector3D simHitPosGlo = rpcdet->localToGlobalCoords(localPosition, offid);
            m_RPC_hitGlobalPosition.push_back(simHitPosGlo);
            m_RPC_detector_globalPosition.push_back(rpcdet->globalPosition());
            m_RPC_particleEncoding.push_back(hit.particleEncoding());
            m_RPC_kineticEnergy.push_back(hit.kineticEnergy());
            m_RPC_depositEnergy.push_back(hit.energyDeposit());
            m_RPC_StepLength.push_back(hit.stepLength());

            int pdgId = -999;
            int barcode = -999;
            const HepMcParticleLink& pLink = hit.particleLink();
            barcode = pLink.barcode();
            if (pLink.isValid()) {
                HepMC::ConstGenParticlePtr genP = pLink.cptr();
                if (genP) {
                    pdgId = genP->pdg_id();
                    barcode = HepMC::barcode(genP);
                } else {
                    ATH_MSG_WARNING("GenParticle is nullptr for hit in "
                                    << stname << " (eta=" << steta << ", phi=" << stphi << ", doubletR=" << dbr << ", doubletZ=" << dbz
                                    << ", doubletPhi=" << dbp << ", gasgaplayer=" << gg << ", measuresPhi=" << mfi << ")");
                }
            } else {
                // if barcode is 0, the hit was not created by a particle generated in the GenEvent step, thus link cannot be valid
                if (msgLvl(barcode == 0 ? MSG::DEBUG : MSG::WARNING)) {
                    msg(barcode == 0 ? MSG::DEBUG : MSG::WARNING)
                        << "HepMcParticleLink with barcode=" << barcode << " is not valid for hit in " << stname << " (eta=" << steta
                        << ", phi=" << stphi << ", doubletR=" << dbr << ", doubletZ=" << dbz << ", doubletPhi=" << dbp
                        << ", gasgaplayer=" << gg << ", measuresPhi=" << mfi << ")" << endmsg;
                }
            }
            m_RPC_trackId.push_back(pdgId);
            m_RPC_truthEl.push_back(barcode);
            ++n_hits;
        }
        m_RPC_nSimHits = n_hits;

        ATH_MSG_DEBUG("processed " << m_RPC_nSimHits << " Rpc hits");
        return true;
    }

}  // namespace MuonPRDTest
