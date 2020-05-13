/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetMonitoringAlg.h"

#include "xAODJet/Jet.h"


JetMonitoringAlg::JetMonitoringAlg( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_jetContainerKey("AntiKt4LCTopoJets"), m_jetFillerTools(this), m_failureOnMissingContainer(true)
{

    declareProperty("JetContainerName",m_jetContainerKey);
    declareProperty("FillerTools", m_jetFillerTools);
    declareProperty("FailureOnMissingContainer", m_failureOnMissingContainer);
}


JetMonitoringAlg::~JetMonitoringAlg() {}



StatusCode JetMonitoringAlg::initialize() {
  
  ATH_CHECK( m_jetContainerKey.initialize() );
  ATH_CHECK( m_jetFillerTools.retrieve() );

  // print out what we have 
  ATH_MSG_INFO( " Scheduled Histo fillers/selectors : ");
  for(const auto& t: m_jetFillerTools){
    ATH_MSG_INFO( "--> "<< t->name() );
  }
  
  return AthMonitorAlgorithm::initialize();
}



StatusCode JetMonitoringAlg::fillHistograms( const EventContext& ctx ) const {

  // retrieve the jet container
  SG::ReadHandle<xAOD::JetContainer> jets(m_jetContainerKey, ctx);    
  if (! jets.isValid() ) {
    if (m_failureOnMissingContainer){
      ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_jetContainerKey);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_WARNING("evtStore() does not contain jet Collection with name "<< m_jetContainerKey);
      return StatusCode::SUCCESS;
    }
  }

  // call each histograming tool on the container
  for(const auto& t: m_jetFillerTools){
    ATH_MSG_DEBUG( " now run "<< t->name() );
    ATH_CHECK( t->processJetContainer(*this, *jets, ctx) );
  }
  
  return StatusCode::SUCCESS;
}




bool JetMonitoringAlg::isPassed(const std::string &c) const {
  return getTrigDecisionTool()->isPassed(c);
}
unsigned int JetMonitoringAlg::isPassedBits(const std::string &c) const {
  //std::cout << " test " << c <<"  " << getTrigDecisionTool()->isPassedBits(c) << std::endl;
  return getTrigDecisionTool()->isPassedBits(c);
}
