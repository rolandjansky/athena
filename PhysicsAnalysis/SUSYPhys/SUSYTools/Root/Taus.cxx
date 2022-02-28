/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// This source file implements all of the functions related to TauJets
// in the SUSYObjDef_xAOD class

// Local include(s):
#include "SUSYTools/SUSYObjDef_xAOD.h"

#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/ITauSelectionTool.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/ITauSmearingTool.h"
#include "TauAnalysisTools/ITauTruthMatchingTool.h"
#include "TauAnalysisTools/ITauOverlappingElectronLLHDecorator.h"
#include "tauRecTools/ITauToolBase.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include <boost/algorithm/string.hpp>

#ifndef XAOD_STANDALONE // For now metadata is Athena-only
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

namespace ST {


StatusCode SUSYObjDef_xAOD::GetTaus(xAOD::TauJetContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, bool recordSG,
                                    const std::string& taukey, const xAOD::TauJetContainer* containerToBeCopied)
{
  if (!m_tool_init) {
    ATH_MSG_ERROR("SUSYTools was not initialized!!");
    return StatusCode::FAILURE;
  }
  //Even in case of null pointer containers, we build TruthTau container 
    if(m_tauDoTTM && !isData()) m_tauTruthMatch->retrieveTruthTaus(); 

  const xAOD::TauJetContainer* taus(0);
  if (copy==NULL) { // empty container provided
    if (containerToBeCopied != nullptr) {
      taus = containerToBeCopied;
    }
    else {
      ATH_CHECK( evtStore()->retrieve(taus, taukey) );
    }
    std::pair<xAOD::TauJetContainer*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*taus);
    copy = shallowcopy.first;
    copyaux = shallowcopy.second;
    bool setLinks = xAOD::setOriginalObjectLink(*taus, *copy);
    if (!setLinks) {
      ATH_MSG_WARNING("Failed to set original object links on " << taukey);
    }
  } else { // use the user-supplied collection instead 
    ATH_MSG_DEBUG("Not retrieving tau collecton, using existing one provided by user");
    taus=copy;
  }

  for (const auto& tau : *copy) {
    ATH_CHECK( this->FillTau(*tau) );
    this->IsSignalTau(*tau, m_tauPt, m_tauEta);
  }
  if (recordSG) {
    ATH_CHECK( evtStore()->record(copy, "STCalib" + taukey + m_currentSyst.name()) );
    ATH_CHECK( evtStore()->record(copyaux, "STCalib" + taukey + m_currentSyst.name() + "Aux.") );
  }
  return StatusCode::SUCCESS;
}


// Can't be const due to applyCorrection on the smearing tool
StatusCode SUSYObjDef_xAOD::FillTau(xAOD::TauJet& input) {

  ATH_MSG_VERBOSE( "Starting FillTau on tau with pT = " << input.pt()/1000 << " GeV" );
  ATH_MSG_VERBOSE( "TAU pT before smearing " << input.pt()/1000 << " GeV");

  // do truth matching first (required unless already done in AOD->DAOD and truth matching info kept in DAOD)
  if(m_tauDoTTM && !isData()) m_tauTruthMatch->getTruth(input);
  
  // If the MVA calibration is being used, be sure to apply the calibration to data as well
  if (fabs(input.eta()) <= 2.5 && input.nTracks() > 0) {
    if (m_tauSmearingTool->applyCorrection(input) != CP::CorrectionCode::Ok)
      ATH_MSG_ERROR(" Tau smearing failed " );
  }
  
  ATH_MSG_VERBOSE( "TAU pt after smearing " << input.pt() );

  // decorate tau with electron llh score
  ATH_CHECK( m_tauElORdecorator->decorate( input ));

  // if tauPrePtCut set, apply min pT cut here before calling tau selection tool 
  // (avoid exceptions when running on derivations with removed tracks for low-pT taus, e.g. HIGG4D2)
  if (input.pt() > m_tauPrePtCut) {
    dec_baseline(input) = m_tauSelToolBaseline->accept( input );
  }
  else {
    dec_baseline(input) = false;
  }
  if(acc_baseline(input)) dec_selected(input) = 2;
  else                    dec_selected(input) = 0;

  if (acc_baseline(input)) ATH_MSG_VERBOSE("FillTau: passed baseline selection");
  else ATH_MSG_VERBOSE("FillTau: failed baseline selection");
  return StatusCode::SUCCESS;
}


bool SUSYObjDef_xAOD::IsSignalTau(const xAOD::TauJet& input, float ptcut, float etacut) const {

  dec_signal(input) = false;

  if ( !acc_baseline(input) ) return false;

  if (input.pt() <= ptcut) return false;

  if (fabs(input.eta()) >= etacut) return false;

  if (!m_tauSelTool->accept( input )) return false;

  dec_signal(input) = true; 

  return true;
}


