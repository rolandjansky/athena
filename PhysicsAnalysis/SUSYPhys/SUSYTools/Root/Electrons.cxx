/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This source file implements all of the functions related to <OBJECT>
// in the SUSYObjDef_xAOD class

// Local include(s):
#include "SUSYTools/SUSYObjDef_xAOD.h"

#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

#include "ElectronPhotonFourMomentumCorrection/IEgammaCalibrationAndSmearingTool.h"
#include "ElectronEfficiencyCorrection/IAsgElectronEfficiencyCorrectionTool.h"
#include "AsgAnalysisInterfaces/IEfficiencyScaleFactorTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "ElectronPhotonShowerShapeFudgeTool/IElectronPhotonShowerShapeFudgeTool.h"
#include "ElectronPhotonSelectorTools/IEGammaAmbiguityTool.h"

#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationSelection/IIsolationCloseByCorrectionTool.h"

#include "PATCore/TResult.h"

#ifndef XAOD_STANDALONE // For now metadata is Athena-only
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

namespace ST {

  const static SG::AuxElement::Decorator<char>      dec_passSignalID("passSignalID");
  const static SG::AuxElement::ConstAccessor<char>  acc_passSignalID("passSignalID");

  const static SG::AuxElement::Decorator<char>      dec_passChID("passChID");
  const static SG::AuxElement::ConstAccessor<char>  acc_passChID("passChID");
  const static SG::AuxElement::Decorator<double>    dec_ecisBDT("ecisBDT");

  const static SG::AuxElement::Decorator<float>     dec_sfChIDEff("chargeIDEffiSF"); //tools' default
  const static SG::AuxElement::ConstAccessor<float> acc_sfChIDEff("chargeIDEffiSF"); //tools' default

  const static SG::AuxElement::Decorator<float>     dec_z0sinTheta("z0sinTheta");
  const static SG::AuxElement::ConstAccessor<float> acc_z0sinTheta("z0sinTheta");
  const static SG::AuxElement::Decorator<float>     dec_d0sig("d0sig");
  const static SG::AuxElement::ConstAccessor<float> acc_d0sig("d0sig");

StatusCode SUSYObjDef_xAOD::GetElectrons(xAOD::ElectronContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, bool recordSG, const std::string& elekey, const xAOD::ElectronContainer* containerToBeCopied)
{
  if (!m_tool_init) {
    ATH_MSG_ERROR("SUSYTools was not initialized!!");
    return StatusCode::FAILURE;
  }

  const xAOD::ElectronContainer* electrons(0);
  if (copy==NULL) { // empty container provided
    if (containerToBeCopied != nullptr) {
      electrons = containerToBeCopied;
    }
    else {
      ATH_CHECK( evtStore()->retrieve(electrons, elekey) );
    }
    std::pair<xAOD::ElectronContainer*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*electrons);
    copy = shallowcopy.first;
    copyaux = shallowcopy.second;
    bool setLinks = xAOD::setOriginalObjectLink(*electrons, *copy);
    if (!setLinks) {
      ATH_MSG_WARNING("Failed to set original object links on " << elekey);
    }
  } else { // use the user-supplied collection instead 
    ATH_MSG_DEBUG("Not retrieving electron collecton, using existing one provided by user");
    electrons=copy;
  }

  bool cached_doIsoSig = m_doElIsoSignal;
  for (const auto& electron : *copy) {
    ATH_CHECK( this->FillElectron(*electron, m_eleBaselinePt, m_eleBaselineEta) );
    if(m_doIsoCloseByOR) //switch off isolation for now if close-by OR corrections were requested
      m_doElIsoSignal = false;
    this->IsSignalElectron(*electron, m_elePt, m_eled0sig, m_elez0, m_eleEta);
  }

  //apply close-by corrections to isolation if requested
  if(m_doIsoCloseByOR){
    // stores the electrons in a vector
    std::vector<const xAOD::IParticle*> pVec;
    for(auto pobj: *copy) {
      pVec.push_back((const xAOD::IParticle*) pobj);
    }

    //restore isSignal settings
    m_doElIsoSignal = cached_doIsoSig;

    //correct isolation and propagate to signal deco
    for (const auto& electron : *copy) {
      dec_isol(*electron) = m_isoCloseByTool->acceptCorrected(*electron, pVec);
      if(m_doElIsoSignal) dec_signal(*electron) &= acc_isol(*electron); //add isolation to signal deco if requested
    }
  }

