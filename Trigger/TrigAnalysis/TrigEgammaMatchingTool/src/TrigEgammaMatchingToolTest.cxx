/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaMatchingToolTest.cxx 

#include "TrigEgammaMatchingToolTest.h"
#include "GaudiKernel/Property.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
using std::string;

//**********************************************************************
namespace Trig{
TrigEgammaMatchingToolTest::
TrigEgammaMatchingToolTest(const std::string& name, 
                    ISvcLocator* pSvcLocator )
: ::AthAlgorithm( name, pSvcLocator ),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingTool",this)
{
    declareProperty("TrigEgammaMatchingTool",m_matchTool);
    declareProperty("TriggerList",m_triggerList);
}

//**********************************************************************

TrigEgammaMatchingToolTest::~TrigEgammaMatchingToolTest() { }

//**********************************************************************

StatusCode TrigEgammaMatchingToolTest::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  if ( (m_trigdec.retrieve()).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
      return StatusCode::FAILURE;
  }
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if(sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to locate Service StoreGateSvc" );
      return sc;
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrigEgammaMatchingToolTest::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  for (auto iter = m_counterBits.begin(); iter != m_counterBits.end(); iter++) {
      ATH_MSG_INFO(iter->first << " == " << iter->second);
  }
  for (auto iter = m_counterMatch1Bits.begin(); iter != m_counterMatch1Bits.end(); iter++) {
      ATH_MSG_INFO(iter->first << " " << iter->second << " " << m_counterMatch2Bits[iter->first] << " " <<  m_counterMatch3Bits[iter->first]); 
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrigEgammaMatchingToolTest::execute() {  
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
  if ( (m_storeGate->retrieve(offElectrons,"Electrons")).isFailure() ){
      ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
  }
  ATH_MSG_INFO("Offline Electron container size " << offElectrons->size());
  for(const auto& eg : *offElectrons){
      if (eg) {
          ATH_MSG_INFO(" REGTEST: egamma energy: " << eg->e());
          ATH_MSG_INFO(" REGTEST: egamma eta: " << eg->eta() );
          ATH_MSG_INFO(" REGTEST: egamma phi: " << eg->phi() );
          for(unsigned int ilist = 0; ilist != m_triggerList.size(); ilist++) {
              const HLT::TriggerElement *finalFC; 
              std::string trigger = m_triggerList.at(ilist);
              ATH_MSG_DEBUG("Test Matching Method 1");
              if(m_matchTool->match(eg,trigger)){
                  ATH_MSG_DEBUG("REGTEST:: Method 1 Matched Electron with tool for " << trigger);
                  m_counterMatch1Bits[trigger]++;
              }
              else ATH_MSG_DEBUG("REGTEST::Fails method 3 " << trigger);
              ATH_MSG_DEBUG("Test mathcing method 2");
              if(m_matchTool->match(eg,trigger,finalFC)){
                  ATH_MSG_DEBUG("REGTEST:: Method 2 Matched Electron with tool for " << trigger);
                  if ( finalFC != NULL ){ 
                      if ( (m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC)).te() != NULL ){ 
                          if( (m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC)).te()->getActiveState()){
                              ATH_MSG_DEBUG("REGTEST::Passed Matching " << trigger);
                                m_counterMatch2Bits[trigger]++;
                          }
                          else ATH_MSG_DEBUG("REGTEST::Fails method 2");
                      }
                  }
              }
              ATH_MSG_DEBUG("REGTEST::Test Matching method 3");
              if(m_matchTool->matchHLT(eg,trigger)){
                  ATH_MSG_DEBUG("REGTEST:: Method 3 Matched Electron with tool for " << trigger);
                  m_counterMatch3Bits[trigger]++;
              }
              else ATH_MSG_DEBUG("REGTEST::Fails method 3");
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
  if ( (m_storeGate->retrieve(onlElectrons,"HLT_xAOD__ElectronContainer_egamma_Electrons")).isFailure() ){
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
}
//**********************************************************************
