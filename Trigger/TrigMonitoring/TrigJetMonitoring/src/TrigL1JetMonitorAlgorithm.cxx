/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL1JetMonitorAlgorithm.h"

TrigL1JetMonitorAlgorithm::TrigL1JetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
  declareProperty("L1JetContainer", m_l1jetContainerkey = "LVL1JetRoIs");
}

TrigL1JetMonitorAlgorithm::~TrigL1JetMonitorAlgorithm() {}

StatusCode TrigL1JetMonitorAlgorithm::initialize() {
  ATH_CHECK(m_l1jetContainerkey.initialize());
  return AthMonitorAlgorithm::initialize();
}

StatusCode TrigL1JetMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  // Retrieve the L1 jet container
  SG::ReadHandle<xAOD::JetRoIContainer> jets(m_l1jetContainerkey, ctx);
  if( !jets.isValid() ){
    ATH_MSG_WARNING("evtStore() does not contain the L1 jet collection with name " << m_l1jetContainerkey);
    return StatusCode::FAILURE;
  }

  // Declare the quantities which should be monitored
  auto et8x8 = Monitored::Scalar<float>("et8x8");
  auto eta   = Monitored::Scalar<float>("eta");
  auto phi   = Monitored::Scalar<float>("phi");

  // Loop over jets
  for(const auto& j : *jets){
    // Set the values of the monitored variables for the event		
    et8x8 = j->et8x8()*0.001;
    eta   = j->eta();
    phi   = j->phi();
    // Fill. First argument is the tool (GMT) name as defined in TrigJetMonitorAlgorithm.py, 
    // all others are the variables to be saved.
    auto tool = getGroup("TrigL1JetMonitor");
    fill(tool,et8x8,eta,phi);
  }

  return StatusCode::SUCCESS;
}