  if (recordSG) {
    ATH_CHECK( evtStore()->record(copy, "STCalib" + elekey + m_currentSyst.name()) );
    ATH_CHECK( evtStore()->record(copyaux, "STCalib" + elekey + m_currentSyst.name() + "Aux.") );
  }
  return StatusCode::SUCCESS;
}


StatusCode SUSYObjDef_xAOD::FillElectron(xAOD::Electron& input, float etcut, float etacut) {

  ATH_MSG_VERBOSE( "Starting FillElectron on el with pre-calibration pt=" << input.pt() );

  // According to https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaIdentificationRun2#Electron_identification:
  // "Please apply the identification to uncalibrated electron object. ID scale factors are to be applied to calibrated objects."
  dec_baseline(input) = false;
  dec_selected(input) = 0;
  dec_signal(input) = false;
  dec_isol(input) = false;
  //dec_passBaseID(input) = false;
  dec_passSignalID(input) = false;
  dec_passChID(input) = false;
  dec_ecisBDT(input) = -999.;

  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
  const xAOD::Vertex* pv = this->GetPrimVtx();
  const xAOD::TrackParticle* track =  input.trackParticle();
  double primvertex_z = pv ? pv->z() : 0;
  double el_z0 = track->z0() + track->vz() - primvertex_z;
  dec_z0sinTheta(input) = el_z0 * TMath::Sin(input.p4().Theta());
  //protect against exception thrown for null or negative d0sig
  try{
    dec_d0sig(input) = xAOD::TrackingHelpers::d0significance( track , evtInfo->beamPosSigmaX(), evtInfo->beamPosSigmaY(), evtInfo->beamPosSigmaXY() );
  }
  catch(...){
    float d0sigError = -99.; 
    ATH_MSG_WARNING("FillElectron : Exception catched from d0significance() calculation. Setting dummy decoration d0sig=" << d0sigError );
    dec_d0sig(input) = d0sigError;
  }

  if (m_debug) {
    unsigned char el_nPixHits(0), el_nSCTHits(0);
    input.trackParticle()->summaryValue(el_nPixHits, xAOD::numberOfPixelHits);
    input.trackParticle()->summaryValue(el_nSCTHits, xAOD::numberOfSCTHits);

    ATH_MSG_INFO( "ELECTRON eta: " << input.eta() );
    ATH_MSG_INFO( "ELECTRON phi: " << input.phi() );
    ATH_MSG_INFO( "ELECTRON cl eta: " << input.caloCluster()->eta() );
    ATH_MSG_INFO( "ELECTRON cl phi: " << input.caloCluster()->phi() );
    ATH_MSG_INFO( "ELECTRON cl e: " << input.caloCluster()->e() );
    ATH_MSG_INFO( "ELECTRON trk eta: " << input.trackParticle()->eta() );
    ATH_MSG_INFO( "ELECTRON trk phi: " << input.trackParticle()->phi() );
    ATH_MSG_INFO( "ELECTRON author: " << input.author() );
    ATH_MSG_INFO( "ELECTRON OQ: " << acc_OQ(input) );
    ATH_MSG_INFO( "ELECTRON nPixHits: " << (int) el_nPixHits );
    ATH_MSG_INFO( "ELECTRON nSCTHits: " << (int) el_nSCTHits );
  }

  // don't bother calibrating or computing WP
  if ( input.pt() < 4e3 ) return StatusCode::SUCCESS;

  if (!input.isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) return StatusCode::SUCCESS;

  if ( m_elecSelLikelihoodBaseline.empty()) {
    ATH_MSG_ERROR("No baseline electron selector defined!");
    return StatusCode::FAILURE;
  }

  if ( !m_elecSelLikelihoodBaseline->accept(&input) )
    if( !m_force_noElId )
      return StatusCode::SUCCESS;
  
  //baseline ID decoration for TauEl OR
  //dec_passBaseID(input) = true;

  // calibrate the electron 4-vector here only if within eta window
  if (fabs(input.caloCluster()->eta()) >= etacut) return StatusCode::SUCCESS;

  if (m_eleBaselineCrackVeto){
    if  ( fabs( input.caloCluster()->etaBE(2) ) >1.37 &&  fabs( input.caloCluster()->etaBE(2) ) <1.52) {
      return StatusCode::SUCCESS; 
    }
  }

  if ( m_egammaCalibTool->applyCorrection(input) != CP::CorrectionCode::Ok)
    ATH_MSG_ERROR( "FillElectron: EgammaCalibTool applyCorrection failed ");

  if (m_isoCorrTool->applyCorrection(input)  != CP::CorrectionCode::Ok)
    ATH_MSG_ERROR("FillElectron: IsolationCorrectionTool applyCorrection failed");

  ATH_MSG_VERBOSE( "FillElectron: post-calibration pt=" << input.pt() );

  //ChargeIDSelector
  if( m_runECIS ){
    dec_passChID(input) = m_elecChargeIDSelectorTool->accept(&input);
    double bdt = m_elecChargeIDSelectorTool->calculate(&input).getResult("bdt");
    dec_ecisBDT(input) = bdt;

    //get ElectronChargeEfficiencyCorrectionTool decorations in this case
    if(m_elecChargeEffCorrTool->applyEfficiencyScaleFactor(input) != CP::CorrectionCode::Ok)
      ATH_MSG_ERROR( "FillElectron: ElectronChargeEfficiencyCorrectionTool SF decoration failed ");

  }

  if (input.pt() < etcut) return StatusCode::SUCCESS;

  if (m_elebaselinez0>0. && acc_z0sinTheta(input)>m_elebaselinez0) return StatusCode::SUCCESS;
  if (m_elebaselined0sig>0. && acc_d0sig(input)>m_elebaselined0sig) return StatusCode::SUCCESS;

  dec_baseline(input) = true;
  dec_selected(input) = 2;
  dec_isol(input) = m_isoTool->accept(input);

  ATH_MSG_VERBOSE( "FillElectron: passed baseline selection" );
  
  return StatusCode::SUCCESS;
}


