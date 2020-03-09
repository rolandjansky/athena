/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEfficiencyMonMT.h"
#include "MuonMatchingTool.h"

TrigMuonEfficiencyMonMT :: TrigMuonEfficiencyMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator),
    m_matchTool("MuonMatchingTool", this),
    m_MuonRoIContainerKey("LVL1MuonRoIs")
{}


StatusCode TrigMuonEfficiencyMonMT :: initialize(){
  StatusCode sc = TrigMuonMonitorAlgorithm::initialize();
  ATH_CHECK( m_matchTool.retrieve() );
  ATH_CHECK( m_MuonRoIContainerKey.initialize() );
  return sc;
}


bool TrigMuonEfficiencyMonMT :: selectEvents() const {
  if(m_event_trigger.empty()) return true;
  return getTrigDecisionTool()->isPassed(m_event_trigger);
}



bool TrigMuonEfficiencyMonMT :: selectMuons(SG::ReadHandle<xAOD::MuonContainer> &muons, std::vector<const xAOD::Muon*> &probes) const {
  for (const xAOD::Muon* mu : *muons) {
    if( mu->muonType()==xAOD::Muon::Combined ){
      probes.push_back(mu);
    }
  }
  return true;
}



StatusCode TrigMuonEfficiencyMonMT :: fillVariablesPerOfflineMuon(const EventContext& ctx, const xAOD::Muon* mu) const {


  auto muPt = Monitored::Scalar<double>(m_group+"_muPt",-1.0);
  auto muPt_L1passed = Monitored::Scalar<float>(m_group+"_muPt_L1passed",false);
  auto muPt_SApassed = Monitored::Scalar<float>(m_group+"_muPt_SApassed",false);
  auto muPt_CBpassed = Monitored::Scalar<float>(m_group+"_muPt_CBpassed",false);
  auto muPt_EFpassed = Monitored::Scalar<float>(m_group+"_muPt_EFpassed",false);

  SG::ReadHandle<xAOD::MuonRoIContainer> rois(m_MuonRoIContainerKey, ctx);
  if (! rois.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain xAOD::MuonRoI collection with name "<< m_MuonRoIContainerKey);
    return StatusCode::FAILURE;
  }
  if(rois->getConstStore()==nullptr){
    xAOD::MuonRoIContainer *ncptr = const_cast<xAOD::MuonRoIContainer*>(rois.get());
    ncptr->setStore(DataLink<SG::IConstAuxStore>(m_MuonRoIContainerKey.key()+"Aux.", ctx));
  }

  muPt = mu->pt()/1e3;

  bool L1pass = false;
  bool SApass = false;
  bool CBpass = false;
  bool EFpass = false;
  m_matchTool->matchL1(mu, rois, m_measured_trigger_L1, L1pass);
  muPt_L1passed = L1pass;
  if(L1pass){
    m_matchTool->matchSA(mu, m_measured_trigger, SApass);
    muPt_SApassed = SApass;
  }
  if(SApass){
    m_matchTool->matchCB(mu, m_measured_trigger, CBpass);
    muPt_CBpassed = CBpass;
  }
  if(CBpass){
    m_matchTool->matchEF(mu, m_measured_trigger, EFpass);
    muPt_EFpassed = EFpass;
  }
    ATH_MSG_DEBUG("L1pass:" << L1pass << " SAPass:" << SApass << " CBpass:" << CBpass << " EFpass:" << EFpass);


  fill(m_group, muPt, muPt_L1passed, muPt_SApassed, muPt_CBpassed, muPt_EFpassed);



  return StatusCode::SUCCESS;
}
