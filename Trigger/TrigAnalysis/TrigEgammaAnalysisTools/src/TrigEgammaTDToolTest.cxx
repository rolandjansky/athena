/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaTDToolTest.cxx 

#include "TrigEgammaTDToolTest.h"
#include "Gaudi/Property.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
using std::string;

//**********************************************************************

TrigEgammaTDToolTest::
TrigEgammaTDToolTest(const std::string& name, 
                    ISvcLocator* pSvcLocator )
: ::AthAlgorithm( name, pSvcLocator ),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool")
{
    declareProperty("TrigEgammaMatchingTool",m_matchTool);
}

//**********************************************************************

TrigEgammaTDToolTest::~TrigEgammaTDToolTest() { }

//**********************************************************************

StatusCode TrigEgammaTDToolTest::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  if ( (m_trigdec.retrieve()).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
      return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrigEgammaTDToolTest::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  for (auto iter = m_counterBits.begin(); iter != m_counterBits.end(); iter++) {
      ATH_MSG_INFO(iter->first << " == " << iter->second);
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrigEgammaTDToolTest::execute() {  
  ATH_MSG_INFO ("Executing " << name() << "...");
  ATH_MSG_INFO( "L1: " << m_trigdec->isPassed( "L1_.*" )
          << ", L2: " << m_trigdec->isPassed( "L2_.*" )
          << ", EF: " << m_trigdec->isPassed( "EF_.*" )
          << ", HLT: " << m_trigdec->isPassed( "HLT_.*" ) );
  auto chainGroups = m_trigdec->getChainGroup("HLT_e.*");
     
  for(auto &trig : chainGroups->getListOfTriggers()) {
      if(m_trigdec->isPassed(trig))
          ATH_MSG_INFO("Passed: " << trig);
      m_counterBits[trig]+=m_trigdec->isPassed(trig);
      
  }

  //Check Containers

  const xAOD::ElectronContainer *offElectrons = 0;
  const xAOD::ElectronContainer *onlElectrons = 0;
  if ( (evtStore()->retrieve(offElectrons,"Electrons")).isFailure() ){
      ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
  }
  ATH_MSG_INFO("Offline Electron container size " << offElectrons->size());
  for(const auto& eg : *offElectrons){
      if (eg) {
          ATH_MSG_INFO(" REGTEST: egamma energy: " << eg->e());
          ATH_MSG_INFO(" REGTEST: egamma eta: " << eg->eta() );
          ATH_MSG_INFO(" REGTEST: egamma phi: " << eg->phi() );
          for(auto &trig : chainGroups->getListOfTriggers()){
              trig.erase(0,4);
              if(m_matchTool->match(eg,trig)) ATH_MSG_DEBUG("REGTEST:: Matched Electron with tool for " << trig);
          }
        
      } else{
          ATH_MSG_INFO(" REGTEST: problems with egamma pointer");
      }
      if(eg->trackParticle()){
              ATH_MSG_INFO(" REGTEST: pt=  " << eg->trackParticle()->pt()); 
              ATH_MSG_INFO(" REGTEST: charge=  " << eg->trackParticle()->charge()) ;
              ATH_MSG_INFO(" REGTEST: eta=  " << eg->trackParticle()->eta()); 
              ATH_MSG_INFO(" REGTEST: phi=  " << eg->trackParticle()->phi()) ;
      }
      if(eg->caloCluster()){
              ATH_MSG_INFO(" REGTEST: et=  " << eg->caloCluster()->et()); 
              ATH_MSG_INFO(" REGTEST: eta=  " << eg->caloCluster()->eta()); 
              ATH_MSG_INFO(" REGTEST: phi=  " << eg->caloCluster()->phi()) ;
      }
      else ATH_MSG_INFO("No caloCluster");
  } 
  if ( (evtStore()->retrieve(onlElectrons,"HLT_xAOD__ElectronContainer_egamma_Electrons")).isFailure() ){
      ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
  }
  ATH_MSG_INFO("Online Electron container size " << onlElectrons->size());
  for(const auto& eg : *onlElectrons){
      if (eg) {
          ATH_MSG_INFO(" REGTEST: egamma energy: " << eg->e());
          ATH_MSG_INFO(" REGTEST: egamma eta: " << eg->eta() );
          ATH_MSG_INFO(" REGTEST: egamma phi: " << eg->phi() );

      } else{
          ATH_MSG_INFO(" REGTEST: problems with egamma pointer");
      }
      if(eg->trackParticle()){
              ATH_MSG_INFO(" REGTEST: pt=  " << eg->trackParticle()->pt()); 
              ATH_MSG_INFO(" REGTEST: charge=  " << eg->trackParticle()->charge()) ;
              ATH_MSG_INFO(" REGTEST: eta=  " << eg->trackParticle()->eta()); 
              ATH_MSG_INFO(" REGTEST: phi=  " << eg->trackParticle()->phi() );
      }
      if(eg->caloCluster()){
              ATH_MSG_INFO(" REGTEST: et=  " << eg->caloCluster()->et()); 
              ATH_MSG_INFO(" REGTEST: eta=  " << eg->caloCluster()->eta()); 
              ATH_MSG_INFO(" REGTEST: phi=  " << eg->caloCluster()->phi()) ;
      }
      else ATH_MSG_INFO("No caloCluster");
  } 
  return StatusCode::SUCCESS;
}

//**********************************************************************