bool SUSYObjDef_xAOD::IsSignalElectron(const xAOD::Electron & input, float etcut, float d0sigcut, float z0cut, float etacut) const
{
  dec_passSignalID(input) = false;
  
  if ( !m_elecSelLikelihood.empty() && m_elecSelLikelihood->accept(&input) ) dec_passSignalID(input) = true;

  //overwrite ID selection if forced by user
  if(m_force_noElId) dec_passSignalID(input) = true;

  if (!acc_passSignalID(input)) return false;

  if (!acc_baseline(input)) return false;
  if (input.p4().Perp2() <= etcut * etcut || input.p4().Perp2() == 0) return false; // eT cut (might be necessary for leading electron to pass trigger)
  if ( etacut==DUMMYDEF ){
    if(fabs(input.eta()) > m_eleEta ) return false;
  }
  else if ( fabs(input.eta()) > etacut ) return false;

  if (m_eleCrackVeto){
    if  ( fabs( input.caloCluster()->etaBE(2) ) >1.37 &&  fabs( input.caloCluster()->etaBE(2) ) <1.52) {
      return StatusCode::SUCCESS; 
    }
  }

  if (acc_d0sig(input) != 0) {
    if (d0sigcut > 0.0 && fabs(acc_d0sig(input)) > d0sigcut) return false; // transverse IP cut
  }

  if (z0cut > 0.0 && fabs(acc_z0sinTheta(input)) > z0cut) return false; // longitudinal IP cut


  ATH_MSG_VERBOSE( "IsSignalElectron: " << m_eleId << " " << acc_passSignalID(input) << " d0sig " << acc_d0sig(input) << " z0 sin(theta) " << acc_z0sinTheta(input) );

  if (acc_isol(input) || !m_doElIsoSignal) {
    ATH_MSG_VERBOSE( "IsSignalElectron: passed isolation");
  } else return false; //isolation selection with IsoTool

  dec_signal(input) = true;

  ATH_MSG_VERBOSE( "IsSignalElectron: passed selection" );
  return true;
}


