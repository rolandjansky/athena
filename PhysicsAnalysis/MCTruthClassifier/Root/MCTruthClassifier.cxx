/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/********************************************************************************

NAME:     MCTruthClassifier.cxx 
PACKAGE:  atlasoff/PhysicsAnalysis/MCTruthClassifier

AUTHORS:  O. Fedin
CREATED:  Sep 2007

PURPOSE:  to classify  truth particles according to their origin. Based on
the truth particle classification the tool provide classification of 
ID and combined muon tracks, egamma electrons (including forward 
electrons) and egamma photons. Both AOD and ESD files can be used.
See  for details: 
https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EgammaMCTruthClassifier            
Updated:  
 ********************************************************************************/

//
#include "MCTruthClassifier/MCTruthClassifier.h"

using namespace MCTruthPartClassifier;
using std::abs;

// -----------------------------------------------------------------------
// Constructor 
// -----------------------------------------------------------------------
MCTruthClassifier::MCTruthClassifier(const std::string& type) :
  asg::AsgTool(type),
  m_thePart{nullptr},
  m_Mother{nullptr},
  m_ParticleOutCome{NonDefinedOutCome},
  m_MotherPDG{0},
  m_MotherStatus{0},
  m_MotherBarcode{0},
  m_NumOfParents{0},
  m_NumOfDaug{0},
  m_MothOriVert{nullptr},
  m_isPrompt{false},
  m_PhotonMother{nullptr},
  m_PhotonMotherPDG{0},
  m_PhotonMotherBarcode{0},
  m_PhotonMotherStatus{0},
  m_BkgElecMother{0},
  m_tauFinalStatePart{},
  m_xaodTruthParticleContainerName{},
  m_barcodeShift{1000000},
  m_barcodeG4Shift{200000},
  m_LQpatch{false}
#ifndef GENERATIONBASE /*Disable when no recostruction packages are expected*/ 
  ,
  m_deltaRMatch{0},
  m_deltaPhi{0},
  m_NumOfSiHits{0},
  m_probability{0.},
  m_egPartPtr{},
  m_egPartdR{},
  m_egPartClas{},
  m_cnvPhtTrkPtr{},
  m_cnvPhtTrPart{},
  m_cnvPhtPartType{},
  m_cnvPhtPartOrig{},
  m_deltaRMatchCut{0.1},
  m_deltaPhiMatchCut{0.1},
  m_NumOfSiHitsCut{0}, 
  m_jetPartDRMatch{0.4}
#endif

#ifndef XAOD_ANALYSIS /*Disable when no recostruction packages are expected*/ 
  , 
  m_truthLinkVecName{}            
#endif

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) /*When no Athena Reconstruction packages expected*/
  ,
  m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
  m_truthInConeTool ("xAOD::TruthParticlesInConeTool/TruthParticlesInConeTool"),
  m_FwdElectronTruthExtrEtaCut{0.},
  m_FwdElectronTruthExtrEtaWindowCut{0.},
  m_partExtrConeEta{0.},
  m_partExtrConePhi{.0},
  m_useCaching{true},
  m_phtClasConePhi{0.},
  m_phtClasConeEta{0.},
  m_phtdRtoTrCut{0.},
  m_fwrdEledRtoTrCut{0.}, 
  m_ROICone{false},
  m_inclG4part{false},
  m_pTChargePartCut{0.0},
  m_pTNeutralPartCut{0.0}
#endif
{

  declareProperty("xAODTruthParticleContainerName" , m_xaodTruthParticleContainerName  = "TruthParticles");
  declareProperty( "LQpatch", m_LQpatch=false);
  declareProperty( "barcodeG4Shift", m_barcodeG4Shift=200001);

#ifndef GENERATIONBASE 
  declareProperty("deltaRMatchCut"   , m_deltaRMatchCut   = 0.2);
  declareProperty("deltaPhiMatchCut" , m_deltaPhiMatchCut = 0.2); 
  declareProperty("NumOfSiHitsCut"   , m_NumOfSiHitsCut   = 3);
  declareProperty("jetPartDRMatch" , m_jetPartDRMatch = 0.4);
#endif

#ifndef XAOD_ANALYSIS 
  declareProperty("xAODTruthLinkVector"            , m_truthLinkVecName="xAODTruthLinks");
#endif

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) /*When no Athena Reconstruction packages expected*/
  declareProperty("ParticleCaloExtensionTool",   m_caloExtensionTool );
  declareProperty("TruthInConeTool",               m_truthInConeTool );
  declareProperty("FwdElectronTruthExtrEtaCut" , m_FwdElectronTruthExtrEtaCut = 2.4, 
                  "Cut on the eta of the truth Particles to be extrapolated for Fwd electrons");
  declareProperty("FwdElectronTruthExtrEtaWindowCut" , m_FwdElectronTruthExtrEtaWindowCut = 0.15, 
                  "Cut on the delta eta of the truth Particles to be extrapolated for Fwd electrons and the current FwdElectron");
  declareProperty("partExtrConePhi"   , m_partExtrConePhi  = 0.4);
  declareProperty("partExtrConeEta"   , m_partExtrConeEta  = 0.2);
  declareProperty("phtClasConePhi"  , m_phtClasConePhi  = 0.05);
  declareProperty("phtClasConeEta"  , m_phtClasConeEta  = 0.025);
  declareProperty("useCaching",      m_useCaching=true);
  declareProperty("phtdRtoTrCut"     , m_phtdRtoTrCut     = 0.1);
  declareProperty("fwrdEledRtoTrCut" , m_fwrdEledRtoTrCut  = 0.15);
  declareProperty("pTChargePartCut"  , m_pTChargePartCut  = 1.0);
  declareProperty("pTNeutralPartCut" , m_pTNeutralPartCut = 0.);
  declareProperty("inclG4part"       , m_inclG4part       = false);
  declareProperty("ROICone"           , m_ROICone  = false);
#endif

}

// DESTRUCTOR:
MCTruthClassifier::~MCTruthClassifier(){  
}

// INITIALIZE METHOD:
StatusCode MCTruthClassifier::initialize(){

  ATH_MSG_INFO( " Initializing MCTruthClassifier" );

  //define barcode scheme
  m_barcodeShift=1000000;
  if(m_barcodeG4Shift==1000001) {m_barcodeShift=10000000;}

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) /*When no Athena Reconstruction packages expected*/
  if( !m_caloExtensionTool.empty() && m_caloExtensionTool.retrieve().isFailure() ) {
    ATH_MSG_WARNING( "Cannot retrieve extrapolateToCaloTool " << m_caloExtensionTool 
                     << " - will not perform extrapolation." );
  }
  if(m_truthInConeTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Cannot retrieve Truth in cone Tool " << m_truthInConeTool);
  }
#endif

  return StatusCode::SUCCESS;
}
// FINALIZE METHOD:
StatusCode  MCTruthClassifier::finalize(){

  return StatusCode::SUCCESS;  
}

// ATHENA EXECUTE METHOD:
StatusCode MCTruthClassifier::execute(){
  return StatusCode::SUCCESS;
}


