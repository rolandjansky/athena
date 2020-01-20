/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/HelperFunctions.h"

#include "TFile.h"
#include "TTree.h"

#include <vector>

//_____________________________________________________________________________
MvaTESEvaluator::MvaTESEvaluator(const std::string& name)
  : TauRecToolBase(name)
  , m_reader(0)
  , m_mu(0)
  , m_nVtxPU(0)    
  , m_center_lambda(0)
  , m_first_eng_dens(0)
  , m_second_lambda(0)
  , m_presampler_frac(0)
  , m_em_probability(0)    
  , m_ptCombined(0)
  , m_ptLC_D_ptCombined(0)
  , m_ptConstituent_D_ptCombined(0)
  , m_etaConstituent(0)    
  , m_PanTauBDT_1p0n_vs_1p1n(0)
  , m_PanTauBDT_1p1n_vs_1pXn(0)
  , m_PanTauBDT_3p0n_vs_3pXn(0)
  , m_nTracks(0)
  , m_PFOEngRelDiff(0)    
  , m_truthPtVis(0)
  , m_pt(0)
  , m_ptPanTauCellBased(0)
  , m_ptDetectorAxis(0)
  , m_truthDecayMode(0)
  , m_PanTau_DecayMode(0)
  , m_etaDetectorAxis(0)
  , m_upsilon_cluster(0)
  , m_lead_cluster_frac(0)
{
  declareProperty( "WeightFileName", m_sWeightFileName = "MvaTES_20170207_v2_BDTG.weights.root" );
}

