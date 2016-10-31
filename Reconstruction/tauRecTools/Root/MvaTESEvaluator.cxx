/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/HelperFunctions.h"

#include "TFile.h"
#include "TTree.h"

#include <vector>

// tools include(s) 

//#include "TauAnalysisTools/HelperFunctions.h"

//_____________________________________________________________________________
MvaTESEvaluator::MvaTESEvaluator(const std::string& name)
  : TauRecToolBase(name)
  , reader(0)
  , mu(0)
  , nVtx(0)    
  , center_lambda(0)
  , first_eng_dens(0)
  , second_lambda(0)
  , presampler_frac(0)
  , em_probability(0)    
  , interpolPt(0)
  , LC_D_interpolPt(0)
  , pantau_D_interpolPt(0)
  , seedCalo_eta(0)    
  , nTracks(0)
  , nPi0PFOs(0)
  , PFOEngRelDiff(0)    
  , nMuSeg(0)    
  , truthPtVis(0)
{
  declareProperty( "WeightFileName", m_sWeightFileName = "LC.pantau.interpolPt250GeV_mediumTaus_BDTG.weights.root" );
  //BDT trained on medium taus: "LC.pantau.interpolPt250GeV_mediumTaus_BDTG.weights.xml"
  //BDT trained on all taus:    "LC.pantau.interpolPt250GeV_allTaus_BDTG.weights.xml"
}

//_____________________________________________________________________________
MvaTESEvaluator::~MvaTESEvaluator()
{
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::initialize(){

  // Declare input variables to the reader
  m_availableVars.insert( std::make_pair("EventInfoAux.averageInteractionsPerCrossing", &mu) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.nVtx", &nVtx) );
  
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.center_lambda", &center_lambda) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.first_eng_dens", &first_eng_dens) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.second_lambda", &second_lambda) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.presampler_frac", &presampler_frac) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.em_probability", &em_probability) );
  
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.LC_pantau_interpolPt", &interpolPt) );
  m_availableVars.insert( std::make_pair("TauJetsAux.LC_TES_precalib/TauJetsAuxDyn.LC_pantau_interpolPt", &LC_D_interpolPt) );
  m_availableVars.insert( std::make_pair("TauJetsAux.ptPanTauCellBased/TauJetsAuxDyn.LC_pantau_interpolPt", &pantau_D_interpolPt) );
  m_availableVars.insert( std::make_pair("TauJetsAux.seedCalo_eta", &seedCalo_eta) );
  
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.nTracks_TauRec", &nTracks) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.nPi0PFOs", &nPi0PFOs) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.PFOEngRelDiff", &PFOEngRelDiff) );
  
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.GhostMuonSegmentCount", &nMuSeg) );

  // Spectator variables declared in the training have to be added to the reader, too
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.truthPtVis", &truthPtVis) );
  
  std::string weightFile = find_file(m_sWeightFileName);

  reader = tauRecTools::configureMVABDT( m_availableVars, weightFile.c_str() );
  if(reader==0) {
    ATH_MSG_FATAL("Couldn't configure MVA");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::execute(xAOD::TauJet& xTau){

  // Retrieve input variables
  
  // Retrieve event info
  static SG::AuxElement::ConstAccessor<double> acc_mu("mu");
  static SG::AuxElement::ConstAccessor<int> acc_nVtx("nVtx");
  mu = acc_mu(xTau);
  nVtx = acc_nVtx(xTau);

  // Retrieve seed jet info
  xTau.detail(xAOD::TauJetParameters::ClustersMeanCenterLambda, center_lambda);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanFirstEngDens, first_eng_dens);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanEMProbability,em_probability);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanSecondLambda, second_lambda);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanPresamplerFrac, presampler_frac);
  int nMuSeg_i=0;
  xTau.detail(xAOD::TauJetParameters::GhostMuonSegmentCount, nMuSeg_i);
  nMuSeg=nMuSeg_i;

  // Retrieve pantau and LC-precalib TES
  seedCalo_eta    = xTau.etaDetectorAxis();
  float pT_LC     = xTau.ptDetectorAxis();
  float pT_pantau = xTau.ptPanTauCellBased();
  xTau.detail(xAOD::TauJetParameters::LC_pantau_interpolPt, interpolPt);
  LC_D_interpolPt     = pT_LC / interpolPt;
  pantau_D_interpolPt = pT_pantau / interpolPt;
  
  // Retrieve substructures info
  nTracks = (float)xTau.nTracks();
  nPi0PFOs = (float) xTau.nPi0s();
  xTau.detail(xAOD::TauJetParameters::PFOEngRelDiff, PFOEngRelDiff);  

  // "Retrieve" spectator variables
  truthPtVis = 0.;

  float ptMVA = float( interpolPt * reader->GetResponse() );
  if(ptMVA<1) ptMVA=1;
  xTau.setP4(xAOD::TauJetParameters::FinalCalib, ptMVA, xTau.etaPanTauCellBased(), xTau.phiPanTauCellBased(), 0);

  return StatusCode::SUCCESS;

}
