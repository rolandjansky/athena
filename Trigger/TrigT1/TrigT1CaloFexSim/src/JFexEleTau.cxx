// TrigT1CaloFexSim includes
#include "TrigT1CaloFexSim/JFexEleTau.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "TrigT1CaloFexSim/JetAlg.h"
//#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EmTauRoIAuxContainer.h"
#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/JetRoIContainer.h"



JFexEleTau::JFexEleTau( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){


  declareProperty( "RegenerateSeeds", m_regenerateSeeds = false, "Generate new seeds or use roundJet seeds: note, 'saveSeeds' must be set for the latter" );
  declareProperty( "ApplyNoise", m_applyNoise = false, "Apply tower noise cut when clustering tau candidate" );
  declareProperty( "CheckMax", m_checkMax = false, "Require seed to be a local max" );
  declareProperty( "NoiseStrategy", m_noiseStrategy = 0, "Noise cuts to use (both seeding and clustering, latter only when applyNoise is true): 0 is none, 1 is default, 2 is lowered");
  declareProperty( "UseRun2", m_useRun2 = false, "Run-II style clustering or Run-III" );
  declareProperty( "SingleTowerSeed", m_singleTowerSeed = false, "Use 1x1 seed" );
}


JFexEleTau::~JFexEleTau() {}


