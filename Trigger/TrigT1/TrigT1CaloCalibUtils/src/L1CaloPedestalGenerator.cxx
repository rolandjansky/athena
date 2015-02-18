/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloPedestalGenerator.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"

#include <vector>
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h" 
#include "CLHEP/Random/Randomize.h"

L1CaloPedestalGenerator::L1CaloPedestalGenerator(const std::string& name, ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator), m_detStore(0), m_storeGate(0), m_caloMgr(0), m_lvl1Helper(0), m_towerKey(0), m_IntTTContainer(0)
{
    declareProperty("TriggerTowerLocation", m_TTLocation  = "LVL1TriggerTowers");
    declareProperty("PedestalMean", m_pedMean  = 10.);
    declareProperty("PedestalRMS",  m_pedRMS   =  0.);
    declareProperty("NSamples",     m_nSamples =   5);
}

L1CaloPedestalGenerator::~L1CaloPedestalGenerator()
{
}

//-------------------------------------------
// Initialize
//-------------------------------------------
StatusCode L1CaloPedestalGenerator::initialize()
{
    ATH_MSG_INFO("From Initialize...");
    StatusCode sc;

    //get a pointer to DetectorStore services
    sc = service("DetectorStore", m_detStore);
    if (sc.isFailure()) {
        ATH_MSG_ERROR( "Cannot access DetectorStore" );
        return StatusCode::FAILURE;
    }

    //get a pointer to Event StoreGate services
    sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
        ATH_MSG_ERROR( "Cannot access StoreGate" );
        return StatusCode::FAILURE;
    }


  // Retrieve CaloIdManager
    sc = m_detStore->retrieve(m_caloMgr);
    if (sc.isFailure()) {
        ATH_MSG_ERROR( "Unable to retrieve CaloIdManager from DetectorStore" );
        return StatusCode::FAILURE;
    }

    m_lvl1Helper = m_caloMgr->getLVL1_ID();
    if (!m_lvl1Helper) {
        ATH_MSG_ERROR( "Could not access CaloLVL1_ID helper" );
        return StatusCode::FAILURE;
    }

	m_towerKey = new LVL1::TriggerTowerKey();

    return StatusCode::SUCCESS;
}

//----------------------------------------
// Execute
//----------------------------------------
StatusCode L1CaloPedestalGenerator::execute()
{
    ATH_MSG_DEBUG("From Execute...");

    StatusCode sc;

    m_IntTTContainer = new std::map<unsigned int, LVL1::InternalTriggerTower*>;

    TriggerTowerCollection* VectorOfTTs = new TriggerTowerCollection;
    sc = m_storeGate->record(VectorOfTTs, m_TTLocation);

    if (sc.isSuccess()) ATH_MSG_DEBUG( "Stored TTs in TES at "<< m_TTLocation );
    else {
    ATH_MSG_ERROR( "failed to write TTs to TES at " << m_TTLocation );
    return StatusCode::FAILURE;
    }

    //---
	// Loop on all Trigger Towers to create the TT objects
	// and store them into containers

	std::vector<Identifier>::const_iterator tower_it = m_lvl1Helper->tower_begin();

	// -- Loop on Trigger Tower offline Ids --
	for(;tower_it!=m_lvl1Helper->tower_end();++tower_it) {
		Identifier towerId = (*tower_it);

		//---
		// Check detector type
		bool bIsTile = 	m_lvl1Helper->is_tile(towerId);
		bool bIsHadronic = m_lvl1Helper->sampling(towerId); // check if tt is an HEC tt

		//---
		// Generate pedestal samples
		std::vector<double> vnoise;
		for(unsigned int sample=0;sample<m_nSamples;++sample) vnoise.push_back(CLHEP::RandGaussZiggurat::shoot(0.,m_pedRMS));

		std::vector< std::vector<double> > vAutoCorrMatrix(m_nSamples, std::vector<double>(m_nSamples,0));
		for(unsigned int sample=0;sample<m_nSamples;++sample) vAutoCorrMatrix[sample][sample] = 1;

		std::vector<double> vsamples;
		for(unsigned int sample=0;sample<m_nSamples;++sample) {
			double noise = 0;
			for(unsigned int jj=0;jj<m_nSamples;++jj) noise+= vAutoCorrMatrix[sample][jj]*vnoise[jj];
			vsamples.push_back(m_pedMean + noise);
		}

		//---
		// Seek for an existing or create a new InternalTriggerTower
		LVL1::InternalTriggerTower* TT = findInternalTriggerTower(towerId);

		if(bIsTile||bIsHadronic) TT->addHadAmps(vsamples);
		else TT->addEMAmps(vsamples);

	} //-- End Loop on Trigger Tower offline Ids --


	//---
	// Loop over Internal Trigger Tower and create fake TT with
	// only the Digits parts filled

	std::map<unsigned int, LVL1::InternalTriggerTower*>::const_iterator it = m_IntTTContainer->begin();
	for(;it!=m_IntTTContainer->end();++it) {
		double eta = it->second->eta();
		double phi = it->second->phi();
		unsigned int key = it->second->key();

		std::vector<double> vEmAmps = it->second->EmAmps();
		std::vector<int> emDigits(vEmAmps.size());
                for (size_t i = 0; i < vEmAmps.size(); i++)
                  emDigits[i] = static_cast<int> (vEmAmps[i]);

		std::vector<double> vHadAmps = it->second->HadAmps();
		std::vector<int> hadDigits(vHadAmps.size());
                for (size_t i = 0; i < vHadAmps.size(); i++)
                  hadDigits[i] = static_cast<int> (vHadAmps[i]);

		//need to put some fake push back to be able to use TriggerTowers2Ntuple
		// cause LVL1::TriggerTower::emEnergy() make a m_em_energy[m_em_peak];
		// instead of using m_em_energy.at(m_em_peak);
		std::vector<int> emEt; emEt.push_back(0);
		std::vector<int> hadEt; hadEt.push_back(0);
		std::vector<int> emBCID; emBCID.push_back(0);
		std::vector<int> hadBCID; hadBCID.push_back(0);
		std::vector<int> emExt; emExt.push_back(0);
		std::vector<int> hadExt; hadExt.push_back(0);
		int peak = 0;
		int emADCPeak = 0;
		int hadADCPeak = 0;

		// Create TT
		LVL1::TriggerTower* tower = new LVL1::TriggerTower(phi,eta,key,
			emDigits, emEt, emExt, emBCID, 0, peak, emADCPeak,
			hadDigits, hadEt, hadExt, hadBCID, 0, peak, hadADCPeak);

		VectorOfTTs->push_back(tower);

	} //-- End Loop on InternalTriggerTowers --

	ATH_MSG_DEBUG( VectorOfTTs->size()<<" TTs have been generated");

    sc = m_storeGate->setConst(VectorOfTTs);
    if (sc.isSuccess()) ATH_MSG_DEBUG( "TT container locked");
    else {
        ATH_MSG_ERROR( "failed to lock TT container");
        return StatusCode::FAILURE;
    }

    it = m_IntTTContainer->begin();
    for(;it!=m_IntTTContainer->end();++it) delete it->second;
    delete m_IntTTContainer;

    ATH_MSG_DEBUG("End of Execute...");
    return StatusCode::SUCCESS;
}

