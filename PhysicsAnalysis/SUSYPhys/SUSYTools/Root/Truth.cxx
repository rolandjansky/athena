/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This source file implements all of the functions related to <OBJECT>
// in the SUSYObjDef_xAOD class

// Local include(s):
#include "SUSYTools/SUSYObjDef_xAOD.h"

// For using the MCTruthClassifier definitions
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

#ifndef XAOD_STANDALONE // For now metadata is Athena-only
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

namespace ST {


bool SUSYObjDef_xAOD :: isPrompt(const xAOD::IParticle* part) const {

  const int type  = acc_truthType(*part);
  int origin      = acc_truthOrigin(*part);
  int originbkg   = 0;
  if(part->isAvailable<int>("bkgTruthOrigin")){ //only in SUSY2,3,5 for the moment!
    originbkg = acc_bkgTruthOrigin(*part);
  }
  else{
    ATH_MSG_DEBUG("::isPrompt() : No bkgTruthOrigin decoration available. Returning false by default.");
    return false;
  }

  //Electrons 
  if( part->type() == xAOD::Type::Electron ){ 
    switch(type){
    case MCTruthPartClassifier::BkgElectron:
      if(origin != MCTruthPartClassifier::PhotonConv) return false;
      if(originbkg == MCTruthPartClassifier::FSRPhot || originbkg == MCTruthPartClassifier::BremPhot) return true;
      origin = originbkg;
      // [[fallthrough]]
    case MCTruthPartClassifier::IsoElectron:
      return (origin == MCTruthPartClassifier::top
	      || origin == MCTruthPartClassifier::WBoson
	      || origin == MCTruthPartClassifier::ZBoson
	      || origin == MCTruthPartClassifier::Higgs
	      || origin == MCTruthPartClassifier::HiggsMSSM
	      || origin == MCTruthPartClassifier::HeavyBoson
	      || origin == MCTruthPartClassifier::WBosonLRSM
	      || origin == MCTruthPartClassifier::NuREle
	      || origin == MCTruthPartClassifier::NuRMu
	      || origin == MCTruthPartClassifier::NuRTau
	      || origin == MCTruthPartClassifier::LQ
	      || origin == MCTruthPartClassifier::SUSY);
    default:
      return false;
    }
    // NB: will classify a few muon/quark brems as prompt electrons, but a small effect
  }
  if( part->type() == xAOD::Type::Muon ){ 
    return (type == MCTruthPartClassifier::IsoMuon);

  }
  else{
    ATH_MSG_DEBUG("::isPrompt() : Only Electrons supported at the moment!");
  }

  //Photons ... ?
  //Taus ... ?
  return false;
}


StatusCode SUSYObjDef_xAOD::FindSusyHP(int& pdgid1, int& pdgid2) const {

  const xAOD::TruthParticleContainer    *truthP = 0;
  bool isTruth3=false;
  std::string key_T1 = "TruthParticles"; 
  std::string key_T3 = "TruthBSM";
 
 
  if(evtStore()->contains<xAOD::TruthParticleContainer>( key_T1 )){
    ATH_CHECK( evtStore()->retrieve(truthP, key_T1) );
  }
  else if(evtStore()->contains<xAOD::TruthParticleContainer>( key_T3 )){
    isTruth3=true;    
    ATH_CHECK( evtStore()->retrieve(truthP, key_T3) );
  }

  return SUSYObjDef_xAOD::FindSusyHP(truthP, pdgid1, pdgid2, isTruth3);
}


StatusCode SUSYObjDef_xAOD::FindSusyHP(const xAOD::TruthParticleContainer *truthP, int& pdgid1, int& pdgid2, bool isTruth3) const {
  if (!truthP) {
    ATH_MSG_ERROR("Null TruthParticleContainer pointer!!");
    return StatusCode::FAILURE;
  }
  if (truthP->empty()) {
    ATH_MSG_ERROR("Empty TruthParticleContainer!!");
    return StatusCode::FAILURE;
  }
  if ( !SUSYObjDef_xAOD::FindSusyHardProc(truthP, pdgid1, pdgid2, isTruth3) ) {
    ATH_MSG_ERROR("Problem finding SUSY hard process");
    return StatusCode::FAILURE;
  }
  if (pdgid1 == 0 || pdgid2 == 0) {
    ATH_MSG_DEBUG("No sparticles found!");
  }
  return StatusCode::SUCCESS;
}


bool SUSYObjDef_xAOD::FindSusyHardProc(const xAOD::TruthParticleContainer *truthP, int& pdgid1, int& pdgid2, bool isTruth3) {

  pdgid1 = 0;
  pdgid2 = 0;

  //check for TRUTH3 structure first
  if(isTruth3){
    if(!truthP || truthP->size()<2){
      return false;
    }
    
    //get ID of first two BSM particles
    pdgid1 = (*truthP)[0]->pdgId();
    pdgid2 = (*truthP)[1]->pdgId();
    return true;
  }

  //go for TRUTH1-like if not...
  const xAOD::TruthParticle* firstsp(0);
  const xAOD::TruthParticle* secondsp(0);

  if (!truthP || truthP->empty()) {
    return false;
  }
  for (const auto& tp : *truthP) {

    //check ifSUSY particle
    if ((abs(tp->pdgId()) > 1000000 && abs(tp->pdgId()) < 1000007) || // squarkL
        (abs(tp->pdgId()) > 1000010 && abs(tp->pdgId()) < 1000017) || // sleptonL
        (abs(tp->pdgId()) > 2000000 && abs(tp->pdgId()) < 2000007) || // squarkR
        (abs(tp->pdgId()) > 2000010 && abs(tp->pdgId()) < 2000017) || // sleptonR
        (abs(tp->pdgId()) > 1000020 && abs(tp->pdgId()) < 1000040)) { // gauginos

      if (tp->nParents() != 0) {
        if ( tp->parent(0)->absPdgId()  < 1000000) {
          if (!firstsp) {
            firstsp = tp;
          } else if (!secondsp) {
            secondsp = tp;
          } else {
            if (firstsp->nChildren() != 0 && tp->barcode() == firstsp->child(0)->barcode()) {
              firstsp = tp;
            }
            else if (secondsp->nChildren() != 0 && tp->barcode() == secondsp->child(0)->barcode()) {
              secondsp = tp;
            }
            else if (firstsp->nChildren() != 0 && firstsp->child(0)->barcode() == secondsp->barcode()) {
              firstsp = secondsp;
              secondsp = tp;
            }
            else if (secondsp->nChildren() != 0 && secondsp->child(0)->barcode() == firstsp->barcode()) {
              secondsp = firstsp;
              firstsp = tp;
            }
          }
        }
      }
    }
  }

  // quit if no sparticles found
  if (!firstsp && !secondsp) return true; // should find none or two

  if (firstsp->nChildren() == 1) {
    for (const auto& tp : *truthP) {
      if (tp->barcode() == firstsp->child(0)->barcode() && tp->pdgId() != firstsp->pdgId()) {
        firstsp = tp;
        break;
      }
    }
  }

  if (secondsp->nChildren() == 1) {
    for (const auto& tp : *truthP) {
      if (tp->barcode() == secondsp->child(0)->barcode() && tp->pdgId() != secondsp->pdgId()) {
        secondsp = tp;
        break;
      }
    }
  }

  if (abs(firstsp->pdgId()) > 1000000) pdgid1 = firstsp->pdgId();
  if (abs(secondsp->pdgId()) > 1000000) pdgid2 = secondsp->pdgId();

  // Return gracefully:
  return true;
}


StatusCode SUSYObjDef_xAOD::FindSusyHP(const xAOD::TruthEvent *truthE, int& pdgid1, int& pdgid2) const {

  if (!truthE) {
    ATH_MSG_ERROR("Null TruthEvent pointer!!");
    return StatusCode::FAILURE;
  }
  if ( !SUSYObjDef_xAOD::FindSusyHardProc(truthE, pdgid1, pdgid2) ) {
    ATH_MSG_ERROR("Problem finding SUSY hard process");
    return StatusCode::FAILURE;
  }
  if (pdgid1 == 0 || pdgid2 == 0) {
    ATH_MSG_DEBUG("No sparticles found!");
  }
  return StatusCode::SUCCESS;
}


bool SUSYObjDef_xAOD::FindSusyHardProc(const xAOD::TruthEvent *truthE, int& pdgid1, int& pdgid2) {

  pdgid1 = 0;
  pdgid2 = 0;

  //go for TRUTH1-like ...
  const xAOD::TruthParticle* firstsp(0);
  const xAOD::TruthParticle* secondsp(0);

  for (unsigned int p=0; p < truthE->nTruthParticles(); ++p){

    const xAOD::TruthParticle* tp = truthE->truthParticle(p);

    //check ifSUSY particle
    if ((abs(tp->pdgId()) > 1000000 && abs(tp->pdgId()) < 1000007) || // squarkL
        (abs(tp->pdgId()) > 1000010 && abs(tp->pdgId()) < 1000017) || // sleptonL
        (abs(tp->pdgId()) > 2000000 && abs(tp->pdgId()) < 2000007) || // squarkR
        (abs(tp->pdgId()) > 2000010 && abs(tp->pdgId()) < 2000017) || // sleptonR
        (abs(tp->pdgId()) > 1000020 && abs(tp->pdgId()) < 1000040)) { // gauginos

      if (tp->nParents() != 0) {
        if ( tp->parent(0)->absPdgId()  < 1000000) {
          if (!firstsp) {
            firstsp = tp;
          } else if (!secondsp) {
            secondsp = tp;
          } else {
            if (firstsp->nChildren() != 0 && tp->barcode() == firstsp->child(0)->barcode()) {
              firstsp = tp;
            }
            else if (secondsp->nChildren() != 0 && tp->barcode() == secondsp->child(0)->barcode()) {
              secondsp = tp;
            }
            else if (firstsp->nChildren() != 0 && firstsp->child(0)->barcode() == secondsp->barcode()) {
              firstsp = secondsp;
              secondsp = tp;
            }
            else if (secondsp->nChildren() != 0 && secondsp->child(0)->barcode() == firstsp->barcode()) {
              secondsp = firstsp;
              firstsp = tp;
            }
          }
        }
      }
    }
  }

  // quit if no sparticles found
  if (!firstsp && !secondsp) return true; // should find none or two
  
  if (firstsp->nChildren() == 1) {
    for (unsigned int p=0; p < truthE->nTruthParticles(); ++p){
      const xAOD::TruthParticle* tp = truthE->truthParticle(p);
      if (tp->barcode() == firstsp->child(0)->barcode() && tp->pdgId() != firstsp->pdgId()) {
        firstsp = tp;
        break;
      }
    }
  }

  if (secondsp->nChildren() == 1) {
    for (unsigned int p=0; p < truthE->nTruthParticles(); ++p){
      const xAOD::TruthParticle* tp = truthE->truthParticle(p);
      if (tp->barcode() == secondsp->child(0)->barcode() && tp->pdgId() != secondsp->pdgId()) {
        secondsp = tp;
        break;
      }
    }
  }

  if (abs(firstsp->pdgId()) > 1000000) pdgid1 = firstsp->pdgId();
  if (abs(secondsp->pdgId()) > 1000000) pdgid2 = secondsp->pdgId();

  // Return gracefully:
  return true;
}


bool SUSYObjDef_xAOD::IsTruthBJet(const xAOD::Jet& input) const {
  //Method to set correctly the IsBjet decoration needed by the JetUncertainties tool
  bool isBjet = false;
  if (acc_signal(input)) {

    int truthlabel(-1);
    if (!input.getAttribute("HadronConeExclTruthLabelID", truthlabel)) {
      ATH_MSG_ERROR("Failed to get jet truth label!");
    }

    isBjet = std::abs(truthlabel) == 5;
    const static SG::AuxElement::Decorator<char> dec_bjet_jetunc("bjet_jetunc"); //added for JetUncertainties usage
    dec_bjet_jetunc(input) = isBjet;

  }
  return isBjet;
}

}
