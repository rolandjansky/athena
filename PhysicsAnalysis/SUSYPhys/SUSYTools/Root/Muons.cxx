/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// This source file implements all of the functions related to <OBJECT>
// in the SUSYObjDef_xAOD class

// Local include(s):
#include "SUSYTools/SUSYObjDef_xAOD.h"

#include "xAODBase/IParticleHelpers.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "AthContainers/ConstDataVector.h"

#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"

#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"

#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/replace.hpp>

#ifndef XAOD_STANDALONE // For now metadata is Athena-only
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

namespace ST {

  const static SG::AuxElement::Decorator<char>      dec_passedHighPtCuts("passedHighPtCuts");
  const static SG::AuxElement::ConstAccessor<char>  acc_passedHighPtCuts("passedHighPtCuts");

  const static SG::AuxElement::Decorator<char>      dec_passSignalID("passSignalID");
  const static SG::AuxElement::ConstAccessor<char>  acc_passSignalID("passSignalID");

  const static SG::AuxElement::Decorator<float>     dec_z0sinTheta("z0sinTheta");
  const static SG::AuxElement::ConstAccessor<float> acc_z0sinTheta("z0sinTheta");
  const static SG::AuxElement::Decorator<float>     dec_d0sig("d0sig");
  const static SG::AuxElement::ConstAccessor<float> acc_d0sig("d0sig");

StatusCode SUSYObjDef_xAOD::GetMuons(xAOD::MuonContainer*& copy, xAOD::ShallowAuxContainer*& copyaux, bool recordSG, const std::string& muonkey, const xAOD::MuonContainer* containerToBeCopied)
{
  if (!m_tool_init) {
    ATH_MSG_ERROR("SUSYTools was not initialized!!");
    return StatusCode::FAILURE;
  }
  
  const xAOD::MuonContainer* muons(0);
  if (copy==NULL) { // empty container provided
    if (containerToBeCopied != nullptr) {
      muons = containerToBeCopied;
    }
    else {
      ATH_CHECK( evtStore()->retrieve(muons, muonkey) );
    }
    std::pair<xAOD::MuonContainer*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*muons);
    copy = shallowcopy.first;
    copyaux = shallowcopy.second;
    bool setLinks = xAOD::setOriginalObjectLink(*muons, *copy);
    if (!setLinks) {
      ATH_MSG_WARNING("Failed to set original object links on " << muonkey);
    }
  } else { // use the user-supplied collection instead 
    ATH_MSG_DEBUG("Not retrieving muon collecton, using existing one provided by user");
    muons=copy;
  }

  for (const auto& muon : *copy) {
    ATH_CHECK( this->FillMuon(*muon, m_muBaselinePt, m_muBaselineEta) );
    this->IsSignalMuon(*muon, m_muPt, m_mud0sig, m_muz0, m_muEta);
    this->IsCosmicMuon(*muon, m_muCosmicz0, m_muCosmicd0);
    this->IsBadMuon(*muon, m_badmuQoverP);
  }
  if (recordSG) {
    ATH_CHECK( evtStore()->record(copy, "STCalib" + muonkey + m_currentSyst.name()) );
    ATH_CHECK( evtStore()->record(copyaux, "STCalib" + muonkey + m_currentSyst.name() + "Aux.") );
  }
  return StatusCode::SUCCESS;
}