float SUSYObjDef_xAOD::GetSignalElecSF(const xAOD::Electron& el,
                                       const bool recoSF,
                                       const bool idSF,
                                       const bool triggerSF,
                                       const bool isoSF,
                                       const std::string& trigExpr,
				       const bool chfSF) {

  if ((m_eleId == "VeryLooseLLH" || m_eleId == "LooseLLH" || m_eleId == "Loose" || m_eleId == "Medium" || m_eleId == "Tight") && (idSF || triggerSF || isoSF)) {
    ATH_MSG_ERROR("No signal electron ID or trigger scale factors provided for the selected working point!");
    ATH_MSG_ERROR("I will now die messily.");
  }

  //shortcut keys for trigger SF config
  static std::string singleLepStr = "singleLepton";
  static std::string diLepStr     = "diLepton";
  static std::string mixedLepStr  = "mixedLepton";

  float sf(1.);

  if (recoSF) {
    double reco_sf(1.);

    CP::CorrectionCode result = m_elecEfficiencySFTool_reco->getEfficiencyScaleFactor(el, reco_sf);
    switch (result) {
    case CP::CorrectionCode::Ok:
      sf *= reco_sf;
      break;
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve signal electron reco SF");
      break;
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "OutOfValidityRange found for signal electron reco SF");
      break;
    default:
      ATH_MSG_WARNING( "Don't know what to do for signal electron reco SF");
    }
  }

  if (idSF) {
    double id_sf(1.);

    CP::CorrectionCode result = m_elecEfficiencySFTool_id->getEfficiencyScaleFactor(el, id_sf);
    switch (result) {
    case CP::CorrectionCode::Ok:
      sf *= id_sf;
      break;
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve signal electron id SF");
      break;
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "OutOfValidityRange found for signal electron id SF");
      break;
    default:
      ATH_MSG_WARNING( "Don't know what to do for signal electron id SF");
    }
  }

  if (triggerSF) {

    //
    std::vector<std::string> trigMChains={};
    std::string theExpr = trigExpr;
    if(trigExpr==singleLepStr)    { trigMChains = (this->treatAsYear()==2015 ? v_trigs15_cache_single  : v_trigs16_cache_single); theExpr=m_electronTriggerSFStringSingle;}
    else if(trigExpr==diLepStr)   { trigMChains = (this->treatAsYear()==2015 ? v_trigs15_cache_dilep   : v_trigs16_cache_dilep);  theExpr=m_electronTriggerSFStringDiLepton;}
    else if(trigExpr==mixedLepStr){ trigMChains = (this->treatAsYear()==2015 ? v_trigs15_cache_mixlep  : v_trigs16_cache_mixlep); theExpr=m_electronTriggerSFStringMixedLepton;}
    else if(trigExpr==m_electronTriggerSFStringSingle)      { trigMChains = (this->treatAsYear()==2015 ? v_trigs15_cache_single  : v_trigs16_cache_single); } 
    else if(trigExpr==m_electronTriggerSFStringDiLepton)    { trigMChains = (this->treatAsYear()==2015 ? v_trigs15_cache_dilep   : v_trigs16_cache_dilep);  }
    else if(trigExpr==m_electronTriggerSFStringMixedLepton) { trigMChains = (this->treatAsYear()==2015 ? v_trigs15_cache_mixlep  : v_trigs16_cache_mixlep); } 
    else{
      //get chains for custom configuration
      trigMChains = GetTriggerOR(theExpr);
    }

    //check matching
    this->TrigMatch({&el}, trigMChains);
    
    if(el.auxdata<int>("trigger_matched")==0){
      ATH_MSG_DEBUG( "Electron was not matched to trigger " << theExpr << " - scale factor does not apply (year " << this->treatAsYear() << ")  Returning 1." );
    }
    else{ //is trig-matched electron, go for it!
      double trig_sf = GetEleTriggerEfficiencySF( el , theExpr );
      sf *= trig_sf;
    }
  }

  if (isoSF) {
    double iso_sf(1.);
    CP::CorrectionCode result = m_elecEfficiencySFTool_iso->getEfficiencyScaleFactor(el, iso_sf);
    switch (result) {
    case CP::CorrectionCode::Ok:
      sf *= iso_sf;
      break;
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve signal electron iso SF");
      break;
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "OutOfValidityRange found for signal electron iso SF");
      break;
    default:
      ATH_MSG_WARNING( "Don't know what to do for signal electron iso SF");
    }
  }

  // new : charge flip SF 
  if (chfSF){
    double chf_sf(1.);

    //ECIS SF 
    CP::CorrectionCode result = m_elecEfficiencySFTool_chf->getEfficiencyScaleFactor(el, chf_sf);
    switch (result) {
    case CP::CorrectionCode::Ok:
      sf *= chf_sf;
      break;
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve signal electron charge-flip SF");
      break;
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "OutOfValidityRange found for signal electron charge-flip SF");
      break;
    default:
      ATH_MSG_WARNING( "Don't know what to do for signal electron charge-flip SF");
    }

    //CHIDEFF SF
    result = m_elecChargeEffCorrTool->getEfficiencyScaleFactor(el, chf_sf);
    switch (result) {
    case CP::CorrectionCode::Ok:
      sf *= chf_sf;
      break;
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve signal electron charge efficiency correction SF");
      break;
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "OutOfValidityRange found for signal electron charge efficiency correction SF");
      break;
    default:
      ATH_MSG_WARNING( "Don't know what to do for signal electron charge efficiency correction SF");
    }   

  }

  
  dec_effscalefact(el) = sf;
  return sf;
}


