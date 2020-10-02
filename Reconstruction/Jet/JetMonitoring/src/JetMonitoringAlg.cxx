/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetMonitoringAlg.h"

#include "xAODJet/Jet.h"

JetMonitoringAlg::JetMonitoringAlg( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_jetContainerKey("AntiKt4LCTopoJets"), m_jetFillerTools(this), m_failureOnMissingContainer(true), m_onlyPassingJets(true)
{
    declareProperty("JetContainerName",m_jetContainerKey);
    declareProperty("FillerTools", m_jetFillerTools);
    declareProperty("FailureOnMissingContainer", m_failureOnMissingContainer);
    declareProperty("OnlyPassingJets", m_onlyPassingJets);
}


JetMonitoringAlg::~JetMonitoringAlg() {}



StatusCode JetMonitoringAlg::initialize() {
  
  ATH_CHECK( m_jetContainerKey.initialize() );
  ATH_CHECK( m_jetFillerTools.retrieve() );

  // print out what we have 
  ATH_MSG_INFO( "Scheduled Histo fillers/selectors : ");
  for(const auto& t: m_jetFillerTools){
    ATH_MSG_INFO( "--> "<< t->name() );
  }
  
  return AthMonitorAlgorithm::initialize();
}



StatusCode JetMonitoringAlg::fillHistograms( const EventContext& ctx ) const {

  if (m_triggerChainString != "" && m_onlyPassingJets) {
    ATH_MSG_DEBUG("JetMonitoringAlg::fillHistograms(const EventContext&) -> enter triggerChainString = "<<m_triggerChainString);
    std::list<const xAOD::Jet*> tmpList;
    const std::vector< TrigCompositeUtils::LinkInfo<xAOD::JetContainer> > fc = getTrigDecisionTool()->features<xAOD::JetContainer>( m_triggerChainString );
    for(const auto& jetLinkInfo : fc) {
      if (!jetLinkInfo.isValid()) {
        ATH_MSG_ERROR("Invalid ElementLink to online jet");
        continue;
      }
      ElementLink<xAOD::JetContainer> j = jetLinkInfo.link;
      const xAOD::Jet *trigjet = dynamic_cast<const xAOD::Jet*>(*j);
      tmpList.push_back( trigjet );
    }
    auto sort = [] (const xAOD::Jet * j1, const xAOD::Jet * j2) {return j1->p4().Et() > j2->p4().Et(); } ;
    tmpList.sort( sort );
    ConstDataVector< xAOD::JetContainer > tmpCont(SG::VIEW_ELEMENTS);
    for(const xAOD::Jet* jet : tmpList ) tmpCont.push_back(jet);
    const xAOD::JetContainer * trigJetsCont = tmpCont.asDataVector();
    if (trigJetsCont->empty()) {
      ATH_MSG_WARNING("Empty trigger jet container for chain "<<m_triggerChainString);
      return StatusCode::SUCCESS;
    }
    for(const auto& t: m_jetFillerTools){
      ATH_MSG_DEBUG( " now run "<< t->name() );
      ATH_CHECK( t->processJetContainer(*this, *trigJetsCont, ctx) );
    }
  } else {
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