StatusCode SUSYObjDef_xAOD::FillMuon(xAOD::Muon& input, float ptcut, float etacut) {
  
  ATH_MSG_VERBOSE( "Starting FillMuon on mu with pt=" << input.pt() );
  
  dec_baseline(input) = false;
  dec_selected(input) = 0;
  dec_signal(input) = false;
  dec_isol(input) = false;
  dec_passedHighPtCuts(input) = false;
  dec_passSignalID(input) = false;
       
  // don't bother calibrating or computing WP
  if ( input.pt() < 3e3 ) return StatusCode::SUCCESS;

  ATH_MSG_VERBOSE( "MUON pt before calibration " << input.pt() );

  ATH_MSG_VERBOSE( "MUON eta  = " << input.eta() );
  ATH_MSG_VERBOSE( "MUON type = " << input.muonType() );
  ATH_MSG_VERBOSE( "MUON author = " << input.author() );
  
  if (m_muonCalibrationAndSmearingTool->applyCorrection( input ) == CP::CorrectionCode::OutOfValidityRange)
    ATH_MSG_VERBOSE("FillMuon: applyCorrection out of validity range");
  
  ATH_MSG_VERBOSE( "MUON pt after calibration " << input.pt() );

  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );

  const xAOD::Vertex* pv = this->GetPrimVtx();
  double primvertex_z = pv ? pv->z() : 0;
  //const xAOD::TrackParticle* track = input.primaryTrackParticle();
  const xAOD::TrackParticle* track;
  if (input.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon) {
    track = input.trackParticle(xAOD::Muon::CombinedTrackParticle);
    if (!track) return StatusCode::SUCCESS; // don't treat SAF muons without CB track further  
  }
  else {
    track = input.primaryTrackParticle();
  }

  //impact parameters (after applyCorrection() so to have the primaryTrack links restored in old buggy samples)
  dec_z0sinTheta(input) = (track->z0() + track->vz() - primvertex_z) * TMath::Sin(input.p4().Theta());
  //protect against exception thrown for null or negative d0sig
  try {
    dec_d0sig(input) = xAOD::TrackingHelpers::d0significance( track , evtInfo->beamPosSigmaX(), evtInfo->beamPosSigmaY(), evtInfo->beamPosSigmaXY() );
  }
  catch (...) {
    float d0sigError = -99.; 
    ATH_MSG_WARNING("FillMuon : Exception catched from d0significance() calculation. Setting dummy decoration d0sig=" << d0sigError );
    dec_d0sig(input) = d0sigError;
  }
  
  if (m_debug) {
    // Summary variables in
    // /cvmfs/atlas.cern.ch/repo/sw/ASG/AnalysisBase/2.0.3/xAODTracking/Root/TrackSummaryAccessors_v1.cxx
    
    unsigned char nBLHits(0), nPixHits(0), nPixelDeadSensors(0), nPixHoles(0),
      nSCTHits(0), nSCTDeadSensors(0), nSCTHoles(0), nTRTHits(0), nTRTOutliers(0);
    
    const xAOD::TrackParticle* track =  input.primaryTrackParticle();
    track->summaryValue( nBLHits, xAOD::numberOfBLayerHits);
    track->summaryValue( nPixHits, xAOD::numberOfPixelHits);
    track->summaryValue( nPixelDeadSensors, xAOD::numberOfPixelDeadSensors);
    track->summaryValue( nPixHoles, xAOD::numberOfPixelHoles);
    
    track->summaryValue( nSCTHits, xAOD::numberOfSCTHits);
    track->summaryValue( nSCTDeadSensors, xAOD::numberOfSCTDeadSensors);
    track->summaryValue( nSCTHoles, xAOD::numberOfSCTHoles);
    
    track->summaryValue( nTRTHits, xAOD::numberOfTRTHits);
    track->summaryValue( nTRTOutliers, xAOD::numberOfTRTOutliers);
    
    ATH_MSG_INFO( "MUON pt: " << input.pt() );
    ATH_MSG_INFO( "MUON eta: " << input.eta() );
    ATH_MSG_INFO( "MUON phi: " << input.phi() );
    ATH_MSG_INFO( "MUON comb: " << (int)(input.muonType() == xAOD::Muon::Combined) );
    ATH_MSG_INFO( "MUON sTag: " << (int)(input.muonType() == xAOD::Muon::SegmentTagged));
    ATH_MSG_INFO( "MUON loose: " << (int)(input.quality() == xAOD::Muon::Loose ) );
    ATH_MSG_INFO( "MUON bHit: " << (int) nBLHits );
    ATH_MSG_INFO( "MUON pHit: " << (int) nPixHits );
    ATH_MSG_INFO( "MUON pDead: " << (int) nPixelDeadSensors );
    ATH_MSG_INFO( "MUON pHole: " << (int) nPixHoles );
    ATH_MSG_INFO( "MUON sHit: "  << (int) nSCTHits);
    ATH_MSG_INFO( "MUON sDead: " << (int) nSCTDeadSensors );
    ATH_MSG_INFO( "MUON sHole: " << (int) nSCTHoles );
    ATH_MSG_INFO( "MUON tHit: "  << (int) nTRTHits );
    ATH_MSG_INFO( "MUON tOut: "  << (int) nTRTOutliers );

    const xAOD::TrackParticle* idtrack =
      input.trackParticle( xAOD::Muon::InnerDetectorTrackParticle );

    if ( !idtrack) {
      ATH_MSG_VERBOSE( "No ID track!! " );
    } else {
      ATH_MSG_VERBOSE( "ID track pt: "  << idtrack->pt() );
    }
  }
  
  if ( !m_force_noMuId && !m_muonSelectionToolBaseline->accept(input)) return StatusCode::SUCCESS;
  
  if (input.pt() <= ptcut || fabs(input.eta()) >= etacut) return StatusCode::SUCCESS;

  if (m_mubaselinez0>0. && fabs(acc_z0sinTheta(input))>m_mubaselinez0) return StatusCode::SUCCESS;
  if (m_mubaselined0sig>0. && fabs(acc_d0sig(input))>m_mubaselined0sig) return StatusCode::SUCCESS;

  //--- Do baseline isolation check
  if ( !( m_muBaselineIso_WP.empty() ) &&  !( m_isoBaselineTool->accept(input) ) ) return StatusCode::SUCCESS;

  dec_baseline(input) = true;
  dec_selected(input) = 2;

  if (!m_muIso_WP.empty()) dec_isol(input) = m_isoTool->accept(input);
  dec_passSignalID(input) = m_muonSelectionTool->accept(input);
  
  ATH_MSG_VERBOSE("FillMuon: passed baseline selection");
  return StatusCode::SUCCESS;
}
  

