/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCSimHitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/TgcHitIdHelper.h"

#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "AtlasHepMC/GenParticle.h"

#include "TTree.h"
#include <TString.h> // for Form

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode TGCSimHitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{

	ATH_MSG_DEBUG("do fillTGCSimHitVariables()");

	CHECK(this->clearVariables());

	const TGCSimHitCollection *tgcContainer = nullptr;
	CHECK(m_evtStore->retrieve(tgcContainer, m_ContainerName.c_str()));

	// Get the TGC Id hit helper
	TgcHitIdHelper* tgchhelper = TgcHitIdHelper::GetHelper();

	if (tgcContainer->size() == 0) ATH_MSG_WARNING(" TgcSimHit empty ");
	for (auto it : *tgcContainer) {
		const TGCSimHit hit = it;

		HitID hitid = hit.TGCid();

		std::string stname = tgchhelper->GetStationName(hitid);
		int         gasgap = tgchhelper->GetGasGap(hitid);
		int         steta = tgchhelper->GetStationEta(hitid);
		int         stphi = tgchhelper->GetStationPhi(hitid);

		bool isValid = false;
		// the last 2 arguments are:
		// bool check -> set to true for validity check of identifier
		// &bool isValid -> reference to boolean, which will be false in case of invalid identifier
		// the channelID() function of the TgcIdHelper needs also a channelNumber and a stripNumber, but the TgcHitIdHelper 
		// does not seem to return this, so we just give channelNumber=1 and stripNumber=1 for now
		Identifier offid = m_TgcIdHelper->channelID(stname, steta, stphi, gasgap, 1, 1, true, &isValid);
		if (!isValid) {
			ATH_MSG_WARNING(" Cannot build a valid Identifier; skip ");
			continue;
		}

		const MuonGM::TgcReadoutElement* tgcdet = MuonDetMgr->getTgcReadoutElement(offid);
		if (!tgcdet) {
			ATH_MSG_ERROR("TGCSimHitVariables::fillVariables() - Failed to retrieve TgcReadoutElement for" << __FILE__ << __LINE__ << m_TgcIdHelper->print_to_string(offid).c_str());
			return StatusCode::FAILURE;
		}

		m_TGC_Sim_stationName->push_back(stname);
		m_TGC_stationName->push_back(m_TgcIdHelper->stationName(offid));
		m_TGC_stationEta->push_back(m_TgcIdHelper->stationEta(offid));
		m_TGC_stationPhi->push_back(m_TgcIdHelper->stationPhi(offid));
		m_TGC_isEndcap->push_back(m_TgcIdHelper->isEndcap(offid));
		m_TGC_gasGap->push_back(m_TgcIdHelper->gasGap(offid));
		m_TGC_isStrip->push_back(m_TgcIdHelper->isStrip(offid));
		m_TGC_measuresPhi->push_back(m_TgcIdHelper->measuresPhi(offid));
		m_TGC_channel->push_back(m_TgcIdHelper->channel(offid));

		m_TGC_Sim_gasGap->push_back(gasgap);
		
		m_TGC_globalTime->push_back(hit.globalTime());

		const Amg::Vector3D localPosition = hit.localPosition();

		m_TGC_hitLocalPositionX->push_back(localPosition.x());
		m_TGC_hitLocalPositionY->push_back(localPosition.y());
		m_TGC_hitLocalPositionZ->push_back(localPosition.z());

		Amg::Vector3D simHitPosLoc(localPosition.x(), localPosition.y(), localPosition.z());
		Amg::Vector3D simHitPosGlo = tgcdet->localToGlobalCoords(simHitPosGlo, offid);

		m_TGC_hitGlobalPositionX->push_back(simHitPosGlo.x());
		m_TGC_hitGlobalPositionY->push_back(simHitPosGlo.y());
		m_TGC_hitGlobalPositionZ->push_back(simHitPosGlo.z());
		m_TGC_hitGlobalPositionR->push_back(simHitPosGlo.perp());
		m_TGC_hitGlobalPositionP->push_back(simHitPosGlo.phi());

		Amg::Vector3D detpos = tgcdet->globalPosition();

		m_TGC_detector_globalPositionX->push_back(detpos.x());
		m_TGC_detector_globalPositionY->push_back(detpos.y());
		m_TGC_detector_globalPositionZ->push_back(detpos.z());
		m_TGC_detector_globalPositionR->push_back(detpos.perp());
		m_TGC_detector_globalPositionP->push_back(detpos.phi());

		m_TGC_particleEncoding->push_back(hit.particleEncoding());
		m_TGC_kineticEnergy->push_back(hit.kineticEnergy());
		m_TGC_depositEnergy->push_back(hit.energyDeposit());
		m_TGC_StepLength->push_back(hit.stepLength());

		int pdgId = -999;
		int barcode = -999;
		const HepMcParticleLink& pLink = hit.particleLink();
		if (pLink.isValid()) {
			const HepMC::GenParticle* genP = pLink.cptr();
			if (genP) {
				pdgId = genP->pdg_id();
				barcode = HepMC::barcode(genP);
			}
		}
		m_TGC_trackId->push_back(pdgId);
		m_TGC_truthEl->push_back(barcode);

		++m_TGC_nSimHits;
	}

	ATH_MSG_DEBUG("processed " << m_TGC_nSimHits << " Tgc hits");
	return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode TGCSimHitVariables::clearVariables()
{

	m_TGC_Sim_stationName->clear();
	m_TGC_stationName->clear();
	m_TGC_stationEta->clear();
	m_TGC_stationPhi->clear();
	m_TGC_isEndcap->clear();
	m_TGC_Sim_gasGap->clear();
	m_TGC_gasGap->clear();
	m_TGC_isStrip->clear();
	m_TGC_measuresPhi->clear();
	m_TGC_channel->clear();
	m_TGC_globalTime->clear();
	m_TGC_hitLocalPositionX->clear();
	m_TGC_hitLocalPositionY->clear();
	m_TGC_hitLocalPositionZ->clear();
	m_TGC_hitGlobalPositionX->clear();
	m_TGC_hitGlobalPositionY->clear();
	m_TGC_hitGlobalPositionZ->clear();
	m_TGC_hitGlobalPositionR->clear();
	m_TGC_hitGlobalPositionP->clear();
	m_TGC_detector_globalPositionX->clear();
	m_TGC_detector_globalPositionY->clear();
	m_TGC_detector_globalPositionZ->clear();
	m_TGC_detector_globalPositionR->clear();
	m_TGC_detector_globalPositionP->clear();
	m_TGC_particleEncoding->clear();
	m_TGC_kineticEnergy->clear();
	m_TGC_depositEnergy->clear();
	m_TGC_StepLength->clear();
	m_TGC_trackId->clear();
	m_TGC_truthEl->clear();
	m_TGC_nSimHits = 0;


	return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode TGCSimHitVariables::initializeVariables()
{
	m_TGC_Sim_stationName = new std::vector<std::string>;
	m_TGC_stationName = new std::vector<int>;
	m_TGC_stationEta = new std::vector<int>;
	m_TGC_stationPhi = new std::vector<int>;
	m_TGC_isEndcap = new std::vector<bool>;
	m_TGC_Sim_gasGap = new std::vector<int>;
	m_TGC_gasGap = new std::vector<int>;
	m_TGC_isStrip = new std::vector<int>;
	m_TGC_measuresPhi = new std::vector<int>;
	m_TGC_channel = new std::vector<int>;
	m_TGC_globalTime = new std::vector<double>;
	m_TGC_hitLocalPositionX = new std::vector<double>;
	m_TGC_hitLocalPositionY = new std::vector<double>;
	m_TGC_hitLocalPositionZ = new std::vector<double>;
	m_TGC_hitGlobalPositionX = new std::vector<double>;
	m_TGC_hitGlobalPositionY = new std::vector<double>;
	m_TGC_hitGlobalPositionZ = new std::vector<double>;
	m_TGC_hitGlobalPositionR = new std::vector<double>;
	m_TGC_hitGlobalPositionP = new std::vector<double>;
	m_TGC_detector_globalPositionX = new std::vector<double>;
	m_TGC_detector_globalPositionY = new std::vector<double>;
	m_TGC_detector_globalPositionZ = new std::vector<double>;
	m_TGC_detector_globalPositionR = new std::vector<double>;
	m_TGC_detector_globalPositionP = new std::vector<double>;
	m_TGC_particleEncoding = new std::vector<int>;
	m_TGC_kineticEnergy = new std::vector<double>;
	m_TGC_depositEnergy = new std::vector<double>;
	m_TGC_StepLength = new std::vector<double>;
	m_TGC_trackId = new std::vector<int>;
	m_TGC_truthEl = new std::vector<int>;
	m_TGC_nSimHits = 0;

	if (m_tree) {
		m_tree->Branch("TGC_Sim_stationName", &m_TGC_Sim_stationName);
		m_tree->Branch("TGC_stationName", &m_TGC_stationName);
		m_tree->Branch("TGC_stationEta", &m_TGC_stationEta);
		m_tree->Branch("TGC_stationPhi", &m_TGC_stationPhi);
		m_tree->Branch("TGC_isEndcap", &m_TGC_isEndcap);

		m_tree->Branch("TGC_Sim_gasGap", &m_TGC_Sim_gasGap);
		m_tree->Branch("TGC_gasGap", &m_TGC_gasGap);
		m_tree->Branch("TGC_isStrip", &m_TGC_isStrip);
		m_tree->Branch("TGC_measuresPhi", &m_TGC_measuresPhi);
		m_tree->Branch("TGC_channel", &m_TGC_channel);

		m_tree->Branch("TGC_globalTime", &m_TGC_globalTime);
		m_tree->Branch("TGC_hitLocalPositionX", &m_TGC_hitLocalPositionX);
		m_tree->Branch("TGC_hitLocalPositionY", &m_TGC_hitLocalPositionY);
		m_tree->Branch("TGC_hitLocalPositionZ", &m_TGC_hitLocalPositionZ);
		m_tree->Branch("TGC_hitGlobalPositionX", &m_TGC_hitGlobalPositionX);
		m_tree->Branch("TGC_hitGlobalPositionY", &m_TGC_hitGlobalPositionY);
		m_tree->Branch("TGC_hitGlobalPositionZ", &m_TGC_hitGlobalPositionZ);
		m_tree->Branch("TGC_hitGlobalPositionR", &m_TGC_hitGlobalPositionR);
		m_tree->Branch("TGC_hitGlobalPositionP", &m_TGC_hitGlobalPositionP);
		m_tree->Branch("TGC_detector_globalPositionX", &m_TGC_detector_globalPositionX);
		m_tree->Branch("TGC_detector_globalPositionY", &m_TGC_detector_globalPositionY);
		m_tree->Branch("TGC_detector_globalPositionZ", &m_TGC_detector_globalPositionZ);
		m_tree->Branch("TGC_detector_globalPositionR", &m_TGC_detector_globalPositionR);
		m_tree->Branch("TGC_detector_globalPositionP", &m_TGC_detector_globalPositionP);

		m_tree->Branch("TGC_particleEncoding", &m_TGC_particleEncoding);
		m_tree->Branch("TGC_kineticEnergy", &m_TGC_kineticEnergy);
		m_tree->Branch("TGC_depositEnergy", &m_TGC_depositEnergy);
		m_tree->Branch("TGC_StepLength", &m_TGC_StepLength);
		m_tree->Branch("TGC_trackId", &m_TGC_trackId);
		m_tree->Branch("TGC_truthEl", &m_TGC_truthEl);
		m_tree->Branch("TGC_nSimHits", &m_TGC_nSimHits);
	}

	return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called in the destructor */
void TGCSimHitVariables::deleteVariables()
{
	delete m_TGC_Sim_stationName;
	delete m_TGC_stationName;
	delete m_TGC_stationEta;
	delete m_TGC_stationPhi;
	delete m_TGC_isEndcap;
	delete m_TGC_Sim_gasGap;
	delete m_TGC_gasGap;
	delete m_TGC_isStrip;
	delete m_TGC_measuresPhi;
	delete m_TGC_channel;
	delete m_TGC_globalTime;
	delete m_TGC_hitLocalPositionX;
	delete m_TGC_hitLocalPositionY;
	delete m_TGC_hitLocalPositionZ;
	delete m_TGC_hitGlobalPositionX;
	delete m_TGC_hitGlobalPositionY;
	delete m_TGC_hitGlobalPositionZ;
	delete m_TGC_hitGlobalPositionR;
	delete m_TGC_hitGlobalPositionP;
	delete m_TGC_detector_globalPositionX;
	delete m_TGC_detector_globalPositionY;
	delete m_TGC_detector_globalPositionZ;
	delete m_TGC_detector_globalPositionR;
	delete m_TGC_detector_globalPositionP;
	delete m_TGC_particleEncoding;
	delete m_TGC_kineticEnergy;
	delete m_TGC_depositEnergy;
	delete m_TGC_StepLength;
	delete m_TGC_trackId;
	delete m_TGC_truthEl;
	m_TGC_nSimHits = 0;

	m_TGC_Sim_stationName = nullptr;
	m_TGC_stationName = nullptr;
	m_TGC_stationEta = nullptr;
	m_TGC_stationPhi = nullptr;
	m_TGC_isEndcap = nullptr;
	m_TGC_Sim_gasGap = nullptr;
	m_TGC_gasGap = nullptr;
	m_TGC_isStrip = nullptr;
	m_TGC_measuresPhi = nullptr;
	m_TGC_channel = nullptr;
	m_TGC_globalTime = nullptr;
	m_TGC_hitLocalPositionX = nullptr;
	m_TGC_hitLocalPositionY = nullptr;
	m_TGC_hitLocalPositionZ = nullptr;
	m_TGC_hitGlobalPositionX = nullptr;
	m_TGC_hitGlobalPositionY = nullptr;
	m_TGC_hitGlobalPositionZ = nullptr;
	m_TGC_hitGlobalPositionR = nullptr;
	m_TGC_hitGlobalPositionP = nullptr;
	m_TGC_detector_globalPositionX = nullptr;
	m_TGC_detector_globalPositionY = nullptr;
	m_TGC_detector_globalPositionZ = nullptr;
	m_TGC_detector_globalPositionR = nullptr;
	m_TGC_detector_globalPositionP = nullptr;
	m_TGC_particleEncoding = nullptr;
	m_TGC_kineticEnergy = nullptr;
	m_TGC_depositEnergy = nullptr;
	m_TGC_StepLength = nullptr;
	m_TGC_trackId = nullptr;
	m_TGC_truthEl = nullptr;

	return;
}