double SUSYObjDef_xAOD::GetEleTriggerEfficiencySF(const xAOD::Electron& el, const std::string& trigExpr) const {

  double trig_sf(1.);

  CP::CorrectionCode result;
  if ( m_electronTriggerSFStringSingle.find(trigExpr) != std::string::npos ) 
    result = m_elecEfficiencySFTool_trig_singleLep->getEfficiencyScaleFactor(el, trig_sf);
  else if ( m_electronTriggerSFStringDiLepton.find(trigExpr) != std::string::npos && m_electronTriggerSFStringMixedLepton.find(trigExpr) == std::string::npos) 
    result = m_elecEfficiencySFTool_trig_diLep->getEfficiencyScaleFactor(el, trig_sf);
  else if ( m_electronTriggerSFStringMixedLepton.find(trigExpr) != std::string::npos )
    result = m_elecEfficiencySFTool_trig_mixLep->getEfficiencyScaleFactor(el, trig_sf);
  else 
    ATH_MSG_ERROR( "The trigger expression (" << trigExpr << ") is not supported by the electron trigger SF!");
  
  switch (result) {
    case CP::CorrectionCode::Error:
      ATH_MSG_ERROR( "Failed to retrieve signal electron trigger SF");
      return 1.;
    case CP::CorrectionCode::OutOfValidityRange:
      ATH_MSG_VERBOSE( "OutOfValidityRange found for signal electron trigger SF");
      return 1.;
    default:
      break;
  }

  return trig_sf; //CorrectionCode::Ok
}