bool SUSYObjDef_xAOD::IsSignalMuon(const xAOD::Muon & input, float ptcut, float d0sigcut, float z0cut, float etacut) const
{

  if (!acc_baseline(input)) return false;
  if (!acc_passSignalID(input)) return false;

  if (input.pt() <= ptcut || input.pt() == 0) return false; // pT cut (might be necessary for leading muon to pass trigger)
  if ( etacut==DUMMYDEF ){
    if(fabs(input.eta()) > m_muEta ) return false;
  }
  else if ( fabs(input.eta()) > etacut ) return false;

  if (z0cut > 0.0 && fabs(acc_z0sinTheta(input)) > z0cut) return false; // longitudinal IP cut
  if (dec_d0sig(input) != 0) {
    if (d0sigcut > 0.0 && fabs(acc_d0sig(input)) > d0sigcut) return false; // transverse IP cut
  }

  if ( (!m_doMuIsoSignal) || acc_isol(input)) {
    ATH_MSG_VERBOSE( "IsSignalMuon: passed isolation");
  } else return false; //isolation selection with IsoTool

  //set HighPtMuon decoration
  IsHighPtMuon(input);

  dec_signal(input) = true;

  if (m_muId == 4) { //i.e. HighPt muons
    ATH_MSG_VERBOSE( "IsSignalMuon: mu pt " << input.pt()
                     << " signal? " << (int) acc_signal(input)
                     << " isolation? " << (int) acc_isol(input)
                     << " passedHighPtCuts? " << (int) acc_passedHighPtCuts(input));
  } else {
    ATH_MSG_VERBOSE( "IsSignalMuon: mu pt " << input.pt()
                     << " signal? " << (int) acc_signal(input)
                     << " isolation? " << (int) acc_isol(input));
    // Don't show HighPtFlag ... we didn't set it!
  }

  return acc_signal(input);
}


bool SUSYObjDef_xAOD::IsHighPtMuon(const xAOD::Muon& input) const
// See https://indico.cern.ch/event/371499/contribution/1/material/slides/0.pdf and 
//     https://indico.cern.ch/event/397325/contribution/19/material/slides/0.pdf and 
//     https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool
{
  if (input.pt() < 3e3){
    ATH_MSG_DEBUG("No HighPt check supported for muons below 3GeV! False.");
    dec_passedHighPtCuts(input) = false;
    return false;
  }

  bool isHighPt=false;
  isHighPt = m_muonSelectionHighPtTool->accept(input);
  dec_passedHighPtCuts(input) = isHighPt;

  return isHighPt;
}


