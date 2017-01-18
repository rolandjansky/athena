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
  , nVtxPU(0)    
  , center_lambda(0)
  , first_eng_dens(0)
  , second_lambda(0)
  , presampler_frac(0)
  , em_probability(0)    
  , interpolPt(0)
  , LC_D_interpolPt(0)
  , pantau_D_interpolPt(0)
  , etaPanTauCellBased(0)    
  , nTracks(0)
  , nPi0s(0)
  , PFOEngRelDiff(0)    
  , nMuSeg(0)    
  , truthPtVis(0)
  , ptTauEtaCalib(0)
  , ptPanTauCellBased(0)
{
  declareProperty( "WeightFileName", m_sWeightFileName = "MvaTES_20161015_pi0fix_BDTG.weights.root" );
  //Old one: "LC.pantau.interpolPt250GeV_mediumTaus_BDTG.weights.root"
}

//_____________________________________________________________________________
MvaTESEvaluator::~MvaTESEvaluator()
{
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::initialize(){

  // Declare input variables to the reader
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.mu", &mu) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.nVtxPU", &nVtxPU) );
  
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanCenterLambda", &center_lambda) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanFirstEngDens", &first_eng_dens) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanSecondLambda", &second_lambda) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanPresamplerFrac", &presampler_frac) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanEMProbability", &em_probability) );
  
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.LC_pantau_interpolPt", &interpolPt) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.LC_TES_precalib/TauJetsAuxDyn.LC_pantau_interpolPt", &LC_D_interpolPt) );
  m_availableVars.insert( std::make_pair("TauJetsAux.ptPanTauCellBased/TauJetsAuxDyn.LC_pantau_interpolPt", &pantau_D_interpolPt) );
  m_availableVars.insert( std::make_pair("TauJetsAux.etaPanTauCellBased", &etaPanTauCellBased) );
  
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.nPi0s", &nPi0s) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.PFOEngRelDiff", &PFOEngRelDiff) );
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.nTracks", &nTracks) );

  // Spectator variables declared in the training have to be added to the reader, too
  m_availableVars.insert( std::make_pair("TauJetsAuxDyn.truthPtVis", &truthPtVis) );
  m_availableVars.insert( std::make_pair("TauJetsAux.ptTauEtaCalib", &ptTauEtaCalib) );
  m_availableVars.insert( std::make_pair("TauJetsAux.ptPanTauCellBased", &ptPanTauCellBased) );
  
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
  static SG::AuxElement::ConstAccessor<int> acc_nVtxPU("nVtxPU");
  mu = acc_mu(xTau);
  nVtxPU = acc_nVtxPU(xTau);

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
  etaPanTauCellBased = xTau.etaPanTauCellBased();
  float pT_LC     = xTau.ptDetectorAxis();
  float pT_pantau = xTau.ptPanTauCellBased();
  xTau.detail(xAOD::TauJetParameters::LC_pantau_interpolPt, interpolPt);
  LC_D_interpolPt     = pT_LC / interpolPt;
  pantau_D_interpolPt = pT_pantau / interpolPt;
  
  // Retrieve substructures info
  nTracks = (float)xTau.nTracks();
  nPi0s = (float) xTau.nPi0PFOs();
  xTau.detail(xAOD::TauJetParameters::PFOEngRelDiff, PFOEngRelDiff);

  float ptMVA = float( interpolPt * reader->GetResponse() );
  if(ptMVA<1) ptMVA=1;
  xTau.setP4(xAOD::TauJetParameters::FinalCalib, ptMVA, xTau.etaPanTauCellBased(), xTau.phiPanTauCellBased(), 0);

  return StatusCode::SUCCESS;

}
