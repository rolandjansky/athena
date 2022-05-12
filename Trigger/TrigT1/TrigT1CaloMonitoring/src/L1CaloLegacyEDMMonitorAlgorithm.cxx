/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "L1CaloLegacyEDMMonitorAlgorithm.h"

L1CaloLegacyEDMMonitorAlgorithm::L1CaloLegacyEDMMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
}

StatusCode L1CaloLegacyEDMMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("L1CaloLegacyEDMMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);
  ATH_MSG_DEBUG("EmTauRoIContainer"<< m_l1TauRoIKey);
  ATH_MSG_DEBUG("JetRoIContainer"<<m_l1JetRoIKey);
  ATH_MSG_DEBUG("EnergySumRoI"<<m_l1EnergySumRoIKey);

  // we initialise all the containers that we need
  ATH_CHECK(m_l1TauRoIKey.initialize());
  ATH_CHECK(m_l1JetRoIKey.initialize());
  ATH_CHECK(m_l1EnergySumRoIKey.initialize());
  
  return AthMonitorAlgorithm::initialize();
}

StatusCode L1CaloLegacyEDMMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG("L1CaloLegacyEDMMonitorAlgorithm::fillHistograms");

  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;

  SG::ReadHandle<xAOD::EmTauRoIContainer> EmTauRoIs{m_l1TauRoIKey, ctx};
  if(!EmTauRoIs.isValid()){
    ATH_MSG_ERROR("No EM Tau container found in storegate  "<< m_l1TauRoIKey);
    return StatusCode::SUCCESS;
  }
 
  SG::ReadHandle<xAOD::JetRoIContainer> JetRoIs{m_l1JetRoIKey, ctx};
  if(!JetRoIs.isValid()){
    ATH_MSG_ERROR("No Jet container found in storegate  "<< m_l1JetRoIKey);
    return StatusCode::SUCCESS;
  }
 
  SG::ReadHandle<xAOD::EnergySumRoI> EnergyRoIs{m_l1EnergySumRoIKey, ctx};
  if(!EnergyRoIs.isValid()){
    ATH_MSG_ERROR("No EnergySum container found in storegate  "<< m_l1EnergySumRoIKey);
    return StatusCode::SUCCESS;
  }

  auto LVL1EmTauRoIs_eta = Monitored::Scalar<float>("LVL1EmTauRoIs_eta",0.);
  auto LVL1EmTauRoIs_phi = Monitored::Scalar<float>("LVL1EmTauRoIs_phi",0.);
  auto LVL1EmTauRoIs_core = Monitored::Scalar<float>("LVL1EmTauRoIs_core",0.);
  auto LVL1EmTauRoIs_emClus = Monitored::Scalar<float>("LVL1EmTauRoIs_emClus",0.);
  auto LVL1EmTauRoIs_tauClus = Monitored::Scalar<float>("LVL1EmTauRoIs_tauClus",0.);
  auto LVL1EmTauRoIs_hadIsol = Monitored::Scalar<float>("LVL1EmTauRoIs_hadIsol",0.);
  auto LVL1EmTauRoIs_emIsol = Monitored::Scalar<float>("LVL1EmTauRoIs_emIsol",0.);
  auto LVL1EmTauRoIs_hadCore  = Monitored::Scalar<float>("LVL1EmTauRoIs_hadCore" ,0.);  
  auto LVL1EmTauRoIs_thrPattern = Monitored::Scalar<float>("LVL1EmTauRoIs_thrPattern" ,0.);
  
  auto LVL1JetRoIs_eta = Monitored::Scalar<float>("LVL1JetRoIs_eta" ,0.);
  auto LVL1JetRoIs_phi = Monitored::Scalar<float>("LVL1JetRoIs_phi" ,0.);  
  auto LVL1JetRoIs_et4x4 = Monitored::Scalar<float>("LVL1JetRoIs_et4x4" ,0.);
  auto LVL1JetRoIs_et8x8 = Monitored::Scalar<float>("LVL1JetRoIs_et8x8" ,0.);

  auto LVL1EnergySumRoI_energyT = Monitored::Scalar<float>("LVL1EnergySumRoI_energyT" ,0.);
  auto LVL1EnergySumRoI_energyX = Monitored::Scalar<float>("LVL1EnergySumRoI_energyX" ,0.);
  auto LVL1EnergySumRoI_energyY = Monitored::Scalar<float>("LVL1EnergySumRoI_energyY" ,0.);

  for (auto roi : *EmTauRoIs) {
	
    ATH_MSG_DEBUG("isTauRoI:" << " eta: " << roi->eta() <<  " phi: " << roi->phi() );

    LVL1EmTauRoIs_eta = roi->eta();
    fill(m_packageName,LVL1EmTauRoIs_eta);
    
    LVL1EmTauRoIs_phi = roi->phi();
    fill(m_packageName, LVL1EmTauRoIs_phi);
  
    LVL1EmTauRoIs_core = roi->core()/1000.;
    fill(m_packageName,LVL1EmTauRoIs_core);
    LVL1EmTauRoIs_emClus = roi->emClus()/1000.;
    fill(m_packageName,LVL1EmTauRoIs_emClus);
    LVL1EmTauRoIs_tauClus= roi->tauClus()/1000.;
    fill(m_packageName,LVL1EmTauRoIs_tauClus);
    LVL1EmTauRoIs_hadIsol= roi->hadIsol()/1000.;
    fill(m_packageName,LVL1EmTauRoIs_hadIsol);
    LVL1EmTauRoIs_emIsol= roi->emIsol()/1000.;
    fill(m_packageName,LVL1EmTauRoIs_emIsol);
    LVL1EmTauRoIs_hadCore=roi->hadCore()/1000.;
    fill(m_packageName,LVL1EmTauRoIs_hadCore);  
    LVL1EmTauRoIs_thrPattern=roi->thrPattern();
    fill(m_packageName,LVL1EmTauRoIs_thrPattern);
  }

  for (auto roi : *JetRoIs) {

    ATH_MSG_DEBUG("isTauRoI:" << " eta: " << roi->eta() <<  " phi: " << roi->phi() );
    
    LVL1JetRoIs_eta = roi->eta();
    fill(m_packageName,LVL1JetRoIs_eta);
    LVL1JetRoIs_phi = roi->phi();
    fill(m_packageName,LVL1JetRoIs_phi);
    LVL1JetRoIs_et4x4 = roi->et4x4()/1000.;
    fill(m_packageName,LVL1JetRoIs_et4x4);
    LVL1JetRoIs_et8x8 = roi->et8x8()/1000.;
    fill(m_packageName,LVL1JetRoIs_et8x8);
  }

  const xAOD::EnergySumRoI* Energyroi = EnergyRoIs.cptr();
  LVL1EnergySumRoI_energyT = Energyroi->energyT()/1000.;
  fill(m_packageName,LVL1EnergySumRoI_energyT);
  LVL1EnergySumRoI_energyX = Energyroi->energyX()/1000.;
  fill(m_packageName,LVL1EnergySumRoI_energyX);
  LVL1EnergySumRoI_energyY = Energyroi->energyY()/1000.;
  fill(m_packageName,LVL1EnergySumRoI_energyY);
  
  auto run = Monitored::Scalar<int>("run",GetEventInfo(ctx)->runNumber());
  variables.push_back(run);
   
  
  fill(m_packageName,variables);
  variables.clear();
  return StatusCode::SUCCESS;
}


