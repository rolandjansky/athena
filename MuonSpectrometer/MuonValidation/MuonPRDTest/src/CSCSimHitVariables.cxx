/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCSimHitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/CscHitIdHelper.h"

#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "AtlasHepMC/GenParticle.h"

#include "TTree.h"

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode CSCSimHitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{

	ATH_MSG_DEBUG("do fillCSCSimHitVariables()");

	CHECK(this->clearVariables());

	const CSCSimHitCollection *cscContainer = nullptr;
	CHECK(m_evtStore->retrieve(cscContainer, m_ContainerName.c_str()));

	// Get the CSC Id hit helper
	CscHitIdHelper* cschhelper = CscHitIdHelper::GetHelper();

	if (!cscContainer->size()) ATH_MSG_DEBUG(m_ContainerName<<" container empty");
        for (const CSCSimHit& hit : *cscContainer) {

		HitID hitid = hit.CSCid();

		std::string stname = cschhelper->GetStationName(hitid);
		int         steta = cschhelper->GetZSector(hitid);
		int         stphi = cschhelper->GetPhiSector(hitid);
		int         clayer = cschhelper->GetChamberLayer(hitid);
		int         wlayer = cschhelper->GetWireLayer(hitid);

		
		bool isValid = false;
		// the last 2 arguments are:
		// bool check -> set to true for validity check of identifier
		// &bool isValid -> reference to boolean, which will be false in case of invalid identifier
		// the channelID() function of the CscIdHelper needs also a measuresPhi and a stripNumber, but the CscHitIdHelper 
		// does not seem to return this, so we just give measuresPhi=0 and stripNumber=1 for now
		Identifier offid = m_CscIdHelper->channelID(stname, steta, stphi, clayer, wlayer, 0, 1, true, &isValid);
		if (!isValid) {
            ATH_MSG_WARNING("Cannot build a valid Identifier for CSC stationName="<<stname<<", eta="<<steta<<", phi="<<stphi<<", chamberLayer="<<clayer<<", wireLayer="<<wlayer<<"; skipping...");
			continue;
		}

		const MuonGM::CscReadoutElement* cscdet = MuonDetMgr->getCscReadoutElement(offid);
		if (!cscdet) {
			ATH_MSG_ERROR("CSCSimHitVariables::fillVariables() - Failed to retrieve CscReadoutElement for "<<m_CscIdHelper->print_to_string(offid).c_str());
			return StatusCode::FAILURE;
		}

		
		m_CSC_Sim_stationName.push_back(stname);
		m_CSC_stationName.push_back(m_CscIdHelper->stationName(offid));
		m_CSC_stationEta.push_back(m_CscIdHelper->stationEta(offid));
		m_CSC_stationPhi.push_back(m_CscIdHelper->stationPhi(offid));
		m_CSC_isEndcap.push_back(m_CscIdHelper->isEndcap(offid));
		m_CSC_strip.push_back(m_CscIdHelper->strip(offid));
		m_CSC_Sim_stationEta.push_back(steta);
		m_CSC_Sim_stationPhi.push_back(stphi);
		m_CSC_Sim_chamberLayer.push_back(clayer);
		m_CSC_Sim_wireLayer.push_back(wlayer);

		m_CSC_globalTime.push_back(hit.globalTime());

		Amg::Vector3D simHitPosGlo = cscdet->localToGlobalCoords(hit.getHitStart(), offid);

		m_CSC_hitGlobalPositionX.push_back(simHitPosGlo.x());
		m_CSC_hitGlobalPositionY.push_back(simHitPosGlo.y());
		m_CSC_hitGlobalPositionZ.push_back(simHitPosGlo.z());
		m_CSC_hitGlobalPositionR.push_back(simHitPosGlo.perp());
		m_CSC_hitGlobalPositionP.push_back(simHitPosGlo.phi());

		Amg::Vector3D detpos = cscdet->globalPosition();
		
		m_CSC_detector_globalPositionX.push_back(detpos.x());
		m_CSC_detector_globalPositionY.push_back(detpos.y());
		m_CSC_detector_globalPositionZ.push_back(detpos.z());
		m_CSC_detector_globalPositionR.push_back(detpos.perp());
		m_CSC_detector_globalPositionP.push_back(detpos.phi());

		m_CSC_kineticEnergy.push_back(hit.kineticEnergy());
		m_CSC_depositEnergy.push_back(hit.energyDeposit());

		int pdgId = -999;
		int barcode = -999;
		const HepMcParticleLink& pLink = hit.particleLink();
		barcode = pLink.barcode();
		if (pLink.isValid()) {
			const HepMC::GenParticle* genP = pLink.cptr();
			if (genP) {
				pdgId = genP->pdg_id();
				barcode = HepMC::barcode(genP);
			} else {
				ATH_MSG_WARNING("GenParticle is nullptr for hit in "<<stname<<" (eta="<<steta<<", phi="<<stphi<<", chamberLayer="<< clayer<<", wireLayer="<<wlayer<<")");
			}
		} else {
			// if barcode is 0, the hit was not created by a particle generated in the GenEvent step, thus link cannot be valid
			if (msgLvl(barcode==0 ? MSG::DEBUG : MSG::WARNING)) {
				msg(barcode==0 ? MSG::DEBUG : MSG::WARNING)<<"HepMcParticleLink with barcode="<<barcode<<" is not valid for hit in "<<stname<<" (eta="<<steta<<", phi="<<stphi<<", chamberLayer="<< clayer<<", wireLayer="<<wlayer<<")"<<endmsg;
			}
		}

		m_CSC_trackId.push_back(pdgId);
		m_CSC_truthEl.push_back(barcode);

		++m_CSC_nSimHits;
	}

	ATH_MSG_DEBUG("processed " << m_CSC_nSimHits << " Csc hits");
	return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode CSCSimHitVariables::clearVariables()
{
	m_CSC_Sim_stationName.clear();
	m_CSC_stationName.clear();
	m_CSC_stationEta.clear();
	m_CSC_stationPhi.clear();
	m_CSC_isEndcap.clear();
	m_CSC_strip.clear();
	m_CSC_Sim_stationEta.clear();
	m_CSC_Sim_stationPhi.clear();
	m_CSC_Sim_chamberLayer.clear();
	m_CSC_Sim_wireLayer.clear();
	m_CSC_globalTime.clear();
	m_CSC_hitGlobalPositionX.clear();
	m_CSC_hitGlobalPositionY.clear();
	m_CSC_hitGlobalPositionZ.clear();
	m_CSC_hitGlobalPositionR.clear();
	m_CSC_hitGlobalPositionP.clear();
	m_CSC_detector_globalPositionX.clear();
	m_CSC_detector_globalPositionY.clear();
	m_CSC_detector_globalPositionZ.clear();
	m_CSC_detector_globalPositionR.clear();
	m_CSC_detector_globalPositionP.clear();
	m_CSC_kineticEnergy.clear();
	m_CSC_depositEnergy.clear();
	m_CSC_trackId.clear();
	m_CSC_truthEl.clear();
	m_CSC_nSimHits = 0;
	return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode CSCSimHitVariables::initializeVariables()
{
	if (m_tree) {
		m_tree->Branch("CSC_Sim_stationName", &m_CSC_Sim_stationName);
		m_tree->Branch("CSC_stationName", &m_CSC_stationName);
		m_tree->Branch("CSC_stationEta", &m_CSC_stationEta);
		m_tree->Branch("CSC_stationPhi", &m_CSC_stationPhi);
		m_tree->Branch("CSC_isEndcap", &m_CSC_isEndcap);
		m_tree->Branch("CSC_strip", &m_CSC_strip);
		m_tree->Branch("CSC_Sim_stationEta", &m_CSC_Sim_stationEta);
		m_tree->Branch("CSC_Sim_stationPhi", &m_CSC_Sim_stationPhi);
		m_tree->Branch("CSC_Sim_chamberLayer", &m_CSC_Sim_chamberLayer);
		m_tree->Branch("CSC_Sim_wireLayer", &m_CSC_Sim_wireLayer);
		m_tree->Branch("CSC_globalTime", &m_CSC_globalTime);
		m_tree->Branch("CSC_hitGlobalPositionX", &m_CSC_hitGlobalPositionX);
		m_tree->Branch("CSC_hitGlobalPositionY", &m_CSC_hitGlobalPositionY);
		m_tree->Branch("CSC_hitGlobalPositionZ", &m_CSC_hitGlobalPositionZ);
		m_tree->Branch("CSC_hitGlobalPositionR", &m_CSC_hitGlobalPositionR);
		m_tree->Branch("CSC_hitGlobalPositionP", &m_CSC_hitGlobalPositionP);
		m_tree->Branch("CSC_detector_globalPositionX", &m_CSC_detector_globalPositionX);
		m_tree->Branch("CSC_detector_globalPositionY", &m_CSC_detector_globalPositionY);
		m_tree->Branch("CSC_detector_globalPositionZ", &m_CSC_detector_globalPositionZ);
		m_tree->Branch("CSC_detector_globalPositionR", &m_CSC_detector_globalPositionR);
		m_tree->Branch("CSC_detector_globalPositionP", &m_CSC_detector_globalPositionP);
		m_tree->Branch("CSC_kineticEnergy", &m_CSC_kineticEnergy);
		m_tree->Branch("CSC_depositEnergy", &m_CSC_depositEnergy);
		m_tree->Branch("CSC_trackId", &m_CSC_trackId);
		m_tree->Branch("CSC_truthEl", &m_CSC_truthEl);
		m_tree->Branch("CSC_nSimHits", &m_CSC_nSimHits);
	}
	return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called in the destructor */
void CSCSimHitVariables::deleteVariables()
{
	return;
}
