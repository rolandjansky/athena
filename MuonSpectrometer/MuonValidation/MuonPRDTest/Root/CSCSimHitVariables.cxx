/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/CSCSimHitVariables.h"

#include "AthenaKernel/errorcheck.h"
#include "AtlasHepMC/GenParticle.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonSimEvent/CscHitIdHelper.h"
namespace MuonPRDTest {
    CSCSimHitVariables::CSCSimHitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "CSC_Sim", false, msglvl),m_key{container_name} {}
    bool CSCSimHitVariables::declare_keys() { return declare_dependency(m_key); }

    bool CSCSimHitVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillCSCSimHitVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }

        SG::ReadHandle<CSCSimHitCollection> cscContainer{m_key, ctx};
        if (!cscContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve container " << m_key << ".");
            return false;
        }
        // Get the CSC Id hit helper
        const CscHitIdHelper* cschhelper = CscHitIdHelper::GetHelper();

        unsigned int numHits{0};
        for (const CSCSimHit& hit : *cscContainer) {
            HitID hitid = hit.CSCid();

		    std::string stname = cschhelper->GetStationName(hitid);
		    int         steta = cschhelper->GetZSector(hitid);
		    int         stphi = cschhelper->GetPhiSector(hitid);
		    int         clayer = cschhelper->GetChamberLayer(hitid);
		    int         wlayer = cschhelper->GetWireLayer(hitid);


            bool isValid = false;
            Identifier offid = idHelperSvc()->cscIdHelper().channelID(stname, steta, stphi, clayer, wlayer, 1, 1, isValid);
            if (!isValid) {
                ATH_MSG_WARNING("Cannot build a valid Identifier for CSC stationName=" << stname << ", eta=" << steta << ", phi=" << stphi
                                                                                       << ", chamberLayer=" << clayer << ", wireLayer=" << wlayer
                                                                                       << "; skipping...");
                continue;
            }

            const MuonGM::CscReadoutElement* cscdet = MuonDetMgr->getCscReadoutElement(offid);
            if (!cscdet) {
                ATH_MSG_ERROR("CSCSimHitVariables::fillVariables() - Failed to retrieve CscReadoutElement for "
                              << idHelperSvc()->toString(offid));
                return false;
            }

            m_CSC_id.push_back(offid);
            m_CSC_globalTime.push_back(hit.globalTime());

		    Amg::Vector3D simHitPosGlo = cscdet->localToGlobalCoords(hit.getHitStart(), offid);
		    m_CSC_hitGlobalPosition.push_back(simHitPosGlo);

		    Amg::Vector3D detpos = cscdet->globalPosition();		
		    m_CSC_detector_globalPosition.push_back(detpos);

		    m_CSC_kineticEnergy.push_back(hit.kineticEnergy());
		    m_CSC_depositEnergy.push_back(hit.energyDeposit());


            int pdgId{-999}, barcode{-999};
            const HepMcParticleLink& pLink = hit.particleLink();
            barcode = pLink.barcode();
            if (pLink.isValid()) {
                HepMC::ConstGenParticlePtr genP = pLink.cptr();
                if (genP) {
                    pdgId = genP->pdg_id();
                    barcode = HepMC::barcode(genP);
                } else {
                    ATH_MSG_WARNING("GenParticle is nullptr for hit in " << stname << " (eta=" << steta << ", phi=" << stphi << ", chamberlayer=" << clayer
                                                                         << ", wirelayer=" << wlayer  << ")");
                }
            } else {
                // if barcode is 0, the hit was not created by a particle generated in the GenEvent step, thus link cannot be valid
                if (msgLvl(barcode == 0 ? MSG::DEBUG : MSG::WARNING)) {
                    msg(barcode == 0 ? MSG::DEBUG : MSG::WARNING)
                        << "HepMcParticleLink with barcode=" << barcode << " is not valid for hit in " << stname << " (eta=" << steta
                        << ", phi=" << stphi << ", chamberlayer=" << clayer << ", wirelayer=" << wlayer << ")" << endmsg;
                }
            }
            m_CSC_trackId.push_back(pdgId);
            m_CSC_truthEl.push_back(barcode);
            ++numHits;
        }
        m_CSC_nSimHits = numHits;

        ATH_MSG_DEBUG("processed " << m_CSC_nSimHits << " Csc hits");
        return true;
    }
}
