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
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"
#include "ElectronPhotonShowerShapeFudgeTool/IElectronPhotonShowerShapeFudgeTool.h"
#include "ElectronPhotonSelectorTools/IEGammaAmbiguityTool.h"
#include "PhotonEfficiencyCorrection/IAsgPhotonEfficiencyCorrectionTool.h"

#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationSelection/IIsolationCloseByCorrectionTool.h"

// Helper for object quality
#include "EgammaAnalysisHelpers/PhotonHelpers.h"

#ifndef XAOD_STANDALONE // For now metadata is Athena-only
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

namespace ST {

StatusCode SUSYObjDef_xAOD::GetPhotons(xAOD::PhotonContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, bool recordSG, const std::string& photonkey, const xAOD::PhotonContainer* containerToBeCopied)
{
  if (!m_tool_init) {
    ATH_MSG_ERROR("SUSYTools was not initialized!!");
    return StatusCode::FAILURE;
  }
  const xAOD::PhotonContainer* photons(0);
  if (copy==NULL) { // empty container provided
    if (containerToBeCopied != nullptr) {
      photons = containerToBeCopied;
    }
    else {
      ATH_CHECK( evtStore()->retrieve(photons, photonkey) );
    }
    std::pair<xAOD::PhotonContainer*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*photons);
    copy = shallowcopy.first;
    copyaux = shallowcopy.second;
    bool setLinks = xAOD::setOriginalObjectLink(*photons, *copy);
    if (!setLinks) {
      ATH_MSG_WARNING("Failed to set original object links on " << photonkey);
    }
  } else { // use the user-supplied collection instead 
    ATH_MSG_DEBUG("Not retrieving photon collecton, using existing one provided by user");
    photons=copy;
  }  

  bool cached_doIsoSig = m_doPhIsoSignal;
  for (const auto& photon : *copy) {
    ATH_CHECK( this->FillPhoton(*photon, m_photonBaselinePt, m_photonBaselineEta) );
    if(m_doIsoCloseByOR) //switch off isolation for now if close-by OR corrections were requested
      m_doPhIsoSignal = false;
    this->IsSignalPhoton(*photon, m_photonPt, m_photonEta);
  }

  //apply close-by corrections to isolation if requested
  if(m_doIsoCloseByOR){
    // stores the electrons in a vector
    std::vector<const xAOD::IParticle*> pVec;
    for(auto pobj: *copy) {
      pVec.push_back((const xAOD::IParticle*) pobj);
    }

    //restore isSignal settings
    m_doPhIsoSignal = cached_doIsoSig;

    //correct isolation and propagate to signal deco
    for (const auto& photon : *copy) {
      dec_isol(*photon) = m_isoCloseByTool->acceptCorrected(*photon, pVec);
      if(m_doPhIsoSignal) dec_signal(*photon) &= dec_isol(*photon); //add isolation to signal deco if requested
    }
  }


  if (recordSG) {
    ATH_CHECK( evtStore()->record(copy, "STCalib" + photonkey + m_currentSyst.name()) );
    ATH_CHECK( evtStore()->record(copyaux, "STCalib" + photonkey + m_currentSyst.name() + "Aux.") );
  }
  return StatusCode::SUCCESS;
}


StatusCode SUSYObjDef_xAOD::FillPhoton(xAOD::Photon& input, float ptcut, float etacut) {

  ATH_MSG_VERBOSE( "Starting FillPhoton on ph with pre-calibration pt=" << input.pt() );

  if (m_debug) {
    ATH_MSG_INFO( "PHOTON eta: " << input.eta() );
    ATH_MSG_INFO( "PHOTON phi: " << input.phi() );
    ATH_MSG_INFO( "PHOTON cl eta: " << input.caloCluster()->eta() );
    ATH_MSG_INFO( "PHOTON cl phi: " << input.caloCluster()->phi() );
    ATH_MSG_INFO( "PHOTON cl e: " << input.caloCluster()->e() );
    ATH_MSG_INFO( "PHOTON OQ: " << input.OQ() );
    ATH_MSG_INFO( "PHOTON author: " << input.author() );
  }

  dec_baseline(input) = false;
  dec_selected(input) = 0;
  dec_isol(input) = false;

  // Author cuts needed according to https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaIdentificationRun2#Photon_authors
  if ( !(input.author() & (xAOD::EgammaParameters::AuthorPhoton + xAOD::EgammaParameters::AuthorAmbiguous)) )
    return StatusCode::SUCCESS;

  if (m_egammaCalibTool->applyCorrection(input)  != CP::CorrectionCode::Ok)
    ATH_MSG_ERROR("FillPhoton: EgammaCalibTool applyCorrection failed");

  if (m_isoCorrTool->applyCorrection(input)  != CP::CorrectionCode::Ok)
    ATH_MSG_ERROR("FillPhoton: IsolationCorrectionTool applyCorrection failed");

  ATH_MSG_VERBOSE( "FillPhoton: post-calibration pt=" << input.pt() );

  if (input.pt() < ptcut || fabs(input.caloCluster()->etaBE(2)) >= etacut) return StatusCode::SUCCESS;

  if (m_photonBaselineCrackVeto){
    if  ( fabs( input.caloCluster()->etaBE(2) ) >1.37 &&  fabs( input.caloCluster()->etaBE(2) ) <1.52) {
      return StatusCode::SUCCESS; 
    }
  }

  //Object quality cut as described at https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaIdentificationRun2#Object_quality_cut
  if (!input.isGoodOQ(xAOD::EgammaParameters::BADCLUSPHOTON))
    return StatusCode::SUCCESS;

  //Photon quality as in https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaIdentificationRun2#Photon_cleaning
  if ( (!m_photonAllowLate && !PhotonHelpers::passOQquality(&input)) ||
       ( m_photonAllowLate && !PhotonHelpers::passOQqualityDelayed(&input)) ){
    return StatusCode::SUCCESS;
  }

  if (!isAtlfast() && !isData()) {
    if ( m_electronPhotonShowerShapeFudgeTool->applyCorrection(input) != CP::CorrectionCode::Ok)
      ATH_MSG_ERROR("FillPhoton - fudge tool: applyCorrection failed");
  }

  if (!m_photonSelIsEMBaseline->accept(&input) )return StatusCode::SUCCESS;

  dec_baseline(input) = true;
  dec_selected(input) = 2;
  dec_isol(input) = m_isoTool->accept(input);

  ATH_MSG_VERBOSE("FillPhoton: passed baseline selection.");
  return StatusCode::SUCCESS;
}