double SUSYObjDef_xAOD::GetSignalTauSF(const xAOD::TauJet& tau, 
                                      const bool idSF, 
                                      const bool triggerSF, 
                                      const std::string& trigExpr)
{

  double sf(1.);

  if(idSF){
    if (acc_baseline(tau)) {
      if (m_tauEffTool->getEfficiencyScaleFactor(tau, sf) != CP::CorrectionCode::Ok) {
        ATH_MSG_WARNING("Failed to retrieve tau efficiency scale factor.");
      }
      ATH_MSG_VERBOSE(" Retrieved tau SF  " << sf);
    }
  }

  if(triggerSF){
    double trig_sf(1.);
    trig_sf = GetTauTriggerEfficiencySF(tau, trigExpr);
    
    if(trig_sf > -90){
      sf *= trig_sf;
      ATH_MSG_VERBOSE(" Retrieved tau trig SF  " << trig_sf);
    } 
  }

  dec_effscalefact(tau) = sf;
  return sf;
}


double SUSYObjDef_xAOD::GetSignalTauSFsys(const xAOD::TauJet& tau, 
                                         const CP::SystematicSet& systConfig,
                                         const bool idSF, 
                                         const bool triggerSF, 
                                         const std::string& trigExpr)
{
  double sf(1.);

  //Set the new systematic variation
  CP::SystematicCode ret = m_tauEffTool->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool for systematic var. " << systConfig.name() ); }

  for(auto& tool : m_tauTrigEffTool) {
    ret = tool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure " << tool->name() << " for systematic var. " << systConfig.name()); }
  }
  
  sf *= GetSignalTauSF(tau, idSF, triggerSF, trigExpr);  

  //Roll back to default
  ret = m_tauEffTool->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool back to default"); }

  for(auto& tool : m_tauTrigEffTool) {
    ret = tool->applySystematicVariation(m_currentSyst);
    if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure " << tool->name() << " back to default"); }
  }
  
  dec_effscalefact(tau) = sf;

  return sf;
}

void SUSYObjDef_xAOD::RetrieveTruthTauContainer(){
  //Build the TruthTaus contaner, or retrieve one if already exists
  m_tauTruthMatch->retrieveTruthTaus(); 

}
  
bool SUSYObjDef_xAOD::GetTauTriggerMatch(const xAOD::TauJet& tau, const std::string& trigExpr){
  bool match = false;

  auto map_it = m_tau_trig_support.find(trigExpr); 
  if(map_it == m_tau_trig_support.end()) {
    ATH_MSG_WARNING("The trigger item requested ("  << trigExpr << ") is not supported! Matching set to false.");
    return match;
  }

  // trigger matching (dR=0.2)
  std::vector<std::string> chains;
  boost::split(chains, map_it->second, [](char c){return c == ',';});

  for(const std::string& chain : chains)
    if(m_trigMatchingTool->match({&tau}, chain, 0.2)) {
      match = true;
      break;
    }
  return match;
}

double SUSYObjDef_xAOD::GetTauTriggerEfficiencySF(const xAOD::TauJet& tau, const std::string& trigExpr) {
  bool match = false;
  return GetTauTriggerEfficiencySF(tau, match,trigExpr);
}

double SUSYObjDef_xAOD::GetTauTriggerEfficiencySF(const xAOD::TauJet& tau, bool& match, const std::string& trigExpr) {

  double eff(1.);
  match = false;

  auto map_it = m_tau_trig_support.find(trigExpr); 
  if(map_it == m_tau_trig_support.end()) {
    ATH_MSG_WARNING("The trigger item requested ("  << trigExpr << ") is not supported! Please check. Setting SF to 1.");
    return eff;
  }

  match = GetTauTriggerMatch(tau,trigExpr);

  if(!match) {
    ATH_MSG_VERBOSE("Tau did not match trigger " << trigExpr);
    return eff;
  }

  int trigIdx = std::distance(std::begin(m_tau_trig_support), map_it);
  if(m_tauTrigEffTool.at(trigIdx)->getEfficiencyScaleFactor(tau, eff) != CP::CorrectionCode::Ok) {
    ATH_MSG_ERROR("Some problem found to retrieve SF for trigger item  requested ("  << trigExpr << ")");
    eff = -99;
  }
  return eff;
}


double SUSYObjDef_xAOD::GetTotalTauSF(const xAOD::TauJetContainer& taus, const bool idSF, const bool triggerSF, const std::string& trigExpr) {

  double sf(1.);

  for (const xAOD::TauJet* tau : taus) {
    // Call this for all taus, which will add the decoration
    double tmpSF = GetSignalTauSF(*tau, idSF, triggerSF, trigExpr);
    if (acc_signal(*tau) && acc_passOR(*tau)) {
      sf *= tmpSF;
    }
  }
  return sf;
}


double SUSYObjDef_xAOD::GetTotalTauSFsys(const xAOD::TauJetContainer& taus, const CP::SystematicSet& systConfig, const bool idSF, const bool triggerSF, const std::string& trigExpr){
  double sf(1.);

  //Set the new systematic variation
  CP::SystematicCode ret = m_tauEffTool->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool for systematic var. " << systConfig.name() ); }

  for(auto& tool : m_tauTrigEffTool) {
    ret = tool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure " << tool->name() << " for systematic var. " << systConfig.name()); }
  }

  sf = GetTotalTauSF(taus, idSF, triggerSF, trigExpr);

  //Roll back to default
  ret = m_tauEffTool->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool back to default"); }

  for(auto& tool : m_tauTrigEffTool) {
    ret = tool->applySystematicVariation(m_currentSyst);
    if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure " << tool->name() << " back to default"); }
  }

  return sf;
}


}
