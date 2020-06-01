/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TriggerMatchingAugmentation.cxx
///////////////////////////////////////////////////////////////////
// Author: Jacob Searcy (jsearcy@umich.edu)
//

// local includes
#include "DerivationFrameworkCore/TriggerMatchingAugmentation.h"

// EDM includes
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include <vector>
#include <string>

namespace DerivationFramework {
  using std::pair;
  using std::string;

  TriggerMatchingAugmentation::TriggerMatchingAugmentation(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p),
    m_matchTool("Trig::MatchingTool"),
    m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
    m_decorPrefix("DFCommonTrigMatch_"),
    m_muonContainerName("Muons"),
    m_electronContainerName("Electrons"),
    m_photonContainerName("Photons")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("MatchingTool", m_matchTool);
    declareProperty("DecorationPrefix", m_decorPrefix);
    declareProperty("PhotonContainerName", m_photonContainerName);
    declareProperty("ElectronContainerName", m_electronContainerName);
    declareProperty("MuonContainerName", m_muonContainerName);
    declareProperty("SingleTriggerList", m_singleTriggerList);
    declareProperty("DiMuonList", m_2mTriggerList);
    declareProperty("DiElectronList", m_2eTriggerList);
    declareProperty("ElectronMuonList", m_emTriggerList);
    declareProperty( "TrigDecisionTool", m_trigDec);
  }

  StatusCode TriggerMatchingAugmentation::initialize()
  {
    CHECK( m_trigDec.retrieve() );
    CHECK( m_matchTool.retrieve() );
    
    if (m_photonContainerName=="") {
      ATH_MSG_ERROR("No Photons collection provided for TriggerMatchingAugmentation!");
      return StatusCode::FAILURE;
    }
    if (m_electronContainerName=="") {
      ATH_MSG_ERROR("No Electrons collection provided for TriggerMatchingAugmentation!");
      return StatusCode::FAILURE;
    }
    if (m_muonContainerName=="") {
      ATH_MSG_ERROR("No Muon collection provided for TriggerMatchingAugmentation!");
      return StatusCode::FAILURE;
    }

    // Generate vectors of decorators to use in the event loop,
    // to avoid needless overhead from searching the AuxStoreRegistry
    for(const std::string& trigname: m_singleTriggerList) {
      // Split up e/gamma/mu to avoid string comparisons in the event loop
      decor_t decor(m_decorPrefix+trigname);
      pair<const string, decor_t> decorpair(trigname,decor);
      if (trigname.find("HLT_e")!=std::string::npos) {
	m_1eDecorList.push_back(decorpair);
      } else if (trigname.find("HLT_m")!=std::string::npos) {
	m_1mDecorList.push_back(decorpair);
      } else {
	m_1gDecorList.push_back(decorpair);
      }
      // Add the trigger name/decorator pair to the selected list
    }

    // Dilepton triggers are split by the user, so simpler to set up
    for(const std::string& trigname: m_2mTriggerList) {
      decor_t decor(m_decorPrefix+trigname);
      pair<const string, decor_t> decorpair(trigname,decor);
      m_2mDecorList.push_back(decorpair);
    }
    for(const std::string& trigname: m_2eTriggerList) {
      decor_t decor(m_decorPrefix+trigname);
      pair<const string, decor_t> decorpair(trigname,decor);
      m_2eDecorList.push_back(decorpair);
    }
    for(const std::string& trigname: m_emTriggerList) {
      decor_t decor(m_decorPrefix+trigname);
      pair<const string, decor_t> decorpair(trigname,decor);
      m_emDecorList.push_back(decorpair);
    }


    return StatusCode::SUCCESS;
  }

  StatusCode TriggerMatchingAugmentation::finalize()
  {
    return StatusCode::SUCCESS;
  }
 
  StatusCode TriggerMatchingAugmentation::addBranches() const
  {

    // Retrieve the containers to decorate
    const xAOD::PhotonContainer* photons = 0;
    if (evtStore()->retrieve(photons, m_photonContainerName).isFailure()) {
         ATH_MSG_WARNING("Couldn't retrieve " << m_photonContainerName << " from TEvent");
         return StatusCode::FAILURE;
    }
    const xAOD::MuonContainer* muons = 0;
    if (evtStore()->retrieve(muons, m_muonContainerName).isFailure()) {
         ATH_MSG_WARNING("Couldn't retrieve " << m_muonContainerName << " from TEvent");
         return StatusCode::FAILURE;
    }
    const xAOD::ElectronContainer* electrons = 0;
    if (evtStore()->retrieve(electrons, m_electronContainerName).isFailure()) {
         ATH_MSG_WARNING("Couldn't retrieve " << m_electronContainerName << " from TEvent");
         return StatusCode::FAILURE;
    }

    // Single electron triggers
    for (const pair<const string, decor_t>& decorpair : m_1eDecorList ) {
      const auto& trigname = decorpair.first;
      const auto& decor = decorpair.second;
      CHECK(matchSingle(electrons,trigname,decor));
    }

    // Single muon triggers
    for (const pair<const string, decor_t>& decorpair : m_1mDecorList ) {
      const auto& trigname = decorpair.first;
      const auto& decor = decorpair.second;
      CHECK(matchSingle(muons,trigname,decor));
    }

    // Single photon triggers
    for (const pair<const string, decor_t>& decorpair : m_1gDecorList ) {
      const auto& trigname = decorpair.first;
      const auto& decor = decorpair.second;
      CHECK(matchSingle(photons,trigname,decor));
    }

    // Dielectron triggers
    for (const pair<const string, decor_t>& decorpair : m_2eDecorList ) {
      const auto& trigname = decorpair.first;
      const auto& decor = decorpair.second;
      CHECK(matchDi(electrons,electrons,trigname,decor));
    }

    // Dimuon triggers
    for (const pair<const string, decor_t>& decorpair : m_2mDecorList ) {
      const auto& trigname = decorpair.first;
      const auto& decor = decorpair.second;
      CHECK(matchDi(muons,muons,trigname,decor));
    }

    // Electron-muon triggers
    for (const pair<const string, decor_t>& decorpair : m_emDecorList ) {
      const auto& trigname = decorpair.first;
      const auto& decor = decorpair.second;
      CHECK(matchDi(electrons,muons,trigname,decor));
    }
   
    return StatusCode::SUCCESS;
  }


  StatusCode TriggerMatchingAugmentation::matchSingle(const xAOD::IParticleContainer* collection,
						      const std::string& trigger,
						      const decor_t& decor) const {
    std::vector<const xAOD::IParticle*> particles;
    bool fired=m_trigDec->isPassed( trigger );
    for(auto p : *collection) {   
      // Avoid repeating if decoration was already attached
      if(!decor.isAvailable(*p)) {
	if(fired){
	  particles.clear();
	  particles.push_back( p );
	  decor(*p)=m_matchTool->match(particles,trigger);      
	}
      }
      else decor(*p)=false;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode TriggerMatchingAugmentation::matchDi(const xAOD::IParticleContainer* collection1,
						  const xAOD::IParticleContainer* collection2,
						  const std::string& trigger,
						  const decor_t& decor) const {
    std::vector<const xAOD::IParticle*> particles;
    bool matched=false;
    bool fired=m_trigDec->isPassed( trigger );
    for(auto p1 : *collection1) {   
      for(auto p2: *collection2){
	// Avoid repeating if decoration was already attached
	if(!decor.isAvailable(*p1) && !decor.isAvailable(*p2)) {
	  if (fired){
	    particles.clear();
	    particles.push_back( p1 );
	    particles.push_back( p2 );
	    matched=m_matchTool->match(particles,trigger);
	    decor(*p1)=matched;      
	    decor(*p2)=matched;      
	  }
	  else{
	    decor(*p1)=matched;
	    decor(*p2)=matched;
	  }
	}
      }
    }
    return StatusCode::SUCCESS;
  }


  

}
