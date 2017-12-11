/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// HnlSkimmingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Avishek Chatterjee (avishek.chatterjee@cern.ch)
// Heavily borrowed from DerivationFramework/DerivationFrameworkExamples/src/SkimmingToolExample.cxx 
// and DerivationFramework/DerivationFrameworkMuons/src/dimuonTaggingTool.cxx

#include "LongLivedParticleDPDMaker/HnlSkimmingTool.h"
#include "AthenaKernel/errorcheck.h"
#include <vector>
#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"

// Constructor
DerivationFramework::HnlSkimmingTool::HnlSkimmingTool(const std::string& t,
						      const std::string& n,
						      const IInterface* p):
  AthAlgTool(t, n, p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareInterface<DerivationFramework::ISkimmingTool>(this);
  declareProperty("TrigDecisionTool", m_trigDecisionTool, "Tool to access the trigger decision");
  declareProperty("Triggers", m_triggers=std::vector< std::string >());

  declareProperty("MuonContainerKey", m_muonSGKey="Muons");
  declareProperty("Mu1PtMin", m_mu1PtMin=-1);
  declareProperty("Mu1AbsEtaMax", m_mu1AbsEtaMax=2.5);
  declareProperty("Mu1Types", m_mu1Types=std::vector< int >());
  declareProperty("Mu1IsoType", m_mu1IsoType=14); //ptcone30
  declareProperty("Mu1IsoCutIsRel", m_mu1IsoCutIsRel=1); //Cut is on relative isolation
  declareProperty("Mu1IsoCut", m_mu1IsoCut=0.05);

  declareProperty("Mu2PtMin", m_mu2PtMin=-1);
  declareProperty("Mu2AbsEtaMax", m_mu2AbsEtaMax=2.5);
  declareProperty("Mu2Types", m_mu2Types=std::vector< int >());
  declareProperty("Mu2IsoType", m_mu2IsoType=14); //ptcone30
  declareProperty("Mu2IsoCutIsRel", m_mu2IsoCutIsRel=1); //Cut is on relative isolation
  declareProperty("Mu2IsoCut", m_mu2IsoCut=1.);
  declareProperty("Mu2d0Min", m_mu2d0Min=0.1);
}
  
// Destructor
DerivationFramework::HnlSkimmingTool::~HnlSkimmingTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::HnlSkimmingTool::initialize()
{
  ATH_MSG_VERBOSE("HnlSkimmingTool initialize() ...");

  if(m_trigDecisionTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::HnlSkimmingTool::finalize()
{
  ATH_MSG_VERBOSE("HnlSkimmingTool finalize() ...");
  return StatusCode::SUCCESS;
}

// The filter itself
//Add of the chi2 cut
bool DerivationFramework::HnlSkimmingTool::IsGood(const xAOD::Muon& mu) const
{
  if(mu.muonType() != xAOD::Muon::Combined) return false;

  float chi2 = 0.;
  if(!mu.parameter(chi2, xAOD::Muon::msInnerMatchChi2)) return false;

  int dof = 1;
  if(!mu.parameter(dof, xAOD::Muon::msInnerMatchDOF)) return false;
  if(dof == 0) dof = 1;

  return (chi2 / static_cast<float>(dof)) < 5.;
}
/////////////////////End of add of the chi2 cut
bool DerivationFramework::HnlSkimmingTool::eventPassesFilter() const
{
  bool acceptEvent(false);

  bool passedTrigger(true);
  if(m_triggers.size()>0){
    passedTrigger = false;
    for(unsigned int i=0; i<m_triggers.size(); i++){
      int decision = m_trigDecisionTool->isPassed(m_triggers.at(i));
      if(decision == 1){
	passedTrigger = true;
	break;
      }
    }
  }

  if(!passedTrigger) return acceptEvent;
 
  // Retrieve muon container	
  const xAOD::MuonContainer* muons(0);
  StatusCode sc = evtStore()->retrieve(muons,m_muonSGKey);	
  if (sc.isFailure()) {
    ATH_MSG_FATAL("No muon collection with name " << m_muonSGKey << " found in StoreGate!");
    return false;
  }
  
  bool muon1passed(false);
  xAOD::MuonContainer::const_iterator mu_itr1;
  for(mu_itr1=muons->begin(); mu_itr1!=muons->end(); mu_itr1++) {
    bool passTypeCut = true;
    if(m_mu1Types.size()>0){
      passTypeCut = false;
      int type = (*mu_itr1)->muonType();
      for(unsigned int i=0; i<m_mu1Types.size(); ++i) {if(m_mu1Types[i]==type) {passTypeCut=true; break;}}
    }
    bool isIso=false; float isoValue = 0.;
    const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_mu1IsoType);
    if(m_mu1IsoCutIsRel==0){
      if((*mu_itr1)->isolation(isoValue, isoType) && isoValue < m_mu1IsoCut) isIso = true;
    }else{
      if((*mu_itr1)->isolation(isoValue, isoType) && isoValue/(*mu_itr1)->pt() < m_mu1IsoCut) isIso = true;
    }
    if((*mu_itr1)->pt()>m_mu1PtMin && fabs((*mu_itr1)->eta())<m_mu1AbsEtaMax && passTypeCut && isIso){
      muon1passed = true;
      break;
    }
  }
  
  if(!muon1passed) return acceptEvent;

  bool muon2passed(false);
  xAOD::MuonContainer::const_iterator mu_itr2;
  for(mu_itr2=muons->begin(); mu_itr2!=muons->end(); mu_itr2++) {
    if(mu_itr2==mu_itr1) continue;
    bool passTypeCut = true;
    if(m_mu2Types.size()>0){
      passTypeCut = false;
      int type = (*mu_itr2)->muonType();
      for(unsigned int i=0; i<m_mu2Types.size(); ++i) {if(m_mu2Types[i]==type) {passTypeCut=true; break;}}
    }
    bool isIso=false; float isoValue = 0.;
    const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_mu2IsoType);
    if(m_mu2IsoCutIsRel==0){
      if((*mu_itr2)->isolation(isoValue, isoType) && isoValue < m_mu2IsoCut) isIso = true;
    }else{
      if((*mu_itr2)->isolation(isoValue, isoType) && isoValue/(*mu_itr2)->pt() < m_mu2IsoCut) isIso = true;
    }
    bool passD0cut = true;
    int type = (*mu_itr2)->muonType();
    const xAOD::Muon *mu = (*mu_itr2);

    if(type==0){ //d0 cut is for combined muons only
      passD0cut = false;
      if(IsGood(*mu)){//if muon has a good chi2/dof
        if(fabs((*mu_itr2)->primaryTrackParticle()->d0())>m_mu2d0Min) passD0cut = true;
      }
      else{passD0cut = true;}
    }
    if((*mu_itr2)->pt()>m_mu2PtMin && fabs((*mu_itr2)->eta())<m_mu2AbsEtaMax && passTypeCut && isIso && passD0cut){
      muon2passed = true;
      break;
    }
  }
 
  
  if(muon2passed) acceptEvent = true;
  
  return acceptEvent; 
}

