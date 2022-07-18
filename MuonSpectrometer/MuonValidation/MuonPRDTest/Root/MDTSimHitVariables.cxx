/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/MDTSimHitVariables.h"

#include "AthenaKernel/errorcheck.h"
#include "AtlasHepMC/GenParticle.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonSimEvent/MdtHitIdHelper.h"
namespace MuonPRDTest {
    MDTSimHitVariables::MDTSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "MDT_Sim", false, msglvl), m_simHitKey{container_name} {}
    bool MDTSimHitVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillMDTSimHitVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }

        SG::ReadHandle<MDTSimHitCollection> mdtContainer{m_simHitKey, ctx};
        if (!mdtContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve container " << m_simHitKey << ".");
            return false;
        }
        // Get the MDT Id hit helper
        const MdtHitIdHelper* mdthhelper = MdtHitIdHelper::GetHelper(idHelperSvc()->mdtIdHelper().tubeMax());

        unsigned int numHits{0};
        for (const MDTSimHit& hit : *mdtContainer) {
            HitID hitid = hit.MDTid();

            std::string stname = mdthhelper->GetStationName(hitid);
            int steta = mdthhelper->GetZSector(hitid);
            int stphi = mdthhelper->GetPhiSector(hitid);
            int ml = mdthhelper->GetMultiLayer(hitid);
            int tl = mdthhelper->GetLayer(hitid);
            int tube = mdthhelper->GetTube(hitid);

            bool isValid = false;
            Identifier offid = idHelperSvc()->mdtIdHelper().channelID(stname, steta, stphi, ml, tl, tube, isValid);
            if (!isValid) {
                ATH_MSG_WARNING("Cannot build a valid Identifier for MDT stationName=" << stname << ", eta=" << steta << ", phi=" << stphi
                                                                                       << ", multiLayer=" << ml << ", tubeLayer=" << tl
                                                                                       << ", tube=" << tube << "; skipping...");
                continue;
            }

            const MuonGM::MdtReadoutElement* mdtdet = MuonDetMgr->getMdtReadoutElement(offid);
            if (!mdtdet) {
                ATH_MSG_ERROR("MDTSimHitVariables::fillVariables() - Failed to retrieve MdtReadoutElement for "
                              << idHelperSvc()->toString(offid));
                return false;
            }

            m_MDT_id.push_back(offid);
            m_MDT_globalTime.push_back(hit.globalTime());

            const Amg::Vector3D& localPosition = hit.localPosition();
            m_MDT_hitLocalPosition.push_back(localPosition);
            m_MDT_hitGlobalPosition.push_back(mdtdet->localToGlobalCoords(localPosition, offid));
            m_MDT_detector_globalPosition.push_back(mdtdet->globalPosition());
            m_MDT_driftRadius.push_back(hit.driftRadius());
            m_MDT_particleEncoding.push_back(hit.particleEncoding());
            m_MDT_kineticEnergy.push_back(hit.kineticEnergy());
            m_MDT_depositEnergy.push_back(hit.energyDeposit());
            m_MDT_StepLength.push_back(hit.stepLength());

            int pdgId{-999}, barcode{-999};
            const HepMcParticleLink& pLink = hit.particleLink();
            barcode = pLink.barcode();
            if (pLink.isValid()) {
                HepMC::ConstGenParticlePtr genP = pLink.cptr();
                if (genP) {
                    pdgId = genP->pdg_id();
                    barcode = HepMC::barcode(genP);
                } else {
                    ATH_MSG_WARNING("GenParticle is nullptr for hit in " << stname << " (eta=" << steta << ", phi=" << stphi << ", ml="
                                                                         << ml << ", tubelayer=" << tl << ", tube=" << tube << ")");
                }
            } else {
                // if barcode is 0, the hit was not created by a particle generated in the GenEvent step, thus link cannot be valid
                if (msgLvl(barcode == 0 ? MSG::DEBUG : MSG::WARNING)) {
                    msg(barcode == 0 ? MSG::DEBUG : MSG::WARNING)
                        << "HepMcParticleLink with barcode=" << barcode << " is not valid for hit in " << stname << " (eta=" << steta
                        << ", phi=" << stphi << ", ml=" << ml << ", tubelayer=" << tl << ", tube=" << tube << ")" << endmsg;
                }
            }
            m_MDT_trackId.push_back(pdgId);
            m_MDT_truthEl.push_back(barcode);
            ++numHits;
        }
        m_MDT_nSimHits = numHits;
        ATH_MSG_DEBUG("processed " << m_MDT_nSimHits << " Mdt hits");
        return true;
    }
    bool MDTSimHitVariables::declare_keys() { return declare_dependency(m_simHitKey); }
}  // namespace MuonPRDTest