StatusCode JFexEleTau::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode JFexEleTau::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode JFexEleTau::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed

  // Prepare output containers (with all xAOD annoying features)
  xAOD::EmTauRoIContainer* clustersForTau = new xAOD::EmTauRoIContainer();
  xAOD::EmTauRoIAuxContainer* auxclustersForTau = new xAOD::EmTauRoIAuxContainer();
  clustersForTau->setStore(auxclustersForTau);
  clustersForTau->reserve(100);
  std::string clusterName("jTaus");
  ATH_CHECK( evtStore()->record(clustersForTau,clusterName) ); 
  ATH_CHECK( evtStore()->record(auxclustersForTau,clusterName+"Aux.") );

  // Prepare output containers (with all xAOD annoying features)
  xAOD::EmTauRoIContainer* clustersForEle = new xAOD::EmTauRoIContainer();
  xAOD::EmTauRoIAuxContainer* auxclustersForEle = new xAOD::EmTauRoIAuxContainer();
  clustersForEle->setStore(auxclustersForEle);
  clustersForEle->reserve(100);
  std::string clusterNameE("jEles");
  ATH_CHECK(evtStore()->record(clustersForEle,clusterNameE) ); 
  ATH_CHECK ( evtStore()->record(auxclustersForEle,clusterNameE+"Aux.") ); 
  
  // Retrieve jTower container
  const xAOD::JGTowerContainer* jTowers =0;
  CHECK( evtStore()->retrieve( jTowers,"JTowers"));

  // Create noise profile
  std::vector<float> jT_noise;
  
  // Control noise cuts
  for( const auto &jT : *jTowers){
    
    if(m_noiseStrategy == 0)
      jT_noise.push_back(0);
    else if(m_noiseStrategy == 1)
      {
	if(jT->sampling()==0) jT_noise.push_back(430);
	else if(jT->sampling()==1) jT_noise.push_back(2400);
	else jT_noise.push_back(2000);
      }
    else if(m_noiseStrategy == 2)
      {  
	if(jT->sampling()==0) jT_noise.push_back(125);
	else if(jT->sampling()==1) jT_noise.push_back(250);
	else jT_noise.push_back(250);
      }
  }

  if(m_regenerateSeeds)
    {
      bool dumpPlots = false;

      // We need to regenerate the seed map through JetAlg
      if( JetAlg::m_SeedMap.find("jTauSeeds") == JetAlg::m_SeedMap.end() )
	CHECK( JetAlg::SeedGrid(jTowers, "jTauSeeds", dumpPlots) );
      
      //ATH_MSG_DEBUG("JFexAlg: SeedFinding with jJetSeeds; m_jJet_seed_size = " << m_jJet_seed_size << ", m_jJet_max_r = " << m_jJet_max_r);
      if(m_singleTowerSeed)
	CHECK( JetAlg::SeedFinding( jTowers, "jTauSeeds", 0.11, 0.26, jT_noise, 
				    2.0, 
				    0.0,/*electron*/false) );
      else
	CHECK( JetAlg::SeedFinding( jTowers, "jTauSeeds", 0.31, 0.26, jT_noise, 
				    2.0, 
				    0.0,/*electron*/false) );
      
      // Check if we can actually access the 2x2 seeds, since we are technically running after the RoundJets
      ATH_MSG_DEBUG("Looking for seeds: jFEX Taus");
      
      std::shared_ptr<JetAlg::Seed> seeds = JetAlg::m_SeedMap["jTauSeeds"];
      
      ATH_MSG_DEBUG("Looping on seeds: jFEX Taus");
      
      // Loop over all seeds
      for(unsigned eta_ind=0; eta_ind<seeds->eta.size(); eta_ind++){
	for(unsigned phi_ind=0; phi_ind<seeds->phi.at(eta_ind).size(); phi_ind++){
	  
	  // Extract seed information
	  float eta = seeds->eta.at(eta_ind);
	  float phi = seeds->phi.at(eta_ind).at(phi_ind);
	  
	  // Ignore non-local maxima
	  if(m_checkMax)
	    if(!seeds->local_max.at(eta_ind).at(phi_ind)) continue;
	  
	  ATH_MSG_DEBUG("jFEX Taus: got a seed at =" << eta << " " << phi);

	  // Use this seed to generate a tau candidate
	  if(m_useRun2)
	    CHECK(analyzeSeedRun2(eta, phi, jTowers, clustersForTau));
	  else
	    CHECK(analyzeSeedRun3(eta, phi, jTowers, clustersForTau));
	  
	}
      }
    } else {
    
    // Retrieve seeds from StoreGate
    const xAOD::JetRoIContainer* jRoundSeeds = 0;
    CHECK( evtStore()->retrieve( jRoundSeeds,"jRoundSeeds_localMax"));

    for(unsigned int i=0; i<jRoundSeeds->size(); i++)
      {
	const xAOD::JetRoI* thisSeed = jRoundSeeds->at(i);
	
	// Re-do entire logic
	float eta = thisSeed->eta();
	float phi = thisSeed->phi();

	if(m_useRun2)
	  CHECK(analyzeSeedRun2(eta, phi, jTowers, clustersForTau));
	else
	  CHECK(analyzeSeedRun3(eta, phi, jTowers, clustersForTau));

      }

    // Run electrons
    // We need to regenerate the seed map through JetAlg

  }
  
  bool dumpPlots = false;
  if( JetAlg::m_SeedMap.find("jEleSeeds") == JetAlg::m_SeedMap.end() )
    CHECK( JetAlg::SeedGrid(jTowers, "jEleSeeds", dumpPlots) );
  
  // Some notes: set the seed_electrons to true: this will seed on EM only
  // and will stretch out the 0.21 range window by +0.1 in eta > 2.5
  CHECK( JetAlg::SeedFinding( jTowers, "jEleSeeds", 0.11, 0.21, jT_noise,
			      2.0, 
			      0.0, true) );
  
  
  std::shared_ptr<JetAlg::Seed> seeds = JetAlg::m_SeedMap["jEleSeeds"];
  
  
  for(unsigned eta_ind=0; eta_ind<seeds->eta.size(); eta_ind++){
    for(unsigned phi_ind=0; phi_ind<seeds->phi.at(eta_ind).size(); phi_ind++){
      
      // Extract seed information
      float eta = seeds->eta.at(eta_ind);
      float phi = seeds->phi.at(eta_ind).at(phi_ind);
      
      
      // Ignore non-local maxima
      if(!seeds->local_max.at(eta_ind).at(phi_ind)) continue;
      
      ATH_MSG_DEBUG("jFEX Eles: got a seed at =" << eta << " " << phi);
      
      
      // Use this seed to generate an electron candidate
      CHECK(analyzeSeedEle(eta, phi, jTowers, clustersForEle));
      
    }
  }
  
  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode JFexEleTau::analyzeSeedRun2(float eta, float phi, const xAOD::JGTowerContainer* jTowers, xAOD::EmTauRoIContainer* clustersForTau) {
  ATH_MSG_DEBUG ("analyzing a given seed " << name() << "...");

  // Ignore seeds outside of tracker coverage
  if(std::abs(eta) > 2.47) return StatusCode::SUCCESS;;

  // Find the 4 EM, 4 Tile towers and 16 isolate EM towers
  std::vector<const xAOD::JGTower*> EMTowers;
  std::vector<const xAOD::JGTower*> TileTowers;
  std::vector<const xAOD::JGTower*> EMIsolation;

  // Loop over constituent towers
  for(unsigned t=0; t<jTowers->size(); t++){

    const xAOD::JGTower* tower = jTowers->at(t);

    //unused for now
    //bool isTile = (fabs(tower->eta()) <1.5 && tower->sampling()==1);
    //bool applyNoise = true;
    //if(isTile) applyNoise=false;

    float dEta = std::abs(eta - tower->eta());
    float dPhi = std::abs(TVector2::Phi_mpi_pi(phi - tower->phi()));

    if(dEta < 0.095 && dPhi < 0.095)
      {
	// Tower found
	if(tower->sampling()==1)
	  TileTowers.push_back(tower);
	else
	  EMTowers.push_back(tower);
      }

    if(dEta < 0.195 && dPhi < 0.195)
      {
	if(tower->sampling()!=1)
	  EMIsolation.push_back(tower);
      }
  }

  // Produce tau energy sums
  float tauEnergy = 0;
  float tileEnergy = 0;
  float emEnergy = 0;
  float fullemEnergy = 0;
  float isoEnergy = 0;

  // Sum of tile energy: 2x2 sum
  // Also full 2x2 EM energy: purely for isolation calculation
  for(unsigned int i=0; i<TileTowers.size(); i++)
    {
      tileEnergy += TileTowers[i]->et();
      fullemEnergy += EMTowers[i]->et();
    }

  for(unsigned int i=0; i<EMIsolation.size(); i++)
    isoEnergy += EMIsolation[i]->et();

  // Test a potential problematic case (probably solved by mpi-pi)
  if(TileTowers.size() == 2 || EMTowers.size() == 2)
    {
      ATH_MSG_INFO("Missing towers, eta/phi" << eta << "/" << phi);
      emEnergy = EMTowers[0]->et() + EMTowers[1]->et();
    }
  else if(EMTowers.size() ==4)
    {
      // Complicated to sort towers, so try all 2 tower sums in EM
      // And veto the ones where the towers are not aligned in either eta or phi
      float biggestSum = 0;
      for(int i=0; i<4; i++)
	{
	  for(int j=0; j<4; j++)
	    {
	      if(i==j) continue;
	      bool sameLineEta = std::abs(EMTowers[i]->eta() - EMTowers[j]->eta()) < 0.02;
	      bool sameLinePhi = std::abs(TVector2::Phi_mpi_pi(EMTowers[i]->phi() - EMTowers[j]->phi())) < 0.02;

	      float currentSum = EMTowers[i]->et() + EMTowers[j]->et();
	      if(currentSum > biggestSum)
		{
		  if(sameLineEta || sameLinePhi)
		    biggestSum = currentSum;
		}
	    }
	}
      emEnergy = biggestSum;
    }
  else{
    ATH_MSG_INFO("Wrong number of EMTowers: This is very problematic");
  }

  // Full tau candidate ET: 1x2 in EM + 2x2 in Tile
  tauEnergy = emEnergy+tileEnergy;
  // Tau isolation: 4x4 ring, ignore the 2x2 inner part in EM
  isoEnergy -= fullemEnergy;

  xAOD::EmTauRoI* clForTau = new xAOD::EmTauRoI();
  clustersForTau->push_back( clForTau );
  clForTau->setEta( eta );
  clForTau->setPhi( phi );
  clForTau->setTauClus( tauEnergy );
  clForTau->setEmIsol( isoEnergy );

  return StatusCode::SUCCESS;
}

