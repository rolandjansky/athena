/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/MvaTESEvaluator.h"

// tools include(s) 

#include "TauAnalysisTools/HelperFunctions.h"

//_____________________________________________________________________________
MvaTESEvaluator::MvaTESEvaluator(const std::string& name)
  : TauRecToolBase(name)
{
  declareProperty( "WeightFileName", m_sWeightFileName = "LC.pantau.interpolPt250GeV_mediumTaus_BDTG.weights.xml" );
  //BDT trained on medium taus: "LC.pantau.interpolPt250GeV_mediumTaus_BDTG.weights.xml"
  //BDT trained on all taus:    "LC.pantau.interpolPt250GeV_allTaus_BDTG.weights.xml"
}

//_____________________________________________________________________________
MvaTESEvaluator::~MvaTESEvaluator()
{
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::initialize(){

  // This loads the TMVA library
  TMVA::Tools::Instance();

  // Create the Reader object
  reader = new TMVA::Reader( "!Color:!Silent" );

  // Declare input variables to the reader
  reader->AddVariable( "EventInfoAux.averageInteractionsPerCrossing" , &mu );
  reader->AddVariable( "TauJetsAuxDyn.nVtx"            , &nVtx );
  
  reader->AddVariable( "TauJetsAuxDyn.center_lambda"   , &center_lambda );
  reader->AddVariable( "TauJetsAuxDyn.first_eng_dens"  , &first_eng_dens );
  reader->AddVariable( "TauJetsAuxDyn.second_lambda"   , &second_lambda );
  reader->AddVariable( "TauJetsAuxDyn.presampler_frac" , &presampler_frac );
  reader->AddVariable( "TauJetsAuxDyn.em_probability"  , &em_probability );
  
  reader->AddVariable( "TauJetsAuxDyn.LC_pantau_interpolPt" , &interpolPt );
  reader->AddVariable( "TauJetsAux.LC_TES_precalib/TauJetsAuxDyn.LC_pantau_interpolPt" ,
                        &LC_D_interpolPt );
  reader->AddVariable( "TauJetsAux.ptPanTauCellBased/TauJetsAuxDyn.LC_pantau_interpolPt" ,
                        &pantau_D_interpolPt );
  reader->AddVariable( "TauJetsAux.seedCalo_eta"       , &seedCalo_eta );
  
  reader->AddVariable( "TauJetsAuxDyn.nTracks_TauRec"  , &nTracks );
  reader->AddVariable( "TauJetsAuxDyn.nPi0PFOs"        , &nPi0PFOs );
  reader->AddVariable( "TauJetsAuxDyn.PFOEngRelDiff"   , &PFOEngRelDiff );
  
  reader->AddVariable( "TauJetsAuxDyn.GhostMuonSegmentCount" , &nMuSeg );

  // Spectator variables declared in the training have to be added to the reader, too
  reader->AddSpectator( "TauJetsAuxDyn.truthPtVis"     , &truthPtVis );

  std::string weightFile = find_file(m_sWeightFileName);
  reader->BookMVA( "BDTG", weightFile.c_str() );

  return StatusCode::SUCCESS;

}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::execute(xAOD::TauJet& xTau){

  // Retrieve input variables
  
  // Retrieve event info
  mu = xTau.auxdata<double>("mu");
  nVtx = xTau.auxdata<int>("nVtx");
  
  // Retrieve seed jet info
  center_lambda = xTau.auxdata<double>("center_lambda");
  first_eng_dens = xTau.auxdata<double>("first_eng_dens");
  em_probability = xTau.auxdata<double>("em_probability");
  second_lambda = xTau.auxdata<double>("second_lambda");
  presampler_frac = xTau.auxdata<double>("presampler_frac");
  nMuSeg = (float)xTau.auxdata<int>("GhostMuonSegmentCount");
  
  // Retrieve pantau and LC-precalib TES
  seedCalo_eta    = xTau.auxdata<float>("seedCalo_eta");
  float pT_LC     = xTau.auxdata<float>("LC_TES_precalib");
  float pT_pantau = xTau.ptPanTauCellBased();
  interpolPt      = xTau.auxdata<double>("LC_pantau_interpolPt");
  LC_D_interpolPt     = pT_LC / interpolPt;
  pantau_D_interpolPt = pT_pantau / interpolPt;
  
  // Retrieve substructures info
  nTracks = (float)xTau.nTracks();
  nPi0PFOs = (float)xTau.auxdata<int>("nPi0PFOs");
  PFOEngRelDiff = xTau.auxdata<double>("PFOEngRelDiff");  

  // "Retrieve" spectator variables
  truthPtVis = 0.;

  //  xTau.auxdecor<float>("ptMvaTES") = float( interpolPt * reader->EvaluateRegression( 0, "BDTG" ) );
  float ptMVA = float( interpolPt * reader->EvaluateRegression( 0, "BDTG" ) );
  if(ptMVA<1) ptMVA=1;
  xTau.auxdecor<float>("ptFinalCalib") = ptMVA;
  xTau.auxdecor<float>("etaFinalCalib") = xTau.auxdata<float>("etaPanTauCellBased");
  xTau.auxdecor<float>("phiFinalCalib") = xTau.auxdata<float>("phiPanTauCellBased");
  xTau.auxdecor<float>("mFinalCalib") = 0;

  return StatusCode::SUCCESS;

}