bool SUSYObjDef_xAOD::IsBadMuon(const xAOD::Muon& input, float qopcut) const
{
  const static SG::AuxElement::Decorator<char> dec_bad("bad");
  dec_bad(input) = false;

  const static SG::AuxElement::Decorator<char> dec_bad_highPt("bad_highPt");
  dec_bad_highPt(input) = false;

  const xAOD::TrackParticle* track;
  if (input.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon) {
    track = input.trackParticle(xAOD::Muon::CombinedTrackParticle);
    if (!track) return StatusCode::SUCCESS; // don't treat SAF muons without CB track further  
  }
  else{
    track = input.primaryTrackParticle();
  }

  float Rerr = Amg::error(track->definingParametersCovMatrix(), 4) / fabs(track->qOverP());
  ATH_MSG_VERBOSE( "Track momentum error (%): " << Rerr * 100 );
  bool isbad = Rerr > qopcut;
  bool isbadHighPt = Rerr > qopcut;

  //new recommendation from MCP
  isbad |= m_muonSelectionTool->isBadMuon(input);

  //new recommendation from MCP (at HighPT)
  isbadHighPt |= m_muonSelectionHighPtTool->isBadMuon(input);

  dec_bad(input) = isbad;
  dec_bad_highPt(input) = isbadHighPt;

  ATH_MSG_VERBOSE( "MUON isbad?: " << isbad );
  return isbad;
}

bool SUSYObjDef_xAOD::IsCosmicMuon(const xAOD::Muon& input, float z0cut, float d0cut) const
{
  const static SG::AuxElement::Decorator<char> dec_cosmic("cosmic");
  dec_cosmic(input) = false;

  const xAOD::TrackParticle* track;
  if (input.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon) {
    track = input.trackParticle(xAOD::Muon::CombinedTrackParticle);
    if (!track){
      ATH_MSG_VERBOSE("WARNING: SAF muon without CB track found. Not possible to check cosmic muon criteria");
      return false; // don't treat SAF muons without CB track further  
    }
  }
  else {
    track = input.primaryTrackParticle();
  }

  double mu_d0 = track->d0();
  const xAOD::Vertex* pv = this->GetPrimVtx();
  double primvertex_z = pv ? pv->z() : 0;
  double mu_z0_exPV = track->z0() + track->vz() - primvertex_z;
  
  bool isCosmicMuon = (fabs(mu_z0_exPV) >= z0cut || fabs(mu_d0) >= d0cut);

  if (isCosmicMuon) {
    ATH_MSG_VERBOSE("COSMIC PV Z = " << primvertex_z << ", track z0 = " << mu_z0_exPV << ", track d0 = " << mu_d0);
  }

  dec_cosmic(input) = isCosmicMuon;
  return isCosmicMuon;
}


  float SUSYObjDef_xAOD::GetSignalMuonSF(const xAOD::Muon& mu, const bool recoSF, const bool isoSF, const bool doBadMuonHP, const bool warnOVR)
{
  float sf(1.);

  if (recoSF) {
    float sf_reco(1.);
    if (m_muonEfficiencySFTool->getEfficiencyScaleFactor( mu, sf_reco ) == CP::CorrectionCode::OutOfValidityRange) {
      if(warnOVR) ATH_MSG_WARNING(" GetSignalMuonSF: Reco getEfficiencyScaleFactor out of validity range");
    }
    ATH_MSG_VERBOSE( " MuonReco ScaleFactor " << sf_reco );
    sf *= sf_reco;

    float sf_ttva(1.);
    if(m_doTTVAsf){
      if (m_muonTTVAEfficiencySFTool->getEfficiencyScaleFactor( mu, sf_ttva ) == CP::CorrectionCode::OutOfValidityRange) {
	if(warnOVR) ATH_MSG_WARNING(" GetSignalMuonSF: TTVA getEfficiencyScaleFactor out of validity range");
      }
      ATH_MSG_VERBOSE( " MuonTTVA ScaleFactor " << sf_ttva );
      sf *= sf_ttva;
    }

    float sf_badHighPt(1.);
    if(m_muId == 4 && doBadMuonHP){
      if (m_muonEfficiencyBMHighPtSFTool->getEfficiencyScaleFactor( mu, sf_badHighPt ) == CP::CorrectionCode::OutOfValidityRange) {
	if(warnOVR) ATH_MSG_WARNING(" GetSignalMuonSF: BadMuonHighPt getEfficiencyScaleFactor out of validity range");
      }
      ATH_MSG_VERBOSE( " MuonTTVA ScaleFactor " << sf_badHighPt );
      sf *= sf_badHighPt;
    }
  }

  if (isoSF) {
    float sf_iso(1.);
    if ( mu.pt() < 4e3 ) {
      ATH_MSG_WARNING( "Muon pt: " << mu.pt()/1000. << "! Isolation SF is not available for Muon pt < 4GeV. Falling back to SF = 1." );
      return 1;
    } 
    if (m_muonIsolationSFTool.empty()) {
      ATH_MSG_WARNING(" GetSignalMuonSF: Attempt to retrieve isolation SF for unsupported working point " << m_muIso_WP);
    } else {
      if (m_muonIsolationSFTool->getEfficiencyScaleFactor( mu, sf_iso ) == CP::CorrectionCode::OutOfValidityRange) {
        if(warnOVR) ATH_MSG_WARNING(" GetSignalMuonSF: Iso getEfficiencyScaleFactor out of validity range");
      }
      ATH_MSG_VERBOSE( " MuonIso ScaleFactor " << sf_iso );
      sf *= sf_iso;
    }
  }

  dec_effscalefact(mu) = sf;
  return sf;
}