//-----------------------------------------
// Finalize
//-----------------------------------------
StatusCode L1CaloPedestalGenerator::finalize()
{
    delete m_towerKey;

    return StatusCode::SUCCESS;
}

LVL1::InternalTriggerTower* L1CaloPedestalGenerator::findInternalTriggerTower(const Identifier& towerId, bool bCreate) {
    // Create tower key from towerId
    double tower_eta = IDeta(towerId,m_lvl1Helper);
    double tower_phi = IDphi(towerId,m_lvl1Helper);
    unsigned int key = m_towerKey->ttKey(tower_phi,tower_eta);
    double tt_phi = m_towerKey->phi();
    double tt_eta = m_towerKey->eta();

    return findInternalTriggerTower(tt_phi, tt_eta, key, bCreate);
}

LVL1::InternalTriggerTower* L1CaloPedestalGenerator::findInternalTriggerTower(double tt_phi, double tt_eta, unsigned int key, bool bCreate) {
    std::map<unsigned int, LVL1::InternalTriggerTower*>::iterator it = m_IntTTContainer->find( key );
    LVL1::InternalTriggerTower* TT = 0;
    if (it == m_IntTTContainer->end()){
        if(bCreate) {
            // no TT yet. Create it!
            TT = new LVL1::InternalTriggerTower(tt_phi,tt_eta, key);
            m_IntTTContainer->insert(std::map<unsigned int, LVL1::InternalTriggerTower*>::value_type(key,TT)); //and put it in the map.
        }
    }else{
        TT = (it->second);
    } // end else
    return TT;
}

double L1CaloPedestalGenerator::IDeta(const Identifier& id, const CaloLVL1_ID* l1id) {
    int region = l1id->region(id);
    int ieta = l1id->eta(id);
    int sign = l1id->pos_neg_z(id);

    double gran[4] = {0.1, 0.2, 0.1, 0.425};
    double offset[4] = {0., 2.5, 3.1, 3.2};
    double eta;

    if (region>=0 && region<=3) {
        eta = sign* ( ( (ieta+0.5) * gran[region] ) + offset[region] );
    }
    else {
        eta = 0.;
    }
    return eta;
}

double L1CaloPedestalGenerator::IDphi(const Identifier& id, const CaloLVL1_ID* l1id) {
    Identifier regId = l1id->region_id(id);

    double phiMax = l1id->phi_max(regId);
    int iphi = l1id->phi(id);

    double phi = (iphi+0.5)*2*M_PI/(phiMax+1);

    return phi;
}

