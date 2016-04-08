/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TestMatchingToolAlg.h"
#include "xAODBase/IParticleContainer.h"

StatusCode TestMatchingToolAlg::initialize() {
  m_tmt.setTypeAndName("Trig::MatchingTool/MyMatchingTool");
  CHECK(m_tmt.retrieve()); //important to retrieve here, because TrigDecisionTool must be initialized before event loop
  return StatusCode::SUCCESS;
}

StatusCode TestMatchingToolAlg::execute() {  

  //For more documentation on the tool, see: https://twiki.cern.ch/twiki/bin/view/Atlas/XAODMatchingTool
  //As of Feb 2016:
  //Recommended threshold for egamma triggers: 0.07
  //Recommended threshold for muon triggers: 0.1

  //here's an example of using the tool
  const xAOD::IParticleContainer* electrons = 0;
  const xAOD::IParticleContainer *taus = 0;
  const xAOD::IParticleContainer *muons = 0;
  CHECK( evtStore()->retrieve( electrons, "Electrons" ));
  CHECK( evtStore()->retrieve( muons, "Muons" ) );
  CHECK( evtStore()->retrieve( taus, "TauJets" ) );
  if(electrons) ATH_MSG_INFO("Offline Electron container size " << electrons->size());
  if(muons) ATH_MSG_INFO("Offline Muon container size " << muons->size());
  if(taus) ATH_MSG_INFO("Offline Tau container size " << taus->size());
  //tool takes a vector of IParticles, and a trigger chain expression
  //can also take a single IParticle for single object trigger matching
  std::vector<const xAOD::IParticle*> myParticles;

  //here's an example of a single object trigger
  if (electrons) {
    for(uint i = 0; i< electrons->size(); i++) {
      myParticles.clear();
      myParticles.push_back( electrons->at(i) );
      ATH_MSG_INFO("HLT_e17_lhloose Matching Decision = " << m_tmt->match(myParticles,"HLT_e17_lhloose",0.07 /*explicit dR threhsold*/) );
      
      // here's an example of a combined trigger
      // e-mu
      if(muons){
          for(uint j = 0; j < muons->size(); j++) {
              myParticles.clear();
              myParticles.push_back(electrons->at(i));
              myParticles.push_back(muons->at(j));
              ATH_MSG_INFO("HLT_e17_lhloose_mu14 = " << m_tmt->match(myParticles,"HLT_e17_lhloose_mu14"));
          }
      }
      // e-tau
      if(taus){
        for(uint j = 0; j < taus->size(); j++) {
          myParticles.clear();
          myParticles.push_back(electrons->at(i));
          myParticles.push_back(taus->at(j));
          ATH_MSG_INFO("HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo = " << m_tmt->match(myParticles,"HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo"));
        }
      }
    }
  }

  // here's an example for muon trigger, using the method for single-object trigger matching
  if(muons){
      for(auto muon : *muons) {
          ATH_MSG_INFO("HLT_mu18 = " << m_tmt->match(*muon,"HLT_mu18"));
      }
  }
  // here's an examplefor a tau trigger
  if(taus){
      for(uint j = 0; j < taus->size(); j++) {
          myParticles.clear();
          myParticles.push_back(taus->at(j));
          ATH_MSG_INFO("HLT_tau25_loose1_ptonly = " << m_tmt->match(myParticles,"HLT_tau25_loose1_ptonly"));
      }
  }

  //here's an example for a dilepton trigger
  //form pairs to test a dilepton trigger
  for(uint i = 0; i< electrons->size()-1; i++) {
      for(uint j = i+1; j < electrons->size(); j++) {
          myParticles.clear();
          myParticles.push_back( electrons->at(i) );
          myParticles.push_back( electrons->at(j) );
          ATH_MSG_INFO("HLT_2e17_lhloose Matching Decision = " << m_tmt->match(myParticles,"HLT_2e17_lhloose") );
      }
  }

  return StatusCode::SUCCESS;
}
