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

  TriggerMatchingAugmentation::TriggerMatchingAugmentation(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p),
    m_tool("Trig::MatchingTool"),
    m_trigDec( "Trig::TrigDecisionTool/DFTriggerMatch_TrigDecisionTool" ),
    m_sgName(""),
    m_muonContainerName(""),
    m_electronContainerName(""),
    m_photonContainerName(""),
    m_singletriggerList(),
    m_2mutriggerList(),
    m_2etriggerList(),
    m_emtriggerList()
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("MatchingTool", m_tool);
    declareProperty("DecorationPrefix", m_sgName);
    declareProperty("PhotonContainerName", m_photonContainerName);
    declareProperty("ElectronContainerName", m_electronContainerName);
    declareProperty("MuonContainerName", m_muonContainerName);
    declareProperty("SingleTriggerList", m_singletriggerList);
    declareProperty("DiMuonList", m_2mutriggerList);
    declareProperty("DiElectronList", m_2etriggerList);
    declareProperty("ElectronMuonList", m_emtriggerList);
    declareProperty( "TrigDecisionTool", m_trigDec);
    //    declareProperty("TriggerList", m_triggerList);
  }

  StatusCode TriggerMatchingAugmentation::initialize()
  {
    m_trigDec.setTypeAndName( "Trig::TrigDecisionTool/DFTriggerMatch_TrigDecisionTool" );
    CHECK( m_trigDec.retrieve() );
    CHECK( m_trigDec->initialize() );
    m_tool.setTypeAndName("Trig::MatchingTool/DFTriggerMatchMatchingTool");
    CHECK(m_tool.retrieve()); //important to retrieve here, because TrigDecisionTool must be initialized before event loop
    

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

    ATH_CHECK(m_tool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerMatchingAugmentation::finalize()
  {
    return StatusCode::SUCCESS;
  }
 
  StatusCode TriggerMatchingAugmentation::addBranches() const
  {

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
    

    for (auto trigger :m_singletriggerList){
      if (trigger.find("HLT_m")!=std::string::npos)CHECK(matchSingle(muons,trigger));
      else if (trigger.find("HLT_e")!=std::string::npos) CHECK(matchSingle(electrons,trigger));
      else CHECK(matchSingle(photons,trigger));
    }

    for( auto trigger:m_2etriggerList){ CHECK(matchDi(electrons,electrons,trigger));}
    for( auto trigger:m_2mutriggerList){ CHECK(matchDi(muons,muons,trigger));}
    for( auto trigger:m_emtriggerList){ CHECK(matchDi(electrons,muons,trigger));}

   
    return StatusCode::SUCCESS;
  }


  StatusCode TriggerMatchingAugmentation::matchSingle(const xAOD::IParticleContainer* collection,std::string trigger) const{
    SG::AuxElement::Decorator< char > decTrig(m_sgName+trigger);
    std::vector<const xAOD::IParticle*> particles;
    bool fired=m_trigDec->isPassed( trigger );
    for(auto p : *collection) {   
      if(fired){
	particles.clear();
	particles.push_back( p );      
	decTrig(*p)=m_tool->match(particles,trigger);      
      }
      else decTrig(*p)=false;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode TriggerMatchingAugmentation::matchDi(const xAOD::IParticleContainer* collection1 ,const xAOD::IParticleContainer* collection2 ,std::string trigger) const{
    SG::AuxElement::Decorator< char > decTrig(m_sgName+trigger);
    std::vector<const xAOD::IParticle*> particles;
    bool matched=false;
    bool fired=m_trigDec->isPassed( trigger );
    for(auto p1 : *collection1) {   
      for(auto p2: *collection2){
	if (fired){
	  particles.clear();
	  particles.push_back( p1 );
	  particles.push_back( p2 );
	  matched=m_tool->match(particles,trigger);
	  decTrig(*p1)=matched;      
	  decTrig(*p2)=matched;      
	}
	else{
	  decTrig(*p1)=matched;
	  decTrig(*p2)=matched;
	}
      }
    }
    return StatusCode::SUCCESS;
  }


  

}
