/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/HelperFunctions.h"

#include <TTree.h>
#include <vector>

//_____________________________________________________________________________
MvaTESEvaluator::MvaTESEvaluator(const std::string& name)
  : TauRecToolBase(name) {
  declareProperty("WeightFileName", m_sWeightFileName = "");
  declareProperty("WeightFileName0p", m_sWeightFileName0p = "");
}

//_____________________________________________________________________________
MvaTESEvaluator::~MvaTESEvaluator() {
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::initialize(){
  
  const std::string weightFile = find_file(m_sWeightFileName);
  m_bdtHelper = std::make_unique<tauRecTools::BDTHelper>();
  ATH_CHECK(m_bdtHelper->initialize(weightFile));

  if(!m_sWeightFileName0p.empty()) {
    const std::string weightFile0p = find_file(m_sWeightFileName0p);
    m_bdtHelper0p = std::make_unique<tauRecTools::BDTHelper>();
    ATH_CHECK(m_bdtHelper0p->initialize(weightFile0p));
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode MvaTESEvaluator::execute(xAOD::TauJet& xTau) const {

  std::map<TString, float*> availableVars;
  MvaInputVariables vars;

  // Declare input variables to the reader
  if(!inTrigger()) {
    availableVars.insert( std::make_pair("TauJetsAuxDyn.mu", &vars.mu) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.nVtxPU", &vars.nVtxPU) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.rho", &vars.rho) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanCenterLambda", &vars.center_lambda) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanFirstEngDens", &vars.first_eng_dens) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanSecondLambda", &vars.second_lambda) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanPresamplerFrac", &vars.presampler_frac) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ClustersMeanEMProbability", &vars.eprobability) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ptIntermediateAxisEM/TauJetsAuxDyn.ptIntermediateAxis", &vars.ptEM_D_ptLC) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.ptIntermediateAxis/TauJetsAuxDyn.ptCombined", &vars.ptLC_D_ptCombined) );
    availableVars.insert( std::make_pair("TauJetsAuxDyn.etaPanTauCellBased", &vars.etaConstituent) );
    if(m_bdtHelper0p && xTau.nTracks()==0) {
      availableVars.insert( std::make_pair("log(TauJetsAuxDyn.ptCombined)", &vars.logPtCombined) );
      availableVars.insert( std::make_pair("TauJetsAuxDyn.LeadClusterFrac", &vars.lead_cluster_frac) );
      availableVars.insert( std::make_pair("TauJetsAuxDyn.centFrac", &vars.centFrac) );
      availableVars.insert( std::make_pair("TauJetsAuxDyn.UpsilonCluster", &vars.upsilon_cluster) );
      availableVars.insert( std::make_pair("TauJetsAuxDyn.ptJetSeed/TauJetsAuxDyn.ptCombined", &vars.ptSeed_D_ptCombined) );
    }
    else {
      availableVars.insert( std::make_pair("TauJetsAuxDyn.ptPanTauCellBased/TauJetsAuxDyn.ptCombined", &vars.ptConstituent_D_ptCombined) );
      availableVars.insert( std::make_pair("TauJetsAuxDyn.ptCombined", &vars.ptCombined) );
      availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_BDTValue_1p0n_vs_1p1n", &vars.PanTauBDT_1p0n_vs_1p1n) );
      availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_BDTValue_1p1n_vs_1pXn", &vars.PanTauBDT_1p1n_vs_1pXn) );
      availableVars.insert( std::make_pair("TauJetsAuxDyn.PanTau_BDTValue_3p0n_vs_3pXn", &vars.PanTauBDT_3p0n_vs_3pXn) );
      availableVars.insert( std::make_pair("TauJetsAuxDyn.nTracks", &vars.nTracks) );
      availableVars.insert( std::make_pair("TauJetsAuxDyn.PFOEngRelDiff", &vars.PFOEngRelDiff) );
    }
  }
  else {
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.mu", &vars.mu) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanCenterLambda", &vars.center_lambda) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanFirstEngDens", &vars.first_eng_dens) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanSecondLambda", &vars.second_lambda) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanPresamplerFrac", &vars.presampler_frac) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ClustersMeanEMProbability", &vars.eprobability) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.LeadClusterFrac", &vars.lead_cluster_frac) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.UpsilonCluster", &vars.upsilon_cluster) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.ptDetectorAxis", &vars.ptDetectorAxis) );
    availableVars.insert( std::make_pair("TrigTauJetsAuxDyn.etaDetectorAxis", &vars.etaDetectorAxis) );
  }

  // Retrieve average pileup
  static const SG::AuxElement::ConstAccessor<float> acc_mu("mu");
  vars.mu = acc_mu(xTau);

  // Retrieve cluster moments
  xTau.detail(xAOD::TauJetParameters::ClustersMeanCenterLambda, vars.center_lambda);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanFirstEngDens, vars.first_eng_dens);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanEMProbability,vars.eprobability);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanSecondLambda, vars.second_lambda);
  xTau.detail(xAOD::TauJetParameters::ClustersMeanPresamplerFrac, vars.presampler_frac);

  if(!inTrigger()) {
    static const SG::AuxElement::ConstAccessor<float> acc_ptCombined("ptCombined");
    float ptCombined = acc_ptCombined(xTau);

    if(ptCombined==0.) {
      xTau.setP4(xAOD::TauJetParameters::FinalCalib, 1., xTau.etaPanTauCellBased(), xTau.phiPanTauCellBased(), 0.);
      // apply MVA calibration as default
      xTau.setP4(1., xTau.etaPanTauCellBased(), xTau.phiPanTauCellBased(), 0.);
      return StatusCode::SUCCESS;
    }

    static const SG::AuxElement::ConstAccessor<int> acc_nVtxPU("nVtxPU");
    vars.nVtxPU = acc_nVtxPU(xTau);

    static const SG::AuxElement::ConstAccessor<float> acc_rho("rho");
    vars.rho = acc_rho(xTau);
    
    static const SG::AuxElement::ConstAccessor<float> acc_ptIntermediateAxisEM("ptIntermediateAxisEM");
    float ptEM = acc_ptIntermediateAxisEM(xTau);

    float ptLC = xTau.ptIntermediateAxis();

    float ptConstituent = xTau.ptPanTauCellBased();
    vars.etaConstituent = xTau.etaPanTauCellBased();

    vars.ptEM_D_ptLC = (ptLC != 0.) ? ptEM / ptLC : 0.;
    vars.ptLC_D_ptCombined = ptLC / ptCombined;

    float ptMVA = 0.;

    if(m_bdtHelper0p && xTau.nTracks()==0) {
      vars.logPtCombined = std::log(ptCombined);
      vars.ptSeed_D_ptCombined = xTau.ptJetSeed() / ptCombined;

      static const SG::AuxElement::ConstAccessor<float> acc_UpsilonCluster("UpsilonCluster");
      vars.upsilon_cluster = acc_UpsilonCluster(xTau);

      static const SG::AuxElement::ConstAccessor<float> acc_LeadClusterFrac("LeadClusterFrac");
      vars.lead_cluster_frac = acc_LeadClusterFrac(xTau);

      xTau.detail(xAOD::TauJetParameters::centFrac, vars.centFrac);

      ptMVA = float( ptCombined * m_bdtHelper0p->getResponse(availableVars) );
    }
    else {
      vars.ptCombined = ptCombined;
      vars.ptConstituent_D_ptCombined = ptConstituent / ptCombined;

      // Retrieve substructure info
      static const SG::AuxElement::ConstAccessor<float> acc_PanTauBDT_1p0n_vs_1p1n("PanTau_BDTValue_1p0n_vs_1p1n");
      static const SG::AuxElement::ConstAccessor<float> acc_PanTauBDT_1p1n_vs_1pXn("PanTau_BDTValue_1p1n_vs_1pXn");
      static const SG::AuxElement::ConstAccessor<float> acc_PanTauBDT_3p0n_vs_3pXn("PanTau_BDTValue_3p0n_vs_3pXn");
      // BDT values are initialised to -1111, while actual scores (when evaluated) are within [-5,1], so take max between BDT score and -5-epsilon
      vars.PanTauBDT_1p0n_vs_1p1n = std::max(acc_PanTauBDT_1p0n_vs_1p1n(xTau), -5.1f);
      vars.PanTauBDT_1p1n_vs_1pXn = std::max(acc_PanTauBDT_1p1n_vs_1pXn(xTau), -5.1f);
      vars.PanTauBDT_3p0n_vs_3pXn = std::max(acc_PanTauBDT_3p0n_vs_3pXn(xTau), -5.1f);
      vars.nTracks = (float)xTau.nTracks();
      xTau.detail(xAOD::TauJetParameters::PFOEngRelDiff, vars.PFOEngRelDiff);
      
      ptMVA = float( ptCombined * m_bdtHelper->getResponse(availableVars) );
    }

    if(ptMVA<1.) ptMVA=1.;
    xTau.setP4(xAOD::TauJetParameters::FinalCalib, ptMVA, vars.etaConstituent, xTau.phiPanTauCellBased(), 0.);
    // apply MVA calibration as default
    xTau.setP4(ptMVA, vars.etaConstituent, xTau.phiPanTauCellBased(), 0.);
  }
  else {

    vars.ptDetectorAxis = xTau.ptDetectorAxis();
    vars.etaDetectorAxis = xTau.etaDetectorAxis();

    static const SG::AuxElement::ConstAccessor<float> acc_UpsilonCluster("UpsilonCluster");
    static const SG::AuxElement::ConstAccessor<float> acc_LeadClusterFrac("LeadClusterFrac");
    vars.upsilon_cluster = acc_UpsilonCluster(xTau);
    vars.lead_cluster_frac = acc_LeadClusterFrac(xTau);

    float ptMVA = float( vars.ptDetectorAxis * m_bdtHelper->getResponse(availableVars) );
    if(ptMVA<1.) ptMVA=1.;

    // this may have to be changed if we apply a calo-only MVA calibration first, followed by a calo+track MVA calibration
    // in which case, the calo-only would be TauJetParameters::TrigCaloOnly, and the final one TauJetParameters::FinalCalib
    xTau.setP4(xAOD::TauJetParameters::FinalCalib, ptMVA, vars.etaDetectorAxis, xTau.phiDetectorAxis(), 0.);
    
    // apply MVA calibration
    xTau.setP4(ptMVA, vars.etaDetectorAxis, xTau.phiDetectorAxis(), 0.);
  }
  
  ATH_MSG_DEBUG("final calib:" << xTau.pt() << " " << xTau.eta() << " " << xTau.phi() << " " << xTau.e());

  return StatusCode::SUCCESS;
}
