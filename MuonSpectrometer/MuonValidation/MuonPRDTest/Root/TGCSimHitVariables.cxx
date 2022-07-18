/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/TGCSimHitVariables.h"

#include "AthenaKernel/errorcheck.h"
#include "AtlasHepMC/GenParticle.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonSimEvent/TgcHitIdHelper.h"
namespace MuonPRDTest {
    TGCSimHitVariables::TGCSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "TGC_Sim", false, msglvl),m_key{container_name} {}
    bool TGCSimHitVariables::declare_keys() { return declare_dependency(m_key); }

    bool TGCSimHitVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillTGCSimHitVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }

        SG::ReadHandle<TGCSimHitCollection> tgcContainer{m_key, ctx};
        if (!tgcContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve container " << m_key << ".");
            return false;
        }
        // Get the TGC Id hit helper
        const TgcHitIdHelper* tgchhelper = TgcHitIdHelper::GetHelper();

        unsigned int numHits{0};
        for (const TGCSimHit& hit : *tgcContainer) {
            HitID hitid = hit.TGCid();

            std::string stname = tgchhelper->GetStationName(hitid);
            int steta = tgchhelper->GetStationEta(hitid);
            int stphi = tgchhelper->GetStationPhi(hitid);
            int gasgap = tgchhelper->GetGasGap(hitid);

            bool isValid = false;
            Identifier offid = idHelperSvc()->tgcIdHelper().channelID(stname, steta, stphi, gasgap, 1, 1, isValid);
            if (!isValid) {
                ATH_MSG_WARNING("Cannot build a valid Identifier for TGC stationName=" << stname << ", eta=" << steta << ", phi=" << stphi
                                                                                       << ", gasgap=" <<gasgap <<  "; skipping...");
                continue;
            }

            const MuonGM::TgcReadoutElement* tgcdet = MuonDetMgr->getTgcReadoutElement(offid);
            if (!tgcdet) {
                ATH_MSG_ERROR("TGCSimHitVariables::fillVariables() - Failed to retrieve TgcReadoutElement for "
                              << idHelperSvc()->toString(offid));
                return false;
            }

            m_TGC_id.push_back(offid);
            m_TGC_globalTime.push_back(hit.globalTime());

            const Amg::Vector3D& localPosition = hit.localPosition();
            m_TGC_hitLocalPosition.push_back(localPosition);
            m_TGC_hitGlobalPosition.push_back(tgcdet->localToGlobalCoords(localPosition, offid));
            m_TGC_detector_globalPosition.push_back(tgcdet->globalPosition());

            m_TGC_particleEncoding.push_back(hit.particleEncoding());
            m_TGC_kineticEnergy.push_back(hit.kineticEnergy());
            m_TGC_depositEnergy.push_back(hit.energyDeposit());
            m_TGC_StepLength.push_back(hit.stepLength());

            int pdgId{-999}, barcode{-999};
            const HepMcParticleLink& pLink = hit.particleLink();
            barcode = pLink.barcode();
            if (pLink.isValid()) {
                HepMC::ConstGenParticlePtr genP = pLink.cptr();
                if (genP) {
                    pdgId = genP->pdg_id();
                    barcode = HepMC::barcode(genP);
                } else {
                    ATH_MSG_WARNING("GenParticle is nullptr for hit in " << stname << " (eta=" << steta << ", phi=" << stphi << ", gasgap=" << gasgap << ")");
                }
            } else {
                // if barcode is 0, the hit was not created by a particle generated in the GenEvent step, thus link cannot be valid
                if (msgLvl(barcode == 0 ? MSG::DEBUG : MSG::WARNING)) {
                    msg(barcode == 0 ? MSG::DEBUG : MSG::WARNING)
                        << "HepMcParticleLink with barcode=" << barcode << " is not valid for hit in " << stname << " (eta=" << steta
                        << ", phi=" << stphi << ", gasgap=" << gasgap << ")" << endmsg;
                }
            }
            m_TGC_trackId.push_back(pdgId);
            m_TGC_truthEl.push_back(barcode);
            ++numHits;
        }
        m_TGC_nSimHits = numHits;

        ATH_MSG_DEBUG("processed " << m_TGC_nSimHits << " Tgc hits");
        return true;
    }
}