double SUSYObjDef_xAOD::GetMuonTriggerEfficiency(const xAOD::Muon& mu, const std::string& trigExpr, const bool isdata) {

  double eff(1.);

  if ( mu.pt() < 4e3 ) {
    ATH_MSG_WARNING( "Muon pt: " << mu.pt()/1000. << "! Trigger Eff is not available for Muon pt < 4GeV. Falling back to Eff = 1." );
    return 1;
  } 
  if (m_muonTriggerSFTool->getTriggerEfficiency(mu, eff, trigExpr, isdata) != CP::CorrectionCode::Ok) {
    ATH_MSG_WARNING("Problem retrieving signal muon trigger efficiency for " << trigExpr );
  }
  else{
    ATH_MSG_DEBUG("Got efficiency " << eff << " for " << trigExpr );
  }
  return eff;
}


double SUSYObjDef_xAOD::GetTotalMuonTriggerSF(const xAOD::MuonContainer& sfmuons, const std::string& trigExpr) {

  if (trigExpr.empty() || sfmuons.size()==0) return 1.;

  double trig_sf = 1.;

  int mulegs = 0;
  const char *tmp = trigExpr.c_str();
  while( (tmp = strstr(tmp, "mu")) ){
    mulegs++;
    tmp++;
  }

  bool isdimuon = (trigExpr.find("2mu") != std::string::npos);
  bool isOR = (trigExpr.find("OR") != std::string::npos);
  
  if((!isdimuon && mulegs<2) || (isdimuon && sfmuons.size()==2) || (mulegs>=2 && isOR)){   //Case 1: the tool takes easy care of the single, standard-dimuon and OR-of-single chains
    if (m_muonTriggerSFTool->getTriggerScaleFactor( sfmuons, trig_sf, trigExpr ) == CP::CorrectionCode::Ok) {
      ATH_MSG_DEBUG( " MuonTrig ScaleFactor " << trig_sf );
    }
    else{
      ATH_MSG_DEBUG( " MuonTrig FAILED SOMEHOW");
    }
  }
  else if(mulegs!=2 && isOR){ //Case 2: not supported. Not efficiency defined for (at least) one leg. Sorry...
    ATH_MSG_WARNING( " SF for " << trigExpr << " are only supported for two muon events!");
  }
  else{ //Case 3: let's go the hard way...
        //Following https://twiki.cern.ch/twiki/bin/view/Atlas/TrigMuonEfficiency

    std::string newtrigExpr = TString(trigExpr).Copy().ReplaceAll("HLT_2","").Data();

    //redefine dimuon triggers here (2mu14 --> mu14_mu14)
    if (isdimuon) { newtrigExpr += "_"+newtrigExpr;  }
    boost::replace_all(newtrigExpr, "HLT_", "");
    boost::char_separator<char> sep("_");
    for (const auto& mutrig : boost::tokenizer<boost::char_separator<char>>(newtrigExpr, sep)) {
      double dataFactor = 1.;
      double mcFactor   = 1.;
      
      for (const auto& mu : sfmuons) {
        // No need for additional trigger matching
        dataFactor *= (1 - GetMuonTriggerEfficiency(*mu, "HLT_"+mutrig, true));
        mcFactor   *= (1 - GetMuonTriggerEfficiency(*mu, "HLT_"+mutrig, false));
      }
      if( (1-mcFactor) > 0. )
        trig_sf *= (1-dataFactor)/(1-mcFactor);
    }
  }

  return trig_sf;

}


  double SUSYObjDef_xAOD::GetTotalMuonSF(const xAOD::MuonContainer& muons, const bool recoSF, const bool isoSF, const std::string& trigExpr, const bool bmhptSF) {
  double sf(1.);

  ConstDataVector<xAOD::MuonContainer> sfmuons(SG::VIEW_ELEMENTS);
  for (const auto& muon : muons) {
    if( !acc_passOR(*muon) ) continue;
    if (acc_signal(*muon)) {
      sfmuons.push_back(muon);
      if (recoSF || isoSF) { sf *= this->GetSignalMuonSF(*muon, recoSF, isoSF, bmhptSF); }
    } else { // decorate baseline muons as well
      if (recoSF || isoSF) { this->GetSignalMuonSF(*muon, recoSF, isoSF, bmhptSF, false); } //avoid OVR warnings in this case
    }
  }

  sf *= GetTotalMuonTriggerSF(*sfmuons.asDataVector(), trigExpr);
  
  return sf;
}


  double SUSYObjDef_xAOD::GetTotalMuonSFsys(const xAOD::MuonContainer& muons, const CP::SystematicSet& systConfig, const bool recoSF, const bool isoSF, const std::string& trigExpr, const bool bmhptSF) {

  double sf(1.);
  //Set the new systematic variation
  CP::SystematicCode ret = m_muonEfficiencySFTool->applySystematicVariation(systConfig);
  if ( ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure MuonEfficiencyScaleFactors for systematic var. " << systConfig.name() );
  }

  ret = m_muonEfficiencyBMHighPtSFTool->applySystematicVariation(systConfig);
  if ( ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure MuonBadMuonHighPtScaleFactors for systematic var. " << systConfig.name() );
  }

  ret = m_muonTTVAEfficiencySFTool->applySystematicVariation(systConfig);
  if ( ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure MuonTTVAEfficiencyScaleFactors for systematic var. " << systConfig.name() );
  }

  ret  = m_muonIsolationSFTool->applySystematicVariation(systConfig);
  if ( ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure MuonIsolationScaleFactors for systematic var. " << systConfig.name() );
  }

  ret  = m_muonTriggerSFTool->applySystematicVariation(systConfig);
  if ( ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure MuonTriggerScaleFactors for systematic var. " << systConfig.name() );
  }

  ret = m_trigGlobalEffCorrTool_diLep->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure TrigGlobalEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
  }

  ret = m_trigGlobalEffCorrTool_multiLep->applySystematicVariation(systConfig);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure TrigGlobalEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
  }

  sf = GetTotalMuonSF(muons, recoSF, isoSF, trigExpr, bmhptSF);

  //Roll back to default
  ret  = m_muonEfficiencySFTool->applySystematicVariation(m_currentSyst);
  if ( ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure MuonEfficiencyScaleFactors back to default.");
  }

  ret = m_muonEfficiencyBMHighPtSFTool->applySystematicVariation(m_currentSyst);
  if ( ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure MuonBadMuonHighPtScaleFactors back to default.");
  }

  ret  = m_muonTTVAEfficiencySFTool->applySystematicVariation(m_currentSyst);
  if ( ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure MuonTTVAEfficiencyScaleFactors back to default.");
  }

  ret  = m_muonIsolationSFTool->applySystematicVariation(m_currentSyst);
  if ( ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure MuonIsolationScaleFactors back to default.");
  }

  ret  = m_muonTriggerSFTool->applySystematicVariation(m_currentSyst);
  if ( ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure MuonTriggerScaleFactors back to default.");
  }

  ret = m_trigGlobalEffCorrTool_diLep->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure TrigGlobalEfficiencyCorrectionTool (trigger) back to default.");
  }

  ret = m_trigGlobalEffCorrTool_multiLep->applySystematicVariation(m_currentSyst);
  if (ret != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Cannot configure TrigGlobalEfficiencyCorrectionTool (trigger) back to default.");
  }

  return sf;
}


}