double SUSYObjDef_xAOD::GetEleTriggerEfficiency(const xAOD::Electron& el, const std::string& trigExpr) const {

  double trig_eff(1.);

  CP::CorrectionCode result;
  if ( m_electronTriggerSFStringSingle.find(trigExpr) != std::string::npos ) 
    result = m_elecEfficiencySFTool_trigEff_singleLep->getEfficiencyScaleFactor(el, trig_eff);
  else if ( m_electronTriggerSFStringDiLepton.find(trigExpr) != std::string::npos && m_electronTriggerSFStringMixedLepton.find(trigExpr) == std::string::npos) 
    result = m_elecEfficiencySFTool_trigEff_diLep->getEfficiencyScaleFactor(el, trig_eff);
  else if ( m_electronTriggerSFStringMixedLepton.find(trigExpr) != std::string::npos )
    result = m_elecEfficiencySFTool_trigEff_mixLep->getEfficiencyScaleFactor(el, trig_eff);
  else 
    ATH_MSG_ERROR( "The trigger expression (" << trigExpr << ") is not supported by the electron trigger efficiency!");
  
  switch (result) {
  case CP::CorrectionCode::Error:
    ATH_MSG_ERROR( "Failed to retrieve signal electron trigger efficiency");
    return 1.;
  case CP::CorrectionCode::OutOfValidityRange:
    ATH_MSG_VERBOSE( "OutOfValidityRange found for signal electron trigger efficiency");
    return 1.;
  default:
    break;
  }

  return trig_eff; //CorrectionCode::Ok
}


  float SUSYObjDef_xAOD::GetTotalElectronSF(const xAOD::ElectronContainer& electrons, const bool recoSF, const bool idSF, const bool triggerSF, const bool isoSF, const std::string& trigExpr, const bool chfSF) {
  float sf(1.);

  for (const auto& electron : electrons) {
    if (!acc_passOR(*electron)) continue;
    if (acc_signal(*electron)) { sf *= this->GetSignalElecSF(*electron, recoSF, idSF, triggerSF, isoSF, trigExpr, chfSF); }
    else { this->GetSignalElecSF(*electron, recoSF, idSF, triggerSF, isoSF, trigExpr, chfSF); }
  }

  return sf;
}


  float SUSYObjDef_xAOD::GetTotalElectronSFsys(const xAOD::ElectronContainer& electrons, const CP::SystematicSet& systConfig, const bool recoSF, const bool idSF, const bool triggerSF, const bool isoSF, const std::string& trigExpr, const bool chfSF) {
  float sf(1.);

  //Set the new systematic variation
  CP::SystematicCode ret = m_elecEfficiencySFTool_reco->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (reco) for systematic var. " << systConfig.name() );
  }

  ret = m_elecEfficiencySFTool_id->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (id) for systematic var. " << systConfig.name() );
  }

  ret = m_elecEfficiencySFTool_trig_singleLep->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
  }

  ret = m_elecEfficiencySFTool_trig_diLep->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
  }

  ret = m_elecEfficiencySFTool_trig_mixLep->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
  }

  ret = m_elecEfficiencySFTool_trigEff_singleLep->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
  }

  ret = m_elecEfficiencySFTool_trigEff_diLep->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
  }

  ret = m_elecEfficiencySFTool_trigEff_mixLep->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
  }

  ret = m_elecEfficiencySFTool_iso->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (iso) for systematic var. " << systConfig.name() );
  }

  ret = m_elecEfficiencySFTool_chf->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (charge-flip) for systematic var. " << systConfig.name() );
  }

  ret = m_elecChargeEffCorrTool->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure ElectronChargeEfficiencyCorrectionTool for systematic var. " << systConfig.name() );
  }


  //Get the total SF for new config
  sf = GetTotalElectronSF(electrons, recoSF, idSF, triggerSF, isoSF, trigExpr, chfSF);

  //Roll back to default
  ret = m_elecEfficiencySFTool_reco->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (reco) back to default.");
  }

  ret = m_elecEfficiencySFTool_id->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (id) back to default.");
  }

  ret = m_elecEfficiencySFTool_trig_singleLep->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) back to default.");
  }

  ret = m_elecEfficiencySFTool_trig_diLep->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) back to default.");
  }

  ret = m_elecEfficiencySFTool_trig_mixLep->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) back to default.");
  }

  ret = m_elecEfficiencySFTool_trigEff_singleLep->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) back to default.");
  }

  ret = m_elecEfficiencySFTool_trigEff_diLep->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) back to default.");
  }

  ret = m_elecEfficiencySFTool_trigEff_mixLep->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) back to default.");
  }

  ret = m_elecEfficiencySFTool_iso->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (iso) back to default.");
  }

  ret = m_elecEfficiencySFTool_chf->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (charge-flip) back to default.");
  }

  ret = m_elecChargeEffCorrTool->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure ElectronChargeEfficiencyCorrectionTool back to default.");
  }

  return sf;
}

}