bool SUSYObjDef_xAOD::IsSignalPhoton(const xAOD::Photon& input, float ptcut, float etacut) const
{

  dec_signal(input) = false;

  if ( !dec_baseline(input) )  return false;

  if ( !m_egammaAmbiguityTool->accept(input) ) return false;

  if ( input.pt() < ptcut ) return false;
  if ( etacut==DUMMYDEF ){
    if(fabs(input.eta()) > m_photonEta ) return false;
  }
  else if ( fabs(input.eta()) > etacut ) return false;

  if (m_photonCrackVeto){
    if  ( fabs( input.caloCluster()->etaBE(2) ) >1.37 &&  fabs( input.caloCluster()->etaBE(2) ) <1.52) {
      return StatusCode::SUCCESS; 
    }
  }


  if (dec_isol(input) || !m_doPhIsoSignal) {
    ATH_MSG_VERBOSE( "IsSignalPhoton: passed isolation");
  } else return false;

  if (!m_photonSelIsEM->accept(&input) ) return false;

  dec_signal(input) = true;

  return true;
}


double SUSYObjDef_xAOD::GetSignalPhotonSF(const xAOD::Photon& ph, const bool effSF, const bool isoSF) const
{
  double sf(1.);

  if (effSF) {

    double sf_eff = 1.;

    CP::CorrectionCode res = m_photonEfficiencySFTool->getEfficiencyScaleFactor( ph, sf_eff );
    if (res == CP::CorrectionCode::OutOfValidityRange) ATH_MSG_WARNING(" GetSignalPhotonSF: getEfficiencyScaleFactor out of validity range");

    sf *= sf_eff;
  }

  if (isoSF) {

    double sf_iso = 1.;

    CP::CorrectionCode res = m_photonIsolationSFTool->getEfficiencyScaleFactor( ph, sf_iso );
    if (res == CP::CorrectionCode::OutOfValidityRange) ATH_MSG_WARNING(" GetSignalPhotonSF: getEfficiencyScaleFactor out of validity range");

    sf *= sf_iso;
  }

  ATH_MSG_VERBOSE( " ScaleFactor " << sf );

  dec_effscalefact(ph) = sf;
  return sf;
}


double SUSYObjDef_xAOD::GetSignalPhotonSFsys(const xAOD::Photon& ph, const CP::SystematicSet& systConfig, const bool effSF, const bool isoSF)
{
  double sf(1.);

  //Set the new systematic variation
  CP::SystematicCode ret = m_photonEfficiencySFTool->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgPhotonEfficiencyCorrectionTool for systematic var. " << systConfig.name() );
  }

  ret = m_photonIsolationSFTool->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgPhotonEfficiencyCorrectionTool for systematic var. " << systConfig.name() );
  }

  if (effSF) {

    double sf_eff = 1.;

    CP::CorrectionCode res = m_photonEfficiencySFTool->getEfficiencyScaleFactor( ph, sf_eff );
    if (res == CP::CorrectionCode::OutOfValidityRange) ATH_MSG_WARNING(" GetSignalPhotonSF: getEfficiencyScaleFactor out of validity range");

    sf *= sf_eff;
  }

  if (isoSF) {

    double sf_iso = 1.;

    CP::CorrectionCode res = m_photonIsolationSFTool->getEfficiencyScaleFactor( ph, sf_iso );
    if (res == CP::CorrectionCode::OutOfValidityRange) ATH_MSG_WARNING(" GetSignalPhotonSF: getEfficiencyScaleFactor out of validity range");

    sf *= sf_iso;
  }

  //Roll back to current sys
  ret = m_photonEfficiencySFTool->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgPhotonEfficiencyCorrectionTool back to default.");
  }

  ret = m_photonIsolationSFTool->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure AsgPhotonEfficiencyCorrectionTool for systematic var. " << systConfig.name() );
  }

  ATH_MSG_VERBOSE( " ScaleFactor " << sf );

  dec_effscalefact(ph) = sf;
  return sf;
}


double SUSYObjDef_xAOD::GetTotalPhotonSF(const xAOD::PhotonContainer& photons, const bool effSF, const bool isoSF) const
{

  double sf(1.);

  for (const auto& photon : photons) {
    if (dec_signal(*photon) && dec_passOR(*photon)) { sf *= this->GetSignalPhotonSF(*photon, effSF, isoSF); }
  }

  return sf;

}


double SUSYObjDef_xAOD::GetTotalPhotonSFsys(const xAOD::PhotonContainer& photons, const CP::SystematicSet& systConfig, const bool effSF, const bool isoSF)
{

  double sf(1.);

  for (const auto& photon : photons) {
    if (dec_signal(*photon) && dec_passOR(*photon)) { sf *= this->GetSignalPhotonSFsys(*photon, systConfig, effSF, isoSF); }
  }

  return sf;
}

}