StatusCode JFexEleTau::analyzeSeedRun3(float eta, float phi, const xAOD::JGTowerContainer* jTowers, xAOD::EmTauRoIContainer* clustersForTau) {
  ATH_MSG_DEBUG ("analyzing a given seed " << name() << "...");

  // Ignore seeds outside of tracker coverage
  if(std::abs(eta) > 2.47) return StatusCode::SUCCESS;;

  // Find the 4 EM, 4 Tile towers and 16 isolate EM towers
  std::vector<const xAOD::JGTower*> EMTowers;
  std::vector<const xAOD::JGTower*> TileTowers;
  std::vector<const xAOD::JGTower*> EMIsolation;

  // Loop over constituent towers
  for(unsigned t=0; t<jTowers->size(); t++){

    const xAOD::JGTower* tower = jTowers->at(t);

    //unused for now
    //bool isTile = (fabs(tower->eta()) <1.5 && tower->sampling()==1);
    //bool applyNoise = true;
    //if(isTile) applyNoise=false;

    float dEta = std::abs(eta - tower->eta());
    float dPhi = std::abs(TVector2::Phi_mpi_pi(phi - tower->phi()));

    if(sqrt(dEta*dEta + dPhi*dPhi) < 0.15)
      {
	// Tower found
	if(tower->sampling()==1)
	  TileTowers.push_back(tower);
	else
	  EMTowers.push_back(tower);
      }

    if(sqrt(dEta*dEta + dPhi*dPhi) < 0.35)
      {
	if(tower->sampling()!=1)
	  EMIsolation.push_back(tower);
      }
  }

  // Produce tau energy sums
  float tauEnergy = 0;
  float tileEnergy = 0;
  float fullemEnergy = 0;
  float isoEnergy = 0;

  // Sum of tile energy: 3x3 sum
  // Also full 3x3 EM energy
  for(unsigned int i=0; i<TileTowers.size(); i++)
    {
      tileEnergy += TileTowers[i]->et();
      fullemEnergy += EMTowers[i]->et();
    }

  for(unsigned int i=0; i<EMIsolation.size(); i++)
    isoEnergy += EMIsolation[i]->et();
  
  
  // Full tau candidate ET: 1x2 in EM + 2x2 in Tile
  tauEnergy = fullemEnergy+tileEnergy;

  // Tau isolation: 4x4 ring, ignore the 2x2 inner part in EM
  isoEnergy -= fullemEnergy;
  
  xAOD::EmTauRoI* clForTau = new xAOD::EmTauRoI();
  clustersForTau->push_back( clForTau );
  clForTau->setEta( eta );
  clForTau->setPhi( phi );
  clForTau->setTauClus( tauEnergy );
  clForTau->setEmIsol( isoEnergy );

  return StatusCode::SUCCESS;
}


