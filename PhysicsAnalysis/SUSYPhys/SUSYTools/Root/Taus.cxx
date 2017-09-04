/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This source file implements all of the functions related to <OBJECT>
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

// For mu information
#include "xAODEventInfo/EventInfo.h"


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

  // Re-decorate BDT score if requested (Bugfix for 20.7.8.2 derivations)
  if(m_tauIDrecalc){
    if(input.hasDiscriminant(xAOD::TauJetParameters::BDTJetScore)){
      ATH_MSG_VERBOSE( "TAU Unflattened BDT Score " << input.discriminant(xAOD::TauJetParameters::BDTJetScore));
      
      // if(input.hasDiscriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans)) 
      // 	ATH_MSG_VERBOSE( "TAU Buggy Flattened BDT score " << input.discriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans));
      
      // We actually need to decorate some information to the tau to work properly
      const xAOD::EventInfo* evtInfo = 0;
      ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
      
      static SG::AuxElement::Accessor<float> acc_mu("MU");
      acc_mu(input) = evtInfo->averageInteractionsPerCrossing();
      static SG::AuxElement::Accessor<int> acc_numTrack("NUMTRACK");
      acc_numTrack(input) = input.nTracks();
      
      // No re-calculating the BDT score in R21
      // ATH_MSG_VERBOSE( "TAU Fixed Flattened BDT score " << input.discriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans));
    }
    else
      ATH_MSG_ERROR(" Missing BDT Score: Cannot re-calculate decision ");
  }
  

  // If the MVA calibration is being used, be sure to apply the calibration to data as well
  if (fabs(input.eta()) <= 2.5 && input.nTracks() > 0 && (!isData() || m_tauMVACalib)) {

    if(m_tauDoTTM) m_tauTruthMatch->getTruth(input);  // do truth matching first if required (e.g. for running on primary xAOD)
                                                                        
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

  if (dec_baseline(input)) ATH_MSG_VERBOSE("FillTau: passed baseline selection");
  else ATH_MSG_VERBOSE("FillTau: failed baseline selection");
  return StatusCode::SUCCESS;
}


bool SUSYObjDef_xAOD::IsSignalTau(const xAOD::TauJet& input, float ptcut, float etacut) const {

  if ( !dec_baseline(input) ) return false;

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
    if (dec_baseline(tau)) {
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

  ret = m_tauTrigEffTool0->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool0 for systematic var. " << systConfig.name() ); }

  ret = m_tauTrigEffTool1->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool1 for systematic var. " << systConfig.name() ); }

  ret = m_tauTrigEffTool2->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool2 for systematic var. " << systConfig.name() ); }

  ret = m_tauTrigEffTool3->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool3 for systematic var. " << systConfig.name() ); }

  ret = m_tauTrigEffTool4->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool4 for systematic var. " << systConfig.name() ); }


  sf *= GetSignalTauSF(tau, idSF, triggerSF, trigExpr);  


  //Roll back to default
  ret = m_tauEffTool->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool back to default"); }

  ret = m_tauTrigEffTool0->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool0 back to default"); }

  ret = m_tauTrigEffTool1->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool1 back to default"); }

  ret = m_tauTrigEffTool2->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool2 back to default"); }

  ret = m_tauTrigEffTool3->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool3 back to default"); }

  ret = m_tauTrigEffTool4->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool4 back to default"); }


  dec_effscalefact(tau) = sf;

  return sf;

}


double SUSYObjDef_xAOD::GetTauTriggerEfficiencySF(const xAOD::TauJet& tau, const std::string& trigExpr) {

  double eff(1.);

  int trigIdx =  0;
  //check if the trigger is among the supported options
  auto itpos = std::find(tau_trig_support.begin(), tau_trig_support.end(), "HLT_"+trigExpr);  
  if (itpos == tau_trig_support.end()){
    ATH_MSG_WARNING("The trigger item requested ("  << trigExpr << ") is not supported! Please check. Setting to 1 for now.");
    return eff;
  }
  else{
    trigIdx = std::distance(tau_trig_support.begin(), itpos);
  }


  CP::CorrectionCode ret = CP::CorrectionCode::Ok;
  switch(trigIdx){
  case 0 : ret = m_tauTrigEffTool0->getEfficiencyScaleFactor(tau, eff); break;
  case 1 : ret = m_tauTrigEffTool1->getEfficiencyScaleFactor(tau, eff); break;
  case 2 : ret = m_tauTrigEffTool2->getEfficiencyScaleFactor(tau, eff); break;
  case 3 : ret = m_tauTrigEffTool3->getEfficiencyScaleFactor(tau, eff); break;
  case 4 : ret = m_tauTrigEffTool4->getEfficiencyScaleFactor(tau, eff); break;
  default: ATH_MSG_WARNING("The trigger item requested ("  << trigExpr << ") is not supported in SUSYTools! Sorry."); break;
  }

  if(ret != CP::CorrectionCode::Ok){
    ATH_MSG_ERROR("Some problem found to retrieve SF for trigger item  requested ("  << trigExpr << ")");
    eff = -99;
  }
  return eff;
}


double SUSYObjDef_xAOD::GetTotalTauSF(const xAOD::TauJetContainer& taus, const bool idSF, const bool triggerSF, const std::string& trigExpr) {

  double sf(1.);

  for (const auto& tau : taus) {
    // Call this for all taus, which will add the decoration
    double tmpSF = GetSignalTauSF(*tau, idSF, triggerSF, trigExpr);
    if (dec_signal(*tau) && dec_passOR(*tau)) {
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

  ret = m_tauTrigEffTool0->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool0 for systematic var. " << systConfig.name() ); }

  ret = m_tauTrigEffTool1->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool1 for systematic var. " << systConfig.name() ); }

  ret = m_tauTrigEffTool2->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool2 for systematic var. " << systConfig.name() ); }

  ret = m_tauTrigEffTool3->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool3 for systematic var. " << systConfig.name() ); }

  ret = m_tauTrigEffTool4->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool4 for systematic var. " << systConfig.name() ); }


  sf = GetTotalTauSF(taus, idSF, triggerSF, trigExpr);


  //Roll back to default
  ret = m_tauEffTool->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool back to default"); }

  ret = m_tauTrigEffTool0->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool0 back to default"); }

  ret = m_tauTrigEffTool1->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool1 back to default"); }

  ret = m_tauTrigEffTool2->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool2 back to default"); }

  ret = m_tauTrigEffTool3->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool3 back to default"); }

  ret = m_tauTrigEffTool4->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure TauTrigEfficiencyCorrectionsTool4 back to default"); }

  return sf;
}


}
