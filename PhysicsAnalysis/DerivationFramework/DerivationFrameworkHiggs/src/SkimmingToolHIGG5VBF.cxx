/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SkimmingToolHIGG5VBF.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Yasu Okumura (yasuyuki.okumura@cern.ch)
// Based on DerivationFramework::SkimmingToolExample and DerivationFramework::SkimmingToolHIGG2

#include "DerivationFrameworkHiggs/SkimmingToolHIGG5VBF.h"
#include <vector>
#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

// Constructor
DerivationFramework::SkimmingToolHIGG5VBF::SkimmingToolHIGG5VBF(const std::string& t,
								const std::string& n,
								const IInterface* p) : 
  AthAlgTool(t, n, p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_ntot(0),
  m_npass(0) 
{
  declareInterface<DerivationFramework::ISkimmingTool>(this);  
  
  // for jet multiplicity requirement
  declareProperty("JetContainerKey", m_jetSGKey="AntiKt4EMTopoJets");
  
  // for jet multiplicity
  declareProperty("ReqNAllJets", m_reqNAllJets=false);
  declareProperty("NumberOfAllJets", m_nAllJets=4);
  declareProperty("AllJetPtCut", m_allJetPtCut=40.*CLHEP::GeV); 
  declareProperty("AllJetEtaCut", m_allJetEtaCut=4.9); 
  
  declareProperty("ReqNCentralJets", m_reqNCentralJets=false);
  declareProperty("NumberOfCentralJets", m_nCentralJets=2);
  declareProperty("CentralJetPtCut", m_centralJetPtCut=40.*CLHEP::GeV); 
  declareProperty("CentralJetEtaCut", m_centralJetEtaCut=2.6); 
  
  // for trigger requirement
  declareProperty("ReqTrigger", m_reqTrigger=false); 
  declareProperty("Triggers", m_triggers=std::vector<std::string>()); 
  
  // for Mjj requirement ()
  declareProperty("ReqVBFMjj", m_reqVbfMjj=false);
  declareProperty("MjjCut", m_vbfMjjCut=0.*CLHEP::GeV);
  
  declareProperty("DoDebug", m_debug=false);

  // photon requirement (p. rose)
  declareProperty("PhotonContainerKey", m_phSGKey="Photons");
  declareProperty("ReqPhoton", m_reqPh=false);
  declareProperty("PhotonPtCut", m_phPtCut=0.*CLHEP::GeV);
  declareProperty("CentralPhotonEtaCut", m_centralPhEtaCut=2.6);


}

// Destructor
DerivationFramework::SkimmingToolHIGG5VBF::~SkimmingToolHIGG5VBF() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::SkimmingToolHIGG5VBF::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  // trigger decision tool
  if(m_trigDecisionTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::SkimmingToolHIGG5VBF::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << m_ntot << " events, " << m_npass << " events passed filter ");
  return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::SkimmingToolHIGG5VBF::eventPassesFilter() const
{
  m_ntot++;
  bool acceptEvent(true);
  
  StatusCode sc(StatusCode::SUCCESS, true);
  
  bool isTriggerFired(m_triggers.size()==0);
  for(unsigned int i(0); i<m_triggers.size(); i++) {
    if(m_trigDecisionTool->isPassed(m_triggers.at(i))) {
      isTriggerFired = true;
      break;
    }
  }
  
  // (1) Count Jet Multiplicity
  m_goodAllJets.clear();
  m_goodCentralJets.clear();
  
  if(m_nAllJets>0) {
    const xAOD::JetContainer *jets(0); 
    ATH_CHECK(evtStore()->retrieve(jets, m_jetSGKey));
    xAOD::JetContainer::const_iterator jet_itr(jets->begin());
    xAOD::JetContainer::const_iterator jet_end(jets->end());
    for(; jet_itr != jet_end; ++jet_itr) {
      if(this->checkAllJetQuality(*jet_itr)) { m_goodAllJets.push_back(*jet_itr); }
      if(this->checkCentralJetQuality(*jet_itr)) { m_goodCentralJets.push_back(*jet_itr); }
    }
  }
  
  // (2) evaluate maximum Mjj in the event
  double maxM = 0.;
  for(unsigned int jet_i = 0; jet_i<m_goodAllJets.size(); jet_i++) {  
    const xAOD::Jet* iJet = m_goodAllJets.at(jet_i);
    const xAOD::IParticle::FourMom_t& iP4 = iJet->p4();
    
    for(unsigned int jet_k=jet_i+1; jet_k<m_goodAllJets.size(); jet_k++) {
      const xAOD::Jet* kJet = m_goodAllJets.at(jet_k);
      const xAOD::IParticle::FourMom_t& kP4 = kJet->p4();
      
      const TLorentzVector jjP4 = iP4 + kP4;
      const double jjM = jjP4.M();
      
      if (maxM<jjM) {maxM=jjM;}
    }    
  }
  
  //get max pt of any photon in the event (p. rose)
  double maxPhPt=0.;
  if(m_reqPh){
    const xAOD::PhotonContainer *phots(0);
    ATH_CHECK(evtStore()->retrieve(phots, m_phSGKey));
    for (const auto* ph : *phots){
      if(abs(ph->eta())<m_centralPhEtaCut)
	if(ph->pt()>maxPhPt) maxPhPt = ph->pt();
    }//for
  }//if
  


  const bool passNAllJet     = (m_goodAllJets.size()>=m_nAllJets);
  const bool passNCentralJet = (m_goodCentralJets.size()>=m_nCentralJets);
  const bool passMjjCut      = (maxM>m_vbfMjjCut);
  const bool passPhPtCut     = (maxPhPt>m_phPtCut);

  if (m_reqNAllJets)     { if (not passNAllJet)     {acceptEvent=false;} }
  if (m_reqNCentralJets) { if (not passNCentralJet) {acceptEvent=false;} }
  //if (m_reqVbfMjj)       { if (not passMjjCut)      {acceptEvent=false;} }
  if (m_reqTrigger)      { if (not isTriggerFired)  {acceptEvent=false;} } 
  //vbf+gamma addition -- logical OR of mjj and phpt cut
  if(m_reqVbfMjj and not m_reqPh)     { if (not passMjjCut)                   {acceptEvent=false;} }
  if(m_reqPh     and not m_reqVbfMjj) { if (not passPhPtCut)                  {acceptEvent=false;} }
  if(m_reqPh     and     m_reqVbfMjj) { if (not (passMjjCut or passPhPtCut) ) {acceptEvent=false;} }
  
  if (acceptEvent) {m_npass++;}
  
  if (m_debug) {
    printf("dbg> L%3d : event accepted [%s] " 
	   "(NJets=%2d [%6s], NCentralJets=%2d [%6s] Mjj=%10.1f [%6s], Trigger [%6s]) \n", 
	   __LINE__, 
	   acceptEvent? "Y" : "N",
	   (int)m_goodAllJets.size(),  passNAllJet ? "PASSED" : "FAILED",
	   (int)m_goodCentralJets.size(),  passNCentralJet ? "PASSED" : "FAILED",
	   maxM, passMjjCut ? "PASSED" : "FAILED",
	   isTriggerFired ? "PASSED" : "FAILED"
	   );
  }
  
  return acceptEvent;
}
  
bool DerivationFramework::SkimmingToolHIGG5VBF::checkAllJetQuality(const xAOD::Jet *jet) const 
{
  if(!jet) return false;
  
  if(jet->pt()<m_allJetPtCut) return false;
  if(fabs(jet->eta())>m_allJetEtaCut) return false;
  
  return true;
}

bool DerivationFramework::SkimmingToolHIGG5VBF::checkCentralJetQuality(const xAOD::Jet *jet) const 
{
  if(!jet) return false;
  
  if(jet->pt()<m_centralJetPtCut) return false;
  if(fabs(jet->eta())>m_centralJetEtaCut) return false;
  
  return true;
}