//_____________________________________________________________________________
MvaTESEvaluator::~MvaTESEvaluator()
{
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::initialize(){
  
  // Declare input variables to the reader
  if(!inTrigger()) {
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.mu", &m_mu) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.nVtxPU", &m_nVtxPU) );
    
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanCenterLambda", &m_center_lambda) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanFirstEngDens", &m_first_eng_dens) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanSecondLambda", &m_second_lambda) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanPresamplerFrac", &m_presampler_frac) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanEMProbability", &m_em_probability) );
    
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.pt_combined", &m_ptCombined) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ptDetectorAxis/TauJetsAuxDyn.pt_combined", &m_ptLC_D_ptCombined) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ptPanTauCellBased/TauJetsAuxDyn.pt_combined", &m_ptConstituent_D_ptCombined) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.etaPanTauCellBased", &m_etaConstituent) );
    
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_BDTValue_1p0n_vs_1p1n", &m_PanTauBDT_1p0n_vs_1p1n) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_BDTValue_1p1n_vs_1pXn", &m_PanTauBDT_1p1n_vs_1pXn) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_BDTValue_3p0n_vs_3pXn", &m_PanTauBDT_3p0n_vs_3pXn) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.nTracks", &m_nTracks) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.PFOEngRelDiff", &m_PFOEngRelDiff) );
    
    // Spectator variables declared in the training have to be added to the reader, too
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.truthPtVis", &m_truthPtVis) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.pt", &m_pt) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ptPanTauCellBased", &m_ptPanTauCellBased) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.ptDetectorAxis", &m_ptDetectorAxis) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.truthDecayMode", &m_truthDecayMode) );
    m_availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_DecayMode", &m_PanTau_DecayMode) );
  }
  else {
    m_availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.mu", &m_mu) );
    m_availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanCenterLambda", &m_center_lambda) );
    m_availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanFirstEngDens", &m_first_eng_dens) );
    m_availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanSecondLambda", &m_second_lambda) );
    m_availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanPresamplerFrac", &m_presampler_frac) );
    m_availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanEMProbability", &m_em_probability) );
    m_availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.LeadClusterFrac", &m_lead_cluster_frac) );
    m_availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.UpsilonCluster", &m_upsilon_cluster) );
    m_availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ptDetectorAxis", &m_ptDetectorAxis) );
    m_availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.etaDetectorAxis", &m_etaDetectorAxis) );
  }

  std::string weightFile = find_file(m_sWeightFileName);

  m_reader = tauRecTools::configureMVABDT( m_availableVars, weightFile.c_str() );
  if(m_reader==0) {
    ATH_MSG_FATAL("Couldn't configure MVA");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::eventInitialize()
{
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::execute(xAOD::TauJet& xTau){

  // Retrieve input variables
  
  // Retrieve event info
  const SG::AuxElement::ConstAccessor<float> acc_mu("mu");
  const SG::AuxElement::ConstAccessor<int> acc_nVtxPU("nVtxPU");
  m_mu = acc_mu(xTau);
  m_nVtxPU = acc_nVtxPU(xTau);

  // Retrieve cluster moments
  xTau.detail(xAOD::TauJetParameters::ClustersMeanCenterLambda, m_center_lambda);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanFirstEngDens, m_first_eng_dens);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanEMProbability,m_em_probability);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanSecondLambda, m_second_lambda);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanPresamplerFrac, m_presampler_frac);

  if(!inTrigger()) {

    // Retrieve pantau and LC-precalib TES
    m_etaConstituent = xTau.etaPanTauCellBased();
    float ptLC = xTau.ptDetectorAxis();
    float ptConstituent = xTau.ptPanTauCellBased();
    const SG::AuxElement::ConstAccessor<float> acc_pt_combined("pt_combined");
    m_ptCombined = acc_pt_combined(xTau);

    if(m_ptCombined>0.) {
      m_ptLC_D_ptCombined = ptLC / m_ptCombined;
      m_ptConstituent_D_ptCombined = ptConstituent / m_ptCombined;
    }
    else {
      xTau.setP4(xAOD::TauJetParameters::FinalCalib, 1., m_etaConstituent, xTau.phiPanTauCellBased(), 0);
      // apply MVA calibration as default
      xTau.setP4(1., m_etaConstituent, xTau.phiPanTauCellBased(), 0);
      return StatusCode::SUCCESS;
    }

    // Retrieve substructure info
    const SG::AuxElement::ConstAccessor<float> acc_PanTauBDT_1p0n_vs_1p1n("PanTau_BDTValue_1p0n_vs_1p1n");
    const SG::AuxElement::ConstAccessor<float> acc_PanTauBDT_1p1n_vs_1pXn("PanTau_BDTValue_1p1n_vs_1pXn");
    const SG::AuxElement::ConstAccessor<float> acc_PanTauBDT_3p0n_vs_3pXn("PanTau_BDTValue_3p0n_vs_3pXn");
    m_PanTauBDT_1p0n_vs_1p1n = acc_PanTauBDT_1p0n_vs_1p1n(xTau);
    m_PanTauBDT_1p1n_vs_1pXn = acc_PanTauBDT_1p1n_vs_1pXn(xTau);
    m_PanTauBDT_3p0n_vs_3pXn = acc_PanTauBDT_3p0n_vs_3pXn(xTau);
    m_nTracks = (float)xTau.nTracks();
    xTau.detail(xAOD::TauJetParameters::PFOEngRelDiff, m_PFOEngRelDiff);
    
    float ptMVA = float( m_ptCombined * m_reader->GetResponse() );
    if(ptMVA<1) ptMVA=1;
    xTau.setP4(xAOD::TauJetParameters::FinalCalib, ptMVA, m_etaConstituent, xTau.phiPanTauCellBased(), 0);

    // apply MVA calibration as default
    xTau.setP4(ptMVA, m_etaConstituent, xTau.phiPanTauCellBased(), 0);
  }
  else {

    m_ptDetectorAxis = xTau.ptDetectorAxis();
    m_etaDetectorAxis = xTau.etaDetectorAxis();

    const SG::AuxElement::ConstAccessor<float> acc_UpsilonCluster("UpsilonCluster");
    const SG::AuxElement::ConstAccessor<float> acc_LeadClusterFrac("LeadClusterFrac");
    m_upsilon_cluster = acc_UpsilonCluster(xTau);
    m_lead_cluster_frac = acc_LeadClusterFrac(xTau);

    float ptMVA = float( m_ptDetectorAxis * m_reader->GetResponse() );
    if(ptMVA<1) ptMVA=1;

    // this may have to be changed if we apply a calo-only MVA calibration first, followed by a calo+track MVA calibration
    // in which case, the calo-only would be TauJetParameters::TrigCaloOnly, and the final one TauJetParameters::FinalCalib
    xTau.setP4(xAOD::TauJetParameters::FinalCalib, ptMVA, m_etaDetectorAxis, xTau.phiDetectorAxis(), 0);
    
    // apply MVA calibration
    xTau.setP4(ptMVA, m_etaDetectorAxis, xTau.phiDetectorAxis(), 0);
  }

  return StatusCode::SUCCESS;
}