StatusCode JFexEleTau::analyzeSeedEle(float eta, float phi, const xAOD::JGTowerContainer* jTowers, xAOD::EmTauRoIContainer* clustersForEle) {
  ATH_MSG_DEBUG ("analyzing a given seed " << name() << "...");

  // Ignore seeds outside of tracker coverage
  if(std::abs(eta) < 2.3) return StatusCode::SUCCESS;;

  // Central tower
  const xAOD::JGTower* EMTower = 0;
  const xAOD::JGTower* HADTower = 0;

  // Isolation Ring
  std::vector<const xAOD::JGTower*> EMIsolation;

  // Loop over constituent towers
  float nearestdREM= 5.0;
  float nearestdRHAD = 5.0;


  // Look for main tower
  for(unsigned t=0; t<jTowers->size(); t++){

    const xAOD::JGTower* tower = jTowers->at(t);

    float dEta = std::abs(eta - tower->eta());
    float dPhi = std::abs(TVector2::Phi_mpi_pi(phi - tower->phi()));

    if(tower->sampling()==1)
      {
	if(sqrt(dEta*dEta + dPhi*dPhi) < nearestdRHAD)
	  {
	    HADTower = tower;
	    nearestdRHAD = sqrt(dEta*dEta + dPhi*dPhi);
	  }
      } else
      {
	if(sqrt(dEta*dEta + dPhi*dPhi) < nearestdREM)
	  {
	    EMTower = tower;
	    nearestdREM = sqrt(dEta*dEta + dPhi*dPhi);
	  }
      }
    

  }
  
  // Fill isolation
  for(unsigned t=0; t<jTowers->size(); t++){
    const xAOD::JGTower* tower = jTowers->at(t);

    float dEta = std::abs(eta - tower->eta());
    float dPhi = std::abs(TVector2::Phi_mpi_pi(phi - tower->phi()));

    // Ignore central tower
    if(tower == EMTower)
      continue;

    if(sqrt(dEta*dEta + dPhi*dPhi) < 0.4)
      {
	if(tower->sampling()!=1)
	  EMIsolation.push_back(tower);
      }

  }

  // Nearest neighbor search
  float closestdR = 5.0;

  // First, find closest tower in dR
  for(unsigned int i=0; i< EMIsolation.size(); i++)
    {
      const xAOD::JGTower* tower = EMIsolation.at(i);
      
      float dEta = std::abs(eta - tower->eta());
      float dPhi = std::abs(TVector2::Phi_mpi_pi(phi - tower->phi()));
      
      if (sqrt(dEta*dEta + dPhi*dPhi) < closestdR)
	closestdR = sqrt(dEta*dEta + dPhi*dPhi);
    }

  // Then, look for largest pT tower that is closer than 1.9 * nearest dR
  // Which basically implies, approximately, don't go "two towers away" from the center
  float highestPt = 0;
  float fullIso = 0;
  for(unsigned int i=0; i< EMIsolation.size(); i++)
    {
      const xAOD::JGTower* tower = EMIsolation.at(i);
      
      fullIso += tower->et();

      float dEta = std::abs(eta - tower->eta());
      float dPhi = std::abs(TVector2::Phi_mpi_pi(phi - tower->phi()));
      
      if (sqrt(dEta*dEta + dPhi*dPhi) < (closestdR*1.9) && tower->et() > highestPt)
	highestPt = tower->et();
    }
  

  // Produce electron energy sums
  float eleEnergy = EMTower->et() + highestPt;
  float hadEnergy = HADTower->et();
  float isoEnergy = fullIso - highestPt;
  
  xAOD::EmTauRoI* clForEle = new xAOD::EmTauRoI();
  clustersForEle->push_back( clForEle );
  clForEle->setEta( eta );
  clForEle->setPhi( phi );
  clForEle->setEmClus( eleEnergy );
  clForEle->setTauClus( hadEnergy );
  clForEle->setEmIsol( isoEnergy );

  return StatusCode::SUCCESS;
}



