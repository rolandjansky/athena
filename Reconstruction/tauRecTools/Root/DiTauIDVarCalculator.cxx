/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/DiTauIDVarCalculator.h"

// Core include(s):
#include "AthLinks/ElementLink.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetConstituentVector.h"

#include "xAODTau/DiTauJet.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "xAODEventInfo/EventInfo.h"

#include "xAODEgamma/ElectronContainer.h"

#include "xAODMuon/MuonContainer.h"

#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationSelection/IsolationSelectionTool.h"

#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"

#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"

#include "tauRecTools/MuonTrackRemoval.h"
#include "tauRecTools/TauSubstructureVariables.h"
#include "tauRecTools/TauCommonCalcVars.h"
#include "tauRecTools/TauJetBDTEvaluator.h"
#include "tauRecTools/TauWPDecorator.h"

#include "PATCore/TResult.h"
// fastjet
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/tools/Filter.hh"
#include "fastjet/tools/MassDropTagger.hh"

using namespace tauRecTools;
using namespace fastjet;

typedef std::vector< ElementLink< xAOD::TrackParticleContainer > >  TrackParticleLinks_t;
typedef ElementLink< xAOD::JetContainer > JetLink_t;

/*
/// Helper macros for checking and retrieving xAOD containers
#define CHECK( ARG )                    \
  do {                                                  \
    const bool result = ARG;                \
    if( ! result ) {                    \
      ::Error( "calcDiTauVariables", "Failed to execute: \"%s\"",  \
           #ARG );                  \
      return 1;                     \
    }                           \
  } while( false )
*/

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
DiTauIDVarCalculator::DiTauIDVarCalculator( const std::string& name )
  : AsgTool(name)
  , m_sDiTauContainerName("DiTauJets")
  , m_eDecayChannel(DecayChannel::Default)
  , m_muSelTool_loose                        ("MuonSelectionToolLoose")
  , m_muSelTool_medium                       ("MuonSelectionToolMedium")
  , m_muSelTool_tight                        ("MuonSelectionToolTight")
  , m_muSelTool_veryloose                    ("MuonSelectionToolVeryLoose")
  , m_muSelTool_highpt                       ("MuonSelectionToolHighPt")
  , m_muonCalibrationTool                    ("MuonCalibrationTool")
  , m_isoSelTool                             ("IsolationSelectionTool")
  , m_electronLikeliHoodTool_medium          ("ElectornLikelihoodToolMedium")   
  , m_electronLikeliHoodTool_loose           ("ElectornLikelihoodToolLoose")
  , m_electronLikeliHoodTool_loose_CutBL     ("ElectornLikelihoodToolLooseCutBL")
  , m_electronLikeliHoodTool_tight           ("ElectornLikelihoodToolTight")
  , m_electronLikeliHoodTool_veryloose       ("ElectornLikelihoodToolVeryLoose")
  , m_muonTrackRemoval                       ("MuonTrackRemoval")
  , m_tauSubstructureVariables               ("TauSubstructureVaribales")
  , m_tauCommonCalcVars                      ("TauCommonCalcVars")
  , m_tauJetBDTEvaluator_1P                  ("TauJetBDTEvaluator_1P")
  , m_tauJetBDTEvaluator_3P                  ("TauJetBDTEvaluator_3P")
  , m_tauWPDecorator                         ("TauWPDecorator")
{
  declareProperty( "DefaultValue", m_dDefault = 0);
  declareProperty( "DiTauContainerName", m_sDiTauContainerName = "DiTauJets");
  declareProperty( "doCalcCluserVariables", m_bCalcCluserVariables = false);
  declareProperty( "DiTauDecayChannel", m_sDecayChannel = "HadHad");
  declareProperty( "MuonTrackRemoval", m_bMuonTrackRemoval = true);
  declareProperty( "RecalcStandardID", m_bRecalcStandardID = false);
  declareProperty( "doCalcSubjetLeadElectronID", m_bCalcSubjetLeadElectronID = false);
}

//______________________________________________________________________________
DiTauIDVarCalculator::~DiTauIDVarCalculator( )
{
}

//______________________________________________________________________________
StatusCode DiTauIDVarCalculator::initialize()
{
  ATH_MSG_INFO( "Initializing DiTauIDVarCalculator" );
  m_DiTauContainerNameAux = m_sDiTauContainerName + "Aux.";
  if(m_sDecayChannel == "HadHad")
    m_eDecayChannel = DecayChannel::HadHad;
  if(m_sDecayChannel == "HadEl")
    m_eDecayChannel = DecayChannel::HadEl;
  if(m_sDecayChannel == "HadMu")
    m_eDecayChannel = DecayChannel::HadMu;
  if(m_eDecayChannel == DecayChannel::Default){
    ATH_MSG_ERROR( "No Valid DecayChannel initialized. Valid options are: HadHad, HadEl and HadMu" );
    return StatusCode::FAILURE;
  }
  
  if(m_eDecayChannel == DecayChannel::HadMu || m_eDecayChannel == DecayChannel::HadEl){
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_isoSelTool, CP::IsolationSelectionTool));
    ANA_CHECK(m_isoSelTool.setProperty("ElectronWP","GradientLoose"));
    ANA_CHECK(m_isoSelTool.setProperty("MuonWP","GradientLoose"));
    ANA_CHECK(m_isoSelTool.initialize());
  }
  
  if(m_eDecayChannel == DecayChannel::HadMu){
      ATH_CHECK(ASG_MAKE_ANA_TOOL(m_muSelTool_loose, CP::MuonSelectionTool));
      ANA_CHECK(m_muSelTool_loose.setProperty("MuQuality", 2));
      ANA_CHECK(m_muSelTool_loose.initialize());

      ATH_CHECK(ASG_MAKE_ANA_TOOL(m_muSelTool_medium, CP::MuonSelectionTool));
      ANA_CHECK(m_muSelTool_medium.setProperty("MuQuality", 1));
      ANA_CHECK(m_muSelTool_medium.initialize());

      ATH_CHECK(ASG_MAKE_ANA_TOOL(m_muSelTool_tight, CP::MuonSelectionTool));
      ANA_CHECK(m_muSelTool_tight.setProperty("MuQuality", 0)); 
      ANA_CHECK(m_muSelTool_tight.initialize());

      ATH_CHECK(ASG_MAKE_ANA_TOOL(m_muSelTool_veryloose, CP::MuonSelectionTool));
      ANA_CHECK(m_muSelTool_veryloose.setProperty("MuQuality", 3));
      ANA_CHECK(m_muSelTool_veryloose.initialize());

      ATH_CHECK(ASG_MAKE_ANA_TOOL(m_muSelTool_highpt, CP::MuonSelectionTool));
      ANA_CHECK(m_muSelTool_highpt.setProperty("MuQuality", 4));
      ANA_CHECK(m_muSelTool_highpt.initialize());

      ATH_CHECK(ASG_MAKE_ANA_TOOL(m_muonCalibrationTool, CP::MuonCalibrationAndSmearingTool));
      //m_muonCalibrationTool.msg().setLevel( MSG::INFO);
      ANA_CHECK(m_muonCalibrationTool.setProperty( "Year", "Data16" ));
      ANA_CHECK(m_muonCalibrationTool.setProperty("Release","Recs2016_15_07"));
      ANA_CHECK(m_muonCalibrationTool.setProperty("StatComb",false));
      ANA_CHECK(m_muonCalibrationTool.setProperty("SagittaCorr",false));
      ANA_CHECK(m_muonCalibrationTool.setProperty("SagittaRelease", "sagittaBiasDataAll_02_08_17"));
      ANA_CHECK(m_muonCalibrationTool.setProperty("doSagittaMCDistortion",false));
      ANA_CHECK(m_muonCalibrationTool.initialize());

      if(m_bMuonTrackRemoval){
        ATH_CHECK(ASG_MAKE_ANA_TOOL(m_muonTrackRemoval, tauRecTools::MuonTrackRemoval));
        ToolHandle<CP::IMuonCalibrationAndSmearingTool> thCalibrationAndSmearingTool(&*m_muonCalibrationTool);
        ToolHandle<CP::IMuonSelectionTool> thMuonSelectionTool(&*m_muSelTool_loose);
        ATH_CHECK(m_muonTrackRemoval.setProperty("MuonCalibrationToolHandle", thCalibrationAndSmearingTool));
        ATH_CHECK(m_muonTrackRemoval.setProperty("MuonSelectionToolHandle", thMuonSelectionTool));
        ATH_CHECK(m_muonTrackRemoval.initialize());

        ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tauSubstructureVariables, TauSubstructureVariables));
        ATH_CHECK(m_tauSubstructureVariables.initialize());
        m_tauSubstructureVariables->setTauEventData(&m_data);

        ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tauCommonCalcVars, TauCommonCalcVars));
        ATH_CHECK(m_tauCommonCalcVars.initialize());

        ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tauJetBDTEvaluator_1P, TauJetBDTEvaluator));
        ATH_CHECK(m_tauJetBDTEvaluator_1P.setProperty("weightsFile", "tauRecTools/00-02-00/vars2016_pt_gamma_1p_isofix.root"));
        ATH_CHECK(m_tauJetBDTEvaluator_1P.setProperty("minNTracks", 0));
        ATH_CHECK(m_tauJetBDTEvaluator_1P.setProperty("maxNTracks", 1));
        ATH_CHECK(m_tauJetBDTEvaluator_1P.setProperty("outputVarName", "BDTScoreRecalc"));
        ATH_CHECK(m_tauJetBDTEvaluator_1P.initialize());

        ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tauJetBDTEvaluator_3P, TauJetBDTEvaluator));
        ATH_CHECK(m_tauJetBDTEvaluator_3P.setProperty("weightsFile", "tauRecTools/00-02-00/vars2016_pt_gamma_3p_isofix.root"));
        ATH_CHECK(m_tauJetBDTEvaluator_3P.setProperty("minNTracks", 2));
        ATH_CHECK(m_tauJetBDTEvaluator_3P.setProperty("maxNTracks", 1000));
        ATH_CHECK(m_tauJetBDTEvaluator_3P.setProperty("outputVarName", "BDTScoreRecalc"));
        ATH_CHECK(m_tauJetBDTEvaluator_3P.initialize());
        
        ATH_CHECK(ASG_MAKE_ANA_TOOL(m_tauWPDecorator, TauWPDecorator));
        ATH_CHECK(m_tauWPDecorator.setProperty("flatteningFile1Prong", "tauRecTools/00-02-00/FlatJetBDT1Pv2.root"));
        ATH_CHECK(m_tauWPDecorator.setProperty("flatteningFile3Prong", "tauRecTools/00-02-00/FlatJetBDT3Pv2.root"));
        ATH_CHECK(m_tauWPDecorator.setProperty("ScoreName", "BDTScoreRecalc"));
        ATH_CHECK(m_tauWPDecorator.setProperty("NewScoreName", "BDTScoreFlatRecalc"));
        ATH_CHECK(m_tauWPDecorator.setProperty("DefineWPs", false));
        ATH_CHECK(m_tauWPDecorator.initialize());
      }
  }
  if(m_eDecayChannel == DecayChannel::HadEl || m_eDecayChannel == DecayChannel::HadHad){
    std::string confDir = "ElectronPhotonSelectorTools/offline/mc16_20170828/";
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_electronLikeliHoodTool_medium, AsgElectronLikelihoodTool));
    ATH_CHECK(m_electronLikeliHoodTool_medium.setProperty("ConfigFile",confDir+"ElectronLikelihoodMediumOfflineConfig2017_Smooth.conf"));

    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_electronLikeliHoodTool_loose, AsgElectronLikelihoodTool));
    ATH_CHECK(m_electronLikeliHoodTool_loose.setProperty("ConfigFile",confDir+"ElectronLikelihoodLooseOfflineConfig2017_Smooth.conf"));
    
    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_electronLikeliHoodTool_loose_CutBL, AsgElectronLikelihoodTool));
    ATH_CHECK(m_electronLikeliHoodTool_loose_CutBL.setProperty("ConfigFile",confDir+"ElectronLikelihoodLooseOfflineConfig2017_CutBL_Smooth.conf"));

    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_electronLikeliHoodTool_tight, AsgElectronLikelihoodTool));
    ATH_CHECK(m_electronLikeliHoodTool_tight.setProperty("ConfigFile",confDir+"ElectronLikelihoodTightOfflineConfig2017_Smooth.conf"));

    ATH_CHECK(ASG_MAKE_ANA_TOOL(m_electronLikeliHoodTool_veryloose, AsgElectronLikelihoodTool));
    ATH_CHECK(m_electronLikeliHoodTool_veryloose.setProperty("ConfigFile",confDir+"ElectronLikelihoodVeryLooseOfflineConfig2017_Smooth.conf"));
  }
  
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//                              Wrapper functions                             //
////////////////////////////////////////////////////////////////////////////////

StatusCode DiTauIDVarCalculator::calculateIDVariables(const xAOD::DiTauJet& xDiTau){
  return execute(xDiTau);
}

StatusCode DiTauIDVarCalculator::execute(const xAOD::DiTauJet& xDiTau)
{
  switch(m_eDecayChannel) {
  case(DecayChannel::HadHad):
    return calculateHadHadIDVariables(xDiTau);
    break;
  case(DecayChannel::HadEl):
    return calculateHadElIDVariables(xDiTau);
    break;
  case(DecayChannel::HadMu):
    return calculateHadMuIDVariables(xDiTau);
    break;
  default:
    return StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}

StatusCode DiTauIDVarCalculator::calculateHadHadIDVariables(const xAOD::DiTauJet& xDiTau)
{
  ATH_MSG_DEBUG("Calculate DiTau ID variables");
  
  xDiTau.auxdecor< int >("n_subjets") = n_subjets(xDiTau);
  ATH_CHECK( decorNtracks(xDiTau) );
  xDiTau.auxdecor< float >( "ditau_pt") = ditau_pt(xDiTau);
  xDiTau.auxdecor< float >( "f_core_lead" ) = f_core(xDiTau, 0);
  xDiTau.auxdecor< float >( "f_core_subl" ) = f_core(xDiTau, 1);
  xDiTau.auxdecor< float >( "f_subjet_lead" ) = f_subjet(xDiTau, 0);
  xDiTau.auxdecor< float >( "f_subjet_subl" ) = f_subjet(xDiTau, 1);
  xDiTau.auxdecor< float >( "f_subjets") = f_subjets(xDiTau);
  xDiTau.auxdecor< float >( "f_track_lead") = f_track(xDiTau, 0);
  xDiTau.auxdecor< float >( "f_track_subl") = f_track(xDiTau, 1);
  xDiTau.auxdecor< float >( "R_max_lead") = R_max(xDiTau, 0);
  xDiTau.auxdecor< float >( "R_max_subl") = R_max(xDiTau, 1);
  xDiTau.auxdecor< int >( "n_track" ) = n_track(xDiTau);
  xDiTau.auxdecor< int >( "n_tracks_lead" ) = n_tracks(xDiTau, 0);
  xDiTau.auxdecor< int >( "n_tracks_subl" ) = n_tracks(xDiTau, 1);
  xDiTau.auxdecor< int >( "n_isotrack" ) = n_isotrack(xDiTau);
  xDiTau.auxdecor< float >( "R_track" ) = R_track(xDiTau);
  xDiTau.auxdecor< float >( "R_track_core" ) = R_track_core(xDiTau);
  xDiTau.auxdecor< float >( "R_track_all" ) = R_track_all(xDiTau);
  xDiTau.auxdecor< float >( "R_isotrack" ) = R_isotrack(xDiTau);
  xDiTau.auxdecor< float >( "R_core_lead" ) = R_core(xDiTau, 0);
  xDiTau.auxdecor< float >( "R_core_subl" ) = R_core(xDiTau, 1);
  xDiTau.auxdecor< float >( "R_tracks_lead" ) = R_tracks(xDiTau, 0);
  xDiTau.auxdecor< float >( "R_tracks_subl" ) = R_tracks(xDiTau, 1);
  xDiTau.auxdecor< float >( "m_track" ) = mass_track(xDiTau);
  xDiTau.auxdecor< float >( "m_track_core" ) = mass_track_core(xDiTau);
  xDiTau.auxdecor< float >( "m_core_lead" ) = mass_core(xDiTau, 0);
  xDiTau.auxdecor< float >( "m_core_subl" ) = mass_core(xDiTau, 1);
  xDiTau.auxdecor< float >( "m_track_all" ) = mass_track_all(xDiTau);
  xDiTau.auxdecor< float >( "m_tracks_lead" ) = mass_tracks(xDiTau, 0);
  xDiTau.auxdecor< float >( "m_tracks_subl" ) = mass_tracks(xDiTau, 1);
  xDiTau.auxdecor< float >( "E_frac_subl" ) = E_frac(xDiTau,1);
  xDiTau.auxdecor< float >( "E_frac_subsubl") = E_frac(xDiTau, 2);
  xDiTau.auxdecor< float >( "R_subjets_subl") = R_subjets(xDiTau, 1);
  xDiTau.auxdecor< float >( "R_subjets_subsubl") = R_subjets(xDiTau, 2);
  xDiTau.auxdecor< float >( "d0_leadtrack_lead") = d0_leadtrack(xDiTau, 0);
  xDiTau.auxdecor< float >( "d0_leadtrack_subl") = d0_leadtrack(xDiTau, 1);
  xDiTau.auxdecor< float >( "f_isotracks" ) = f_isotracks(xDiTau);
  xDiTau.auxdecor< int >( "n_iso_ellipse" ) = n_iso_ellipse(xDiTau);
  if (m_bCalcSubjetLeadElectronID)
  {
    xDiTau.auxdecor< int >( "leadjet_elid" ) = subjetLeadElectronID(xDiTau, 0);
    xDiTau.auxdecor< int >( "subleadjet_elid" ) = subjetLeadElectronID(xDiTau, 1);
  }

  
  if (m_bCalcCluserVariables)
  {
    std::vector<PseudoJet> vClusters = getClusters(xDiTau);
    
    xDiTau.auxdecor< int >( "n_antikt_subjets" ) = n_antikt_subjets(vClusters);
    xDiTau.auxdecor< int >( "n_ca_subjets" ) = n_ca_subjets(vClusters);
    xDiTau.auxdecor< float >( "f_clusters" ) = f_clusters(xDiTau, vClusters);
    mass_drop(xDiTau, vClusters);
  }
  else 
  {
    xDiTau.auxdecor< int >( "n_antikt_subjets" ) = m_dDefault;
    xDiTau.auxdecor< int >( "n_ca_subjets" ) = m_dDefault;
    xDiTau.auxdecor< float >( "f_clusters" ) = m_dDefault;
    xDiTau.auxdecor< float >( "mu_massdrop" ) = m_dDefault;
    xDiTau.auxdecor< float >( "y_massdrop" ) = m_dDefault;
  }


  return StatusCode::SUCCESS;
}

StatusCode DiTauIDVarCalculator::calculateHadMuIDVariables(const xAOD::DiTauJet& xDiTau){
  
  static const SG::AuxElement::ConstAccessor<float>  acc_centFrac           ("centFrac")          ;
  static const SG::AuxElement::ConstAccessor<float>  acc_etOverPtLeadTrk    ("etOverPtLeadTrk")   ;
  static const SG::AuxElement::ConstAccessor<float>  acc_innerTrkAvgDist    ("innerTrkAvgDist")   ;
  static const SG::AuxElement::ConstAccessor<float>  acc_absipSigLeadTrk    ("absipSigLeadTrk")   ;
  static const SG::AuxElement::ConstAccessor<float>  acc_SumPtTrkFrac       ("SumPtTrkFrac")      ;
  static const SG::AuxElement::ConstAccessor<float>  acc_EMPOverTrkSysP     ("EMPOverTrkSysP")    ;
  static const SG::AuxElement::ConstAccessor<float>  acc_ptRatioEflowApprox ("ptRatioEflowApprox");
  static const SG::AuxElement::ConstAccessor<float>  acc_mEflowApprox       ("mEflowApprox")      ;
  static const SG::AuxElement::ConstAccessor<float>  acc_dRmax              ("dRmax")             ;
  static const SG::AuxElement::ConstAccessor<float>  acc_trFlightPathSig    ("trFlightPathSig")   ;
  static const SG::AuxElement::ConstAccessor<float>  acc_massTrkSys         ("massTrkSys")        ;
  static const SG::AuxElement::ConstAccessor<float>  acc_ChPiEMEOverCaloEME ("ChPiEMEOverCaloEME");

  static const SG::AuxElement::ConstAccessor<float>  acc_BDTScoreRecalc     ("BDTScoreRecalc");
  static const SG::AuxElement::ConstAccessor<float>  acc_BDTScoreFlatRecalc ("BDTScoreFlatRecalc");

  static const SG::AuxElement::Decorator<float>  dec_centFrac           ("centFrac")          ;
  static const SG::AuxElement::Decorator<float>  dec_massTrkSys         ("massTrkSys")        ;
  static const SG::AuxElement::Decorator<float>  dec_etOverPtLeadTrk    ("etOverPtLeadTrk")   ;
  static const SG::AuxElement::Decorator<float>  dec_innerTrkAvgDist    ("innerTrkAvgDist")   ;
  static const SG::AuxElement::Decorator<float>  dec_absipSigLeadTrk    ("absipSigLeadTrk")   ;
  static const SG::AuxElement::Decorator<float>  dec_SumPtTrkFrac       ("SumPtTrkFrac")      ;
  static const SG::AuxElement::Decorator<float>  dec_EMPOverTrkSysP     ("EMPOverTrkSysP")    ;
  static const SG::AuxElement::Decorator<float>  dec_ptRatioEflowApprox ("ptRatioEflowApprox");
  static const SG::AuxElement::Decorator<float>  dec_mEflowApprox       ("mEflowApprox")      ;
  static const SG::AuxElement::Decorator<float>  dec_dRmax              ("dRmax")             ;
  static const SG::AuxElement::Decorator<float>  dec_trFlightPathSig    ("trFlightPathSig")   ;
  static const SG::AuxElement::Decorator<float>  dec_ChPiEMEOverCaloEME ("ChPiEMEOverCaloEME");

  static const SG::AuxElement::Decorator<float>  dec_BDTScoreRecalc     ("TauJetBDTScoreRecalc");
  static const SG::AuxElement::Decorator<float>  dec_BDTScoreFlatRecalc ("TauJetBDTScoreFlatRecalc");
  
  static const SG::AuxElement::Decorator<int>    dec_tau_ntrack         ("tau_ntrack");
  
  static const SG::AuxElement::Decorator<char>   acc_mu_isoGL     ("mu_isoGL");
  
  static const SG::AuxElement::Decorator<float> dec_MeasEnergyLoss            ("mu_MeasEnergyLoss")          ;
  static const SG::AuxElement::Decorator<float> dec_ParamEnergyLoss           ("mu_ParamEnergyLoss")         ;
  static const SG::AuxElement::Decorator<float> dec_MeasEnergyLossSigma       ("mu_MeasEnergyLossSigma")     ;
  static const SG::AuxElement::Decorator<float> dec_ParamEnergyLossSigmaPlus  ("mu_ParamEnergyLossSigmaPlus");
  static const SG::AuxElement::Decorator<float> dec_ParamEnergyLossSigmaMinus ("mu_ParamEnergyLossSigmaMinus")    ;
        
  static const SG::AuxElement::Accessor<ElementLink<xAOD::MuonContainer>>   acc_muonLink ("muonLink");
  static const SG::AuxElement::Accessor<ElementLink<xAOD::TauJetContainer>> acc_tauLink  ("tauLink");

  static const SG::AuxElement::Decorator<char>  acc_muonVeryLoose ("muonVeryLoose");
  static const SG::AuxElement::Decorator<char>  acc_muonLoose     ("muonLoose");
  static const SG::AuxElement::Decorator<char>  acc_muonMedium    ("muonMedium");
  static const SG::AuxElement::Decorator<char>  acc_muonTight     ("muonTight");
  static const SG::AuxElement::Decorator<char>  acc_muonHighpt    ("muonHighpt");
  static const SG::AuxElement::Decorator<int>   acc_MuonQuality   ("MuonQuality");
  static const SG::AuxElement::Decorator<int>   acc_MuonType      ("MuonType");
  static const SG::AuxElement::Decorator<char>  acc_validMuon     ("validMuon");


  const xAOD::TauJet* pTau  = *acc_tauLink(xDiTau);
  const xAOD::Muon*   pMuon = *acc_muonLink(xDiTau);
  std::unique_ptr<xAOD::TauJet> pTauCopy;

  if(m_bMuonTrackRemoval){
    pTauCopy.reset(new xAOD::TauJet());
    pTauCopy->makePrivateStore(*pTau);
    m_data.clear();
    ATH_CHECK(m_muonTrackRemoval->execute(*pTauCopy));

    // set variables to default values, if last track of tau was removed
    if (pTauCopy->nTracks() == 0) {
      pTauCopy->setDetail( xAOD::TauJetParameters::etOverPtLeadTrk, static_cast<float>( -1111 ) );
      pTauCopy->setDetail( xAOD::TauJetParameters::dRmax, static_cast<float>( -1111 ) );
      pTauCopy->setDetail( xAOD::TauJetParameters::massTrkSys, static_cast<float>( -1111 ) );
      pTauCopy->setDetail( xAOD::TauJetParameters::innerTrkAvgDist, static_cast<float>( -1111 ) );
    }
    
    ATH_CHECK(m_tauSubstructureVariables->execute(*pTauCopy));
    ATH_CHECK(m_tauCommonCalcVars->execute(*pTauCopy));

    if(m_bRecalcStandardID){
      // recalculate and decorate output of standard Tau ID after muon track removal:
      // set some input variables for standard Tau ID:
      
      static const SG::AuxElement::Accessor<int> acc_numTrack("NUMTRACK");
      acc_numTrack(*pTauCopy) = pTauCopy->nTracks();
      const xAOD::EventInfo* xEventInfo;
      static const SG::AuxElement::Accessor<float> acc_mu("MU");
      ATH_CHECK( evtStore()->retrieve(xEventInfo,"EventInfo") );

      static const SG::AuxElement::Accessor<float> acc_absEtaLead("ABS_ETA_LEAD_TRACK");
      
      if(pTauCopy->nTracks()>0)
        acc_absEtaLead(*pTauCopy) = pTauCopy->track(0)->eta();
      else
        acc_absEtaLead(*pTauCopy) = 0;
      
      acc_mu(*pTauCopy) = xEventInfo->averageInteractionsPerCrossing();
      
      ATH_CHECK(m_tauJetBDTEvaluator_1P->execute(*pTauCopy));
      ATH_CHECK(m_tauJetBDTEvaluator_3P->execute(*pTauCopy));
      ATH_CHECK(m_tauWPDecorator->execute(*pTauCopy));
    
      dec_BDTScoreRecalc    (xDiTau) = acc_BDTScoreRecalc    (*pTauCopy);
      dec_BDTScoreFlatRecalc(xDiTau) = acc_BDTScoreFlatRecalc(*pTauCopy);
    }
    pTau = &*pTauCopy;
  }
    
  // Decorate MuonQuality and Tau-ID-Vars to DiTauCandidate:

  dec_centFrac          (xDiTau) = acc_centFrac          (*pTau);
  dec_etOverPtLeadTrk   (xDiTau) = acc_etOverPtLeadTrk   (*pTau);
  dec_innerTrkAvgDist   (xDiTau) = acc_innerTrkAvgDist   (*pTau);
  dec_absipSigLeadTrk   (xDiTau) = acc_absipSigLeadTrk   (*pTau);
  dec_SumPtTrkFrac      (xDiTau) = acc_SumPtTrkFrac      (*pTau);
  dec_EMPOverTrkSysP    (xDiTau) = acc_EMPOverTrkSysP    (*pTau);
  dec_ptRatioEflowApprox(xDiTau) = acc_ptRatioEflowApprox(*pTau);
  dec_mEflowApprox      (xDiTau) = acc_mEflowApprox      (*pTau);
  dec_dRmax             (xDiTau) = acc_dRmax             (*pTau);
  dec_trFlightPathSig   (xDiTau) = acc_trFlightPathSig   (*pTau);
  dec_massTrkSys        (xDiTau) = acc_massTrkSys        (*pTau);
  dec_ChPiEMEOverCaloEME(xDiTau) = acc_ChPiEMEOverCaloEME(*pTau);
  dec_tau_ntrack        (xDiTau) = pTau->nTracks();

  xAOD::Muon* mu = nullptr;
  //callibrate Muon:
  if( !m_muonCalibrationTool->correctedCopy( *pMuon, mu ) ) {
    ATH_MSG_ERROR( "MuonCalibrationTool can not be applied" );
    acc_validMuon(xDiTau) = false;
    return StatusCode::FAILURE;
  }
  acc_validMuon(xDiTau) = true;
  acc_mu_isoGL      (xDiTau) = (bool)(m_isoSelTool->accept(*mu));

  float MeasEnergyLoss            = 0;
  float ParamEnergyLoss           = 0;
  float MeasEnergyLossSigma       = 0;
  float ParamEnergyLossSigmaPlus  = 0;
  float ParamEnergyLossSigmaMinus = 0;
  
  if( ! mu->parameter (MeasEnergyLoss           , xAOD::Muon::MeasEnergyLoss           ) )
    Info("DiTaulepHadVarCalc", "MuonParameter not available");
  if( ! mu->parameter (ParamEnergyLoss          , xAOD::Muon::ParamEnergyLoss          ) )
    Info("DiTaulepHadVarCalc", "MuonParameter not available");
  if( ! mu->parameter (MeasEnergyLossSigma      , xAOD::Muon::MeasEnergyLossSigma      ) )
    Info("DiTaulepHadVarCalc", "MuonParameter not available");
  if( ! mu->parameter (ParamEnergyLossSigmaPlus , xAOD::Muon::ParamEnergyLossSigmaPlus ) )
    Info("DiTaulepHadVarCalc", "MuonParameter not available");
  if( ! mu->parameter (ParamEnergyLossSigmaMinus, xAOD::Muon::ParamEnergyLossSigmaMinus) )
    Info("DiTaulepHadVarCalc", "MuonParameter not available");
  
  dec_MeasEnergyLoss            (xDiTau) =  MeasEnergyLoss           ; 
  dec_ParamEnergyLoss           (xDiTau) =  ParamEnergyLoss          ;
  dec_MeasEnergyLossSigma       (xDiTau) =  MeasEnergyLossSigma      ;
  dec_ParamEnergyLossSigmaPlus  (xDiTau) =  ParamEnergyLossSigmaPlus ;
  dec_ParamEnergyLossSigmaMinus (xDiTau) =  ParamEnergyLossSigmaMinus;

  acc_muonVeryLoose (xDiTau) = m_muSelTool_veryloose->accept(*mu);
  acc_muonMedium    (xDiTau) = m_muSelTool_medium   ->accept(*mu);
  acc_muonTight     (xDiTau) = m_muSelTool_tight    ->accept(*mu);  
  acc_muonLoose     (xDiTau) = m_muSelTool_loose    ->accept(*mu);
  acc_muonHighpt    (xDiTau) = m_muSelTool_highpt   ->accept(*mu);
  acc_MuonQuality   (xDiTau) = m_muSelTool_veryloose->getQuality(*mu);
  acc_MuonType      (xDiTau) = mu->muonType(); 

  delete mu;
  
  return StatusCode::SUCCESS;
}

StatusCode DiTauIDVarCalculator::calculateHadElIDVariables(const xAOD::DiTauJet& xDiTau){
  static const SG::AuxElement::ConstAccessor<ElementLink<xAOD::DiTauJetContainer>> acc_origDitau("origDiTauLink");
  static const SG::AuxElement::ConstAccessor<ElementLink<xAOD::ElectronContainer>>  acc_electron ("elLink");
  static const SG::AuxElement::ConstAccessor<int> acc_origDiTauSubjet("origSubjetIndex");

  if(!acc_origDitau(xDiTau).isValid())
    return StatusCode::FAILURE;
  if(!acc_electron(xDiTau).isValid())
    return StatusCode::FAILURE;
  
  const xAOD::DiTauJet* origDiTau = *(acc_origDitau(xDiTau));
  const xAOD::Electron* electron  = *(acc_electron(xDiTau));
    
  static const SG::AuxElement::Decorator< float >  acc_tau_f_core          ( "tau_f_core"       );      
  static const SG::AuxElement::Decorator< float >  acc_tau_f_subjet        ( "tau_f_subjet"     );
  static const SG::AuxElement::Decorator< float >  acc_tau_f_track         ( "tau_f_track"      );
  static const SG::AuxElement::Decorator< float >  acc_tau_R_max           ( "tau_R_max"        );
  static const SG::AuxElement::Decorator< int >    acc_tau_n_tracks        ( "tau_n_tracks"     );
  static const SG::AuxElement::Decorator< float >  acc_tau_R_core          ( "tau_R_core"       );
  static const SG::AuxElement::Decorator< float >  acc_tau_R_tracks        ( "tau_R_tracks"     );
  static const SG::AuxElement::Decorator< float >  acc_tau_m_core          ( "tau_m_core"       );
  static const SG::AuxElement::Decorator< float >  acc_tau_m_tracks        ( "tau_m_tracks"     );
  static const SG::AuxElement::Decorator< float >  acc_tau_d0_leadtrack    ( "tau_d0_leadtrack" );
                                                                                              
  static const SG::AuxElement::Decorator< float >  acc_f_subjets           ( "f_subjets"    );
  static const SG::AuxElement::Decorator< int >    acc_n_track             ( "n_track"      );
  static const SG::AuxElement::Decorator< int >    acc_n_isotrack          ( "n_isotrack"   );
  static const SG::AuxElement::Decorator< float >  acc_R_track             ( "R_track"      );
  static const SG::AuxElement::Decorator< float >  acc_R_track_core        ( "R_track_core" );
  static const SG::AuxElement::Decorator< float >  acc_R_track_all         ( "R_track_all"  );
  static const SG::AuxElement::Decorator< float >  acc_R_isotrack          ( "R_isotrack"   );
  static const SG::AuxElement::Decorator< float >  acc_m_track             ( "m_track"      );  
  static const SG::AuxElement::Decorator< float >  acc_m_track_core        ( "m_track_core" );
  static const SG::AuxElement::Decorator< float >  acc_m_track_all         ( "m_track_all"  );
                                                                                              
  static const SG::AuxElement::Decorator< float >  acc_E_frac_subl         ( "E_frac_subl"       );    
  static const SG::AuxElement::Decorator< float >  acc_E_frac_subsubl      ( "E_frac_subsubl"    ); 
  static const SG::AuxElement::Decorator< float >  acc_R_subjets_subl      ( "R_subjets_subl"    ); 
  static const SG::AuxElement::Decorator< float >  acc_R_subjets_subsubl   ( "R_subjets_subsubl" );
  static const SG::AuxElement::Decorator< float >  acc_f_isotracks         ( "f_isotracks"       );    
  static const SG::AuxElement::Decorator< int >    acc_n_iso_ellipse       ( "n_iso_ellipse"     );  
                                                                                          
  static const SG::AuxElement::Decorator< float >  acc_E_frac_HadEl        ( "E_frac_HadEl"      );     

  static const SG::AuxElement::Decorator< float > acc_el_f1core              ( "el_f1core"           );
  static const SG::AuxElement::Decorator< float > acc_el_f3core              ( "el_f3core"           );
  static const SG::AuxElement::Decorator< float > acc_el_weta1               ( "el_weta1"            );
  static const SG::AuxElement::Decorator< float > acc_el_fracs1              ( "el_fracs1"           );
  static const SG::AuxElement::Decorator< float > acc_el_poscs1              ( "el_poscs1"           );
  static const SG::AuxElement::Decorator< float > acc_el_poscs2              ( "el_poscs2"           );
  static const SG::AuxElement::Decorator< float > acc_el_asy1                ( "el_asy1"             );
  static const SG::AuxElement::Decorator< float > acc_el_pos                 ( "el_pos"              );
  static const SG::AuxElement::Decorator< float > acc_el_pos7                ( "el_pos7"             );
  static const SG::AuxElement::Decorator< float > acc_el_barys1              ( "el_barys1"           );
  static const SG::AuxElement::Decorator< float > acc_el_wtots1              ( "el_wtots1"           );
  static const SG::AuxElement::Decorator< float > acc_el_r33over37allcalo    ( "el_r33over37allcalo" );
  static const SG::AuxElement::Decorator< char >  acc_el_isoGL               ( "el_isoGL"            );


  acc_tau_f_core       (xDiTau) = f_core      (xDiTau, 0);
  acc_tau_f_subjet     (xDiTau) = f_subjet    (xDiTau, 0);
  acc_tau_f_track      (xDiTau) = f_track     (xDiTau, 0);
  acc_tau_R_max        (xDiTau) = R_max       (xDiTau, 0);
  acc_tau_n_tracks     (xDiTau) = n_tracks    (xDiTau, 0);
  acc_tau_R_core       (xDiTau) = R_core      (xDiTau, 0);
  acc_tau_R_tracks     (xDiTau) = R_tracks    (xDiTau, 0);
  acc_tau_m_core       (xDiTau) = mass_core   (xDiTau, 0);
  acc_tau_m_tracks     (xDiTau) = mass_tracks (xDiTau, 0);
  acc_tau_d0_leadtrack (xDiTau) = d0_leadtrack(xDiTau, 0);


  if(!origDiTau->isAvailable<int>("n_subjets"))
    origDiTau->auxdecor<int>("n_subjets") = origDiTau->nSubjets();
  acc_f_subjets        (xDiTau) = f_subjets(*origDiTau);           
  acc_n_track          (xDiTau) = n_track(*origDiTau);               
  acc_n_isotrack       (xDiTau) = n_isotrack(*origDiTau);         
  acc_R_track          (xDiTau) = R_track(*origDiTau);               
  acc_R_track_core     (xDiTau) = R_track_core(*origDiTau);     
  acc_R_track_all      (xDiTau) = R_track_all(*origDiTau);       
  acc_R_isotrack       (xDiTau) = R_isotrack(*origDiTau);         
  acc_m_track          (xDiTau) = mass_track(*origDiTau);               
  acc_m_track_core     (xDiTau) = mass_track_core(*origDiTau);     
  acc_m_track_all      (xDiTau) = mass_track_all(*origDiTau);       

  acc_E_frac_subl      (xDiTau) = E_frac(*origDiTau,1);           
  acc_E_frac_subsubl   (xDiTau) = E_frac(*origDiTau, 2);       
  acc_R_subjets_subl   (xDiTau) = R_subjets(*origDiTau, 1);    
  acc_R_subjets_subsubl(xDiTau) = R_subjets(*origDiTau, 2); 
  acc_f_isotracks      (xDiTau) = f_isotracks(*origDiTau);          
  acc_n_iso_ellipse    (xDiTau) = n_iso_ellipse(*origDiTau);    

  acc_el_f1core          (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::f1core          );
  acc_el_f3core          (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::f3core          );
  acc_el_weta1           (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::weta1           );
  acc_el_fracs1          (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::fracs1          );
  acc_el_poscs1          (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::poscs1          );
  acc_el_poscs2          (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::poscs2          );
  acc_el_asy1            (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::asy1            );
  acc_el_pos             (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::pos             );
  acc_el_pos7            (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::pos7            );
  acc_el_barys1          (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::barys1          );
  acc_el_wtots1          (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::wtots1          );
  acc_el_r33over37allcalo(xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::r33over37allcalo);
  acc_el_isoGL           (xDiTau) = (bool)m_isoSelTool->accept(*electron);
  
  acc_E_frac_HadEl       (xDiTau) = (xDiTau.subjetPt(0) + electron->pt()) / (origDiTau->pt());

  static const SG::AuxElement::Decorator< float > acc_el_likelihood            ( "el_likelihood" );
  static const SG::AuxElement::Decorator< int >   acc_el_IDSelection           ( "el_IDSelection" );
  static const SG::AuxElement::Decorator< float > acc_tau_leadingElLikelihood  ( "tau_leadingElLikelihood" );
  static const SG::AuxElement::Decorator< float > acc_tau_leadingElDeltaR      ( "tau_leadingElDeltaR" );
  static const SG::AuxElement::Decorator< int >   acc_tau_leadingElIDSelection ( "tau_leadingElIDSelection" );

  static const SG::AuxElement::Decorator< float > acc_el_f3                    ( "el_f3" );
  static const SG::AuxElement::Decorator< float > acc_el_Rhad                  ( "el_Rhad" );
  static const SG::AuxElement::Decorator< float > acc_el_Rhad1                 ( "el_Rhad1" );
  static const SG::AuxElement::Decorator< float > acc_el_Reta                  ( "el_Reta" );
  static const SG::AuxElement::Decorator< float > acc_el_w2                    ( "el_w2" );
  static const SG::AuxElement::Decorator< float > acc_el_f1                    ( "el_f1" );
  static const SG::AuxElement::Decorator< float > acc_el_Eratio                ( "el_Eratio" );
  static const SG::AuxElement::Decorator< float > acc_el_deltaEta              ( "el_deltaEta" );
  static const SG::AuxElement::Decorator< float > acc_el_d0                    ( "el_d0" );
  static const SG::AuxElement::Decorator< float > acc_el_d0sigma               ( "el_d0sigma" );
  static const SG::AuxElement::Decorator< float > acc_el_Rphi                  ( "el_Rphi" );
  static const SG::AuxElement::Decorator< float > acc_el_dpOverp               ( "el_dpOverp" );
  static const SG::AuxElement::Decorator< float > acc_el_deltaPhiRescaled2     ( "el_deltaPhiRescaled2" );
  static const SG::AuxElement::Decorator< float > acc_el_trans_TRT_PID         ( "el_trans_TRT_PID" );

  static const SG::AuxElement::Decorator< float > acc_nSiHitsPlusDeadSensors   ( "el_nSiHitsPlusDeadSensors" );
  static const SG::AuxElement::Decorator< float > acc_nPixHitsPlusDeadSensors  ( "el_nPixHitsPlusDeadSensors" );
  static const SG::AuxElement::Decorator< float > acc_passBLayerRequirement    ( "el_passBLayerRequirement" );
        
  acc_el_f3     (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::f3); 
  acc_el_Rhad   (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::Rhad);
  acc_el_Rhad1  (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::Rhad1);
  acc_el_Reta   (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::Reta); 
  acc_el_w2     (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::weta2); 
  acc_el_f1     (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::f1); 
  acc_el_Eratio (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::Eratio);
  acc_el_Rphi   (xDiTau) = getElectronInfo(electron, xAOD::EgammaParameters::ShowerShapeType::Rphi);

  float deltaEta(0);
  if(!electron->trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1))
    ATH_MSG_WARNING("failed to retrieve deltaEta1 from electron");
  acc_el_deltaEta          (xDiTau) = deltaEta;

  float deltaPhiRescaled2(0);
  if(!electron->trackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2))
    ATH_MSG_WARNING("failed to rerieve deltaPhiRescaled2 from electron");
  acc_el_deltaPhiRescaled2 (xDiTau) = deltaPhiRescaled2;
  
  const xAOD::TrackParticle* track = electron->trackParticle();
  float d0(0);
  float trackqoverp(0);
  float d0sigma(0);
  float TRT_PID(0);
  float trans_TRT_PID(0);
  float dpOverp(0);
  uint8_t nSiHitsPlusDeadSensors(0);
  uint8_t nPixHitsPlusDeadSensors(0);
  bool passBLayerRequirement(false);
  
  if (track)
  {
    trackqoverp = track->qOverP();
    d0 = track->d0();
    float vard0 = track->definingParametersCovMatrix()(0,0);
    if (vard0 > 0) {
      d0sigma=sqrtf(vard0);
    }

    if(!track->summaryValue(TRT_PID, xAOD::eProbabilityHT))
      ATH_MSG_WARNING("Could not retrieve eProbablityHT from electron track");

    //Transform the TRT PID output for use in the LH tool.
    float tau = 15.0;
    float fEpsilon = 1.0e-30;  // to avoid zero division
    float pid_tmp = TRT_PID;
    if (pid_tmp >= 1.0) pid_tmp = 1.0 - 1.0e-15;  //this number comes from TMVA
    else if (pid_tmp <= fEpsilon) pid_tmp = fEpsilon;
    trans_TRT_PID = - log(1.0/pid_tmp - 1.0)*(1./float(tau));

    unsigned int index;
    if( track->indexOfParameterAtPosition(index, xAOD::LastMeasurement) ) {

            float refittedTrack_LMqoverp  =
              track->charge() / sqrt(std::pow(track->parameterPX(index), 2) +
                                     std::pow(track->parameterPY(index), 2) +
                                     std::pow(track->parameterPZ(index), 2));

            dpOverp = 1 - trackqoverp/(refittedTrack_LMqoverp);
    }
    else
      ATH_MSG_WARNING("Could not retrieve indexOfParameterAtPosition from electron track");

    nSiHitsPlusDeadSensors  = ElectronSelectorHelpers::numberOfSiliconHitsAndDeadSensors(track);
    nPixHitsPlusDeadSensors = ElectronSelectorHelpers::numberOfPixelHitsAndDeadSensors(track);
    passBLayerRequirement   = ElectronSelectorHelpers::passBLayerRequirement(track);
  }
  
  acc_el_d0                (xDiTau) = d0;
  acc_el_d0sigma           (xDiTau) = d0sigma;
  acc_el_dpOverp           (xDiTau) = dpOverp;
  acc_el_trans_TRT_PID     (xDiTau) = trans_TRT_PID;
  acc_nSiHitsPlusDeadSensors  (xDiTau) = nSiHitsPlusDeadSensors;
  acc_nPixHitsPlusDeadSensors (xDiTau) = nPixHitsPlusDeadSensors;
  acc_passBLayerRequirement   (xDiTau) = passBLayerRequirement;

  Root::TResult likelihood = m_electronLikeliHoodTool_medium->calculate(electron);
  acc_el_likelihood(xDiTau) = likelihood.getResult("likelihood");
  int IDSelection = 0;
  if(m_electronLikeliHoodTool_veryloose->accept(electron))
    IDSelection++;
  if(m_electronLikeliHoodTool_loose->accept(electron))
    IDSelection++;
  if(m_electronLikeliHoodTool_loose_CutBL->accept(electron))
    IDSelection++;
  if(m_electronLikeliHoodTool_medium->accept(electron))
    IDSelection++;
  if(m_electronLikeliHoodTool_tight->accept(electron))
    IDSelection++;
  acc_el_IDSelection(xDiTau) = IDSelection;

  // get leading electron in core cone of tau subjet:
  const xAOD::ElectronContainer* electrons = evtStore()->retrieve<const xAOD::ElectronContainer>("Electrons");
  TLorentzVector p4LeadElectron;
  p4LeadElectron.SetPtEtaPhiM(5000,0,0,0);
  const xAOD::Electron* leadElectron = nullptr;
  TLorentzVector p4Subjet;
  p4Subjet.SetPtEtaPhiE(xDiTau.subjetPt(0),
                        xDiTau.subjetEta(0),
                        xDiTau.subjetPhi(0),
                        xDiTau.subjetE(0));
  for(auto electron : *electrons){
    TLorentzVector p4Electron = electron->p4();
    if(p4Subjet.DeltaR(p4Electron) < 0.1){
      if(p4Electron.Pt() > p4LeadElectron.Pt()){
        p4LeadElectron = p4Electron;
        leadElectron = electron;
      }
    }
  }

  if(leadElectron){
    acc_tau_leadingElDeltaR (xDiTau) = p4LeadElectron.DeltaR(p4Subjet);
    likelihood = m_electronLikeliHoodTool_medium->calculate(leadElectron);
    acc_tau_leadingElLikelihood (xDiTau) = likelihood.getResult("likelihood");
    IDSelection = 0;
    if(m_electronLikeliHoodTool_veryloose->accept(leadElectron))
      IDSelection++;
    if(m_electronLikeliHoodTool_loose->accept(leadElectron))
      IDSelection++;
    if(m_electronLikeliHoodTool_loose_CutBL->accept(leadElectron))
      IDSelection++;
    if(m_electronLikeliHoodTool_medium->accept(leadElectron))
      IDSelection++;
    if(m_electronLikeliHoodTool_tight->accept(leadElectron))
      IDSelection++;
    acc_tau_leadingElIDSelection (xDiTau) = IDSelection;
  }
  else{
    acc_tau_leadingElDeltaR (xDiTau) = -1;
    acc_tau_leadingElIDSelection (xDiTau) = 0;
    acc_tau_leadingElLikelihood (xDiTau) = -10.0;
  }

  return StatusCode::SUCCESS;
}

int DiTauIDVarCalculator::subjetLeadElectronID(const xAOD::DiTauJet& xDiTau, int iSubjet) const {
  if(xDiTau.auxdata<int>("n_subjets") < iSubjet)
    return -1;
  
  const xAOD::ElectronContainer* electrons = evtStore()->retrieve<const xAOD::ElectronContainer>("Electrons");
  
  TLorentzVector p4LeadElectron;
  p4LeadElectron.SetPtEtaPhiM(5000,0,0,0);
  const xAOD::Electron* leadElectron = nullptr;
  
  TLorentzVector p4Subjet;
  p4Subjet.SetPtEtaPhiE(xDiTau.subjetPt(iSubjet),
                        xDiTau.subjetEta(iSubjet),
                        xDiTau.subjetPhi(iSubjet),
                        xDiTau.subjetE(iSubjet));
  
  for(auto electron : *electrons){
    TLorentzVector p4Electron = electron->p4();
    if(p4Subjet.DeltaR(p4Electron) < 0.1){
      if(p4Electron.Pt() > p4LeadElectron.Pt()){
        p4LeadElectron = p4Electron;
        leadElectron = electron;
      }
    }
  }
  
  int IDSelection = -1;
  
  if(leadElectron){
    if(m_electronLikeliHoodTool_veryloose->accept(leadElectron))
      IDSelection++;
    if(m_electronLikeliHoodTool_loose->accept(leadElectron))
      IDSelection++;
    if(m_electronLikeliHoodTool_loose_CutBL->accept(leadElectron))
      IDSelection++;
    if(m_electronLikeliHoodTool_medium->accept(leadElectron))
      IDSelection++;
    if(m_electronLikeliHoodTool_tight->accept(leadElectron))
      IDSelection++;
  }
  
  return IDSelection;
}

std::string DiTauIDVarCalculator::getDecayMode(){
  return m_sDecayChannel;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________

float DiTauIDVarCalculator::getElectronInfo(const xAOD::Electron* el, const xAOD::EgammaParameters::ShowerShapeType information){
  float val = -1111;
  if(!el->showerShapeValue(val, information))
    ATH_MSG_WARNING("Could not retrieve Electron Info with Enum:" << information);
  return val;
}

float DiTauIDVarCalculator::n_subjets(const xAOD::DiTauJet& xDiTau) const
{
  int nSubjet = 0;
  while (xDiTau.subjetPt(nSubjet) > 0. )
  {
    nSubjet++;
  }

  return nSubjet;
}


float DiTauIDVarCalculator::ditau_pt(const xAOD::DiTauJet& xDiTau) const
{
  if (xDiTau.auxdata<int>("n_subjets") < 2 ) {
    return m_dDefault;
  }

  return xDiTau.subjetPt(0)+xDiTau.subjetPt(1);
}


//______________________________________________________________________________;
float DiTauIDVarCalculator::f_core(const xAOD::DiTauJet& xDiTau, int iSubjet) const 
{
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  return xDiTau.fCore(iSubjet);
}


//______________________________________________________________________________;
float DiTauIDVarCalculator::f_subjet(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  return xDiTau.subjetPt(iSubjet) / xDiTau.pt();
}


//______________________________________________________________________________;
float DiTauIDVarCalculator::f_subjets(const xAOD::DiTauJet& xDiTau) const
{
  if (xDiTau.auxdata<int>("n_subjets") < 2 ) {
    return m_dDefault;
  }

  return (xDiTau.subjetPt(0) + xDiTau.subjetPt(1))/ xDiTau.pt();
}


//______________________________________________________________________________;
float DiTauIDVarCalculator::f_track(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
      
  TLorentzVector tlvTrack;
  TLorentzVector tlvLeadTrack;
  tlvLeadTrack.SetPtEtaPhiE( 0,0,0, 0);

  for (const auto &xTrack: xTracks) 
  { 
    if (!xTrack) 
    {
      ATH_MSG_ERROR("Could not read Track");
      continue;
    }
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    if ( tlvSubjet.DeltaR(tlvTrack) < 0.2 )
    {
      if (tlvLeadTrack.Pt() < tlvTrack.Pt()) 
      {
        tlvLeadTrack = tlvTrack;
      }
    }
  }

  return tlvLeadTrack.Pt() / tlvSubjet.Pt();
}


//______________________________________________________________________________;
float DiTauIDVarCalculator::R_max(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{ 
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
      
  TLorentzVector tlvTrack;
  TLorentzVector tlvRmaxTrack;
  double Rmax = 0;
  for (const auto &xTrack: xTracks) 
  {
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    if ( tlvSubjet.DeltaR(tlvTrack) < xDiTau.auxdata< float >("R_subjet") )
    {
      if (tlvTrack.DeltaR(tlvSubjet) > Rmax) 
      {
        Rmax = tlvTrack.DeltaR(tlvSubjet);
      }
    }
  }

  return Rmax;
}


//______________________________________________________________________________;
int DiTauIDVarCalculator::n_track(const xAOD::DiTauJet& xDiTau) const
{ 
  return xDiTau.nTracks();
}

//______________________________________________________________________________;
int DiTauIDVarCalculator::n_tracks(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  if (!xDiTau.isAvailable<std::vector<int>>("n_tracks"))
  {
    ATH_MSG_DEBUG("n_tracks decoration not available. Try with track links.");

    if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
    {
      ATH_MSG_WARNING("Track links not available. Return 0.");
      return (int)m_dDefault;
    } 

    TrackParticleLinks_t xTracks = xDiTau.trackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                            xDiTau.subjetEta(iSubjet),
                            xDiTau.subjetPhi(iSubjet),
                            xDiTau.subjetE(iSubjet) );
        
    TLorentzVector tlvTrack;
    int nTracks = 0;
    for (const auto &xTrack: xTracks) 
    { 
      tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                             (*xTrack)->eta(),
                             (*xTrack)->phi(),
                             (*xTrack)->e() );
      if ( tlvSubjet.DeltaR(tlvTrack) < 0.2 ) nTracks++;
    }

    return nTracks;
  }

  return xDiTau.auxdata<std::vector<int>>("n_tracks").at(iSubjet);

}

//______________________________________________________________________________;
int DiTauIDVarCalculator::n_isotrack(const xAOD::DiTauJet& xDiTau) const
{ 
  return xDiTau.nIsoTracks();
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::R_tracks(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }

  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }
 
  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
    
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    if ( tlvSubjet.DeltaR(tlvTrack) < 0.2 )
    {
      //ATH_MSG_DEBUG("smaller");
      R_sum += tlvSubjet.DeltaR(tlvTrack)*tlvTrack.Pt();
      pt += tlvTrack.Pt();
    }
  }
  
  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::R_core(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }
  if (iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }
 
  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
    
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    if ( tlvSubjet.DeltaR(tlvTrack) < xDiTau.auxdata< float >( "R_core" ) )
    {
      R_sum += tlvSubjet.DeltaR(tlvTrack)*tlvTrack.Pt();
      pt += tlvTrack.Pt();
    }
  }
  
  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::R_track_core(const xAOD::DiTauJet& xDiTau) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }
  if (xDiTau.auxdata<int>("n_subjets") < 2) {
    return m_dDefault;
  }
 

  for (int i = 0; i<=1; i++)
  {
  
    TrackParticleLinks_t xTracks = xDiTau.trackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(i),
                            xDiTau.subjetEta(i),
                            xDiTau.subjetPhi(i),
                            xDiTau.subjetE(i) );
      
    TLorentzVector tlvTrack;

    for (auto xTrack: xTracks) 
    { 
      tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                             (*xTrack)->eta(),
                             (*xTrack)->phi(),
                             (*xTrack)->e() );
      if ( tlvSubjet.DeltaR(tlvTrack) < xDiTau.auxdata< float >("R_core") )
      {
        //ATH_MSG_DEBUG("smaller");
        R_sum += tlvSubjet.DeltaR(tlvTrack)*tlvTrack.Pt();
        pt += tlvTrack.Pt();
      }
    }
  }
  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::R_track(const xAOD::DiTauJet& xDiTau) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }
  if (xDiTau.auxdata<int>("n_subjets") < 2) {
    return m_dDefault;
  }
 
  for (int i = 0; i<=1; i++)
  {
  
    TrackParticleLinks_t xTracks = xDiTau.trackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(i),
                            xDiTau.subjetEta(i),
                            xDiTau.subjetPhi(i),
                            xDiTau.subjetE(i) );
      
    TLorentzVector tlvTrack;

    for (auto xTrack: xTracks) 
    { 
      tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                             (*xTrack)->eta(),
                             (*xTrack)->phi(),
                             (*xTrack)->e() );

      if (tlvSubjet.DeltaR(tlvTrack) < 0.2)
      {
        R_sum += tlvSubjet.DeltaR(tlvTrack)*tlvTrack.Pt();
        pt += tlvTrack.Pt();
      }
    }
  }
  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}
//______________________________________________________________________________;
float DiTauIDVarCalculator::R_track_all(const xAOD::DiTauJet& xDiTau) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }

  for (int i = 0; i<xDiTau.auxdata<int>("n_subjets"); i++)
  {
  
    TrackParticleLinks_t xTracks = xDiTau.trackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(i),
                            xDiTau.subjetEta(i),
                            xDiTau.subjetPhi(i),
                            xDiTau.subjetE(i) );
      
    TLorentzVector tlvTrack;

    for (auto xTrack: xTracks) 
    { 
      tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                             (*xTrack)->eta(),
                             (*xTrack)->phi(),
                             (*xTrack)->e() );

      if (tlvSubjet.DeltaR(tlvTrack) <= 0.2)
      {
        R_sum += tlvSubjet.DeltaR(tlvTrack)*tlvTrack.Pt();
        pt += tlvTrack.Pt();
      }
    }
  }

  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::R_isotrack(const xAOD::DiTauJet& xDiTau) const
{
  double R_sum = 0;
  double pt = 0;

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("isoTrackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  }
  
  if (xDiTau.auxdata<int>("n_subjets") < 2) {
    return m_dDefault;
  }
 
  for (int i = 0; i<=1; i++)
  {
  
    TrackParticleLinks_t xIsoTracks = xDiTau.isoTrackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(i),
                            xDiTau.subjetEta(i),
                            xDiTau.subjetPhi(i),
                            xDiTau.subjetE(i) );
      
    TLorentzVector tlvIsoTrack;

    for (auto xIsoTrack: xIsoTracks) 
    { 
      tlvIsoTrack.SetPtEtaPhiE( (*xIsoTrack)->pt(),
                                (*xIsoTrack)->eta(),
                                (*xIsoTrack)->phi(),
                                (*xIsoTrack)->e() );

      if (tlvSubjet.DeltaR(tlvIsoTrack) < 0.4)
      {
        R_sum += tlvSubjet.DeltaR(tlvIsoTrack)*tlvIsoTrack.Pt();
        pt += tlvIsoTrack.Pt();
      }
    }
  }

  if (pt == 0)
  {
    return m_dDefault;
  }

  return R_sum / pt;
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::mass_track_core(const xAOD::DiTauJet& xDiTau) const
{

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 
    
  if (xDiTau.auxdata<int>("n_subjets") < 2) {
    return m_dDefault;
  }

  TLorentzVector tlvallTracks;

  for (int i = 0; i<=1; i++)
  {

    TrackParticleLinks_t xTracks = xDiTau.trackLinks();

    TLorentzVector tlvSubjet;
    tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(i),
                            xDiTau.subjetEta(i),
                            xDiTau.subjetPhi(i),
                            xDiTau.subjetE(i) );
    
    TLorentzVector tlvTrack;

    for (auto xTrack: xTracks) 
    { 
      tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                             (*xTrack)->eta(),
                             (*xTrack)->phi(),
                             (*xTrack)->e() );
      if ( tlvSubjet.DeltaR(tlvTrack) < xDiTau.auxdata< float >("R_core") )
      {
        //ATH_MSG_DEBUG("smaller");
        tlvallTracks += tlvTrack;
      }
    }
  }
  if (tlvallTracks.M() < 0)
  {
    return m_dDefault;
  }

  return tlvallTracks.M();
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::mass_core(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 
    
  if ( iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  TLorentzVector tlvallTracks;


  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
  
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );
    if ( tlvSubjet.DeltaR(tlvTrack) < xDiTau.auxdata< float >("R_core") )
    {
      //ATH_MSG_DEBUG("smaller");
      tlvallTracks += tlvTrack;
    }
  }
  
  if (tlvallTracks.M() < 0)
  {
    return m_dDefault;
  }

  return tlvallTracks.M();
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::mass_tracks(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 
    
  if ( iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  TLorentzVector tlvallTracks;

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
  
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );
    if ( tlvSubjet.DeltaR(tlvTrack) < 0.2 )
    {
      tlvallTracks += tlvTrack;
    }
  }
  
  if (tlvallTracks.M() < 0)
  {
    return m_dDefault;
  }

  return tlvallTracks.M();
}
//______________________________________________________________________________;
float DiTauIDVarCalculator::mass_track(const xAOD::DiTauJet& xDiTau) const
{

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 

  TLorentzVector tlvallTracks;

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();
    
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    tlvallTracks += tlvTrack;
  }
  
  if (tlvallTracks.M() < 0)
  {
    return m_dDefault;
  }
  return tlvallTracks.M();
}
//______________________________________________________________________________;
float DiTauIDVarCalculator::mass_track_all(const xAOD::DiTauJet& xDiTau) const
{

  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Link not available");
  } 

  TLorentzVector tlvallTracks;

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();
    
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    tlvallTracks += tlvTrack;
  }

  
  TrackParticleLinks_t xIsoTracks = xDiTau.isoTrackLinks();

  TLorentzVector tlvIsoTrack;

  for (auto xIsoTrack: xIsoTracks) 
  { 
    tlvIsoTrack.SetPtEtaPhiE( (*xIsoTrack)->pt(),
                             (*xIsoTrack)->eta(),
                             (*xIsoTrack)->phi(),
                             (*xIsoTrack)->e() );

    tlvallTracks += tlvIsoTrack;
  }

  if (tlvallTracks.M() < 0)
  {
    return m_dDefault;
  }

  return tlvallTracks.M();
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::E_frac(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{ 
  if ( iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  return xDiTau.subjetE(iSubjet) / xDiTau.subjetE(0);
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::R_subjets(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{

  if ( iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }
  
  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Track links not available");
  }
  
  TLorentzVector tlvLeadSubjet;
  tlvLeadSubjet.SetPtEtaPhiE( xDiTau.subjetPt(0),
                              xDiTau.subjetEta(0),
                              xDiTau.subjetPhi(0),
                              xDiTau.subjetE(0) );

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
  return tlvLeadSubjet.DeltaR(tlvSubjet);
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::d0_leadtrack(const xAOD::DiTauJet& xDiTau, int iSubjet) const
{
  double pt_leadtrk = 0;
  double d0 = m_dDefault;
  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    ATH_MSG_WARNING("Track links not available");
  } 

  if ( iSubjet < 0 || iSubjet >= xDiTau.auxdata<int>("n_subjets")) {
    return m_dDefault;
  }

  TLorentzVector tlvSubjet;
  tlvSubjet.SetPtEtaPhiE( xDiTau.subjetPt(iSubjet),
                          xDiTau.subjetEta(iSubjet),
                          xDiTau.subjetPhi(iSubjet),
                          xDiTau.subjetE(iSubjet) );
			  
  TrackParticleLinks_t xTracks = xDiTau.trackLinks();
    
  TLorentzVector tlvTrack;

  for (auto xTrack: xTracks) 
  { 
    tlvTrack.SetPtEtaPhiE( (*xTrack)->pt(),
                           (*xTrack)->eta(),
                           (*xTrack)->phi(),
                           (*xTrack)->e() );

    if (tlvTrack.DeltaR(tlvSubjet) < xDiTau.auxdata< float >("R_core")) 
    {
      if (tlvTrack.Pt() > pt_leadtrk)
      {
        pt_leadtrk = tlvTrack.Pt();
        d0 = (*xTrack)->d0();
      }
    }
  }
  return d0;
}

//______________________________________________________________________________;
float DiTauIDVarCalculator::f_isotracks(const xAOD::DiTauJet& xDiTau) const
{
  double iso_pt = 0;
  if (!xDiTau.isAvailable< TrackParticleLinks_t >("isoTrackLinks") )
  {
    ATH_MSG_WARNING("Track links not available");
  }
  
  TrackParticleLinks_t xIsoTracks = xDiTau.isoTrackLinks();

  TLorentzVector tlvIsoTrack;

  for (auto xIsoTrack: xIsoTracks) 
  { 
    tlvIsoTrack.SetPtEtaPhiE( (*xIsoTrack)->pt(),
                              (*xIsoTrack)->eta(),
                              (*xIsoTrack)->phi(),
                              (*xIsoTrack)->e() );

    iso_pt += tlvIsoTrack.Pt();
  }

  return iso_pt / xDiTau.pt();
}

//______________________________________________________________________________;
int DiTauIDVarCalculator::n_iso_ellipse(const xAOD::DiTauJet& xDiTau) const
{
  int N_iso = 0;
  
  if (!xDiTau.isAvailable< TrackParticleLinks_t >("isoTrackLinks") )
  {
    ATH_MSG_WARNING("Track links not available");
  }
  
  if ( xDiTau.auxdata<int>("n_subjets") < 2 ) 
  {
    return m_dDefault;
  }

  TLorentzVector tlvLeadSubjet;
  tlvLeadSubjet.SetPtEtaPhiE( xDiTau.subjetPt(0),
                              xDiTau.subjetEta(0),
                              xDiTau.subjetPhi(0),
                              xDiTau.subjetE(0) );

  
  TLorentzVector tlvSubleadSubjet;
  tlvSubleadSubjet.SetPtEtaPhiE( xDiTau.subjetPt(1),
                                 xDiTau.subjetEta(1),
                                 xDiTau.subjetPhi(1),
                                 xDiTau.subjetE(1) );
				 
  TrackParticleLinks_t xIsoTracks = xDiTau.isoTrackLinks();

  TLorentzVector tlvIsoTrack;

  for (auto xIsoTrack: xIsoTracks) 
  { 
    tlvIsoTrack.SetPtEtaPhiE( (*xIsoTrack)->pt(),
                              (*xIsoTrack)->eta(),
                              (*xIsoTrack)->phi(),
                              (*xIsoTrack)->e() );
			      
    if ((tlvIsoTrack.DeltaR(tlvLeadSubjet) + tlvIsoTrack.DeltaR(tlvSubleadSubjet)) <= (tlvLeadSubjet.DeltaR(tlvSubleadSubjet)+ 0.4))
    {
      N_iso += 1;
    }
  }
  return N_iso;
}
  
// ----------------------------------------------------------------------------
std::vector<PseudoJet> DiTauIDVarCalculator::getClusters (const xAOD::DiTauJet& xDiTau) const
{
  // do some checks and retrieve the antikt 10 seed jet
  if (!xDiTau.isAvailable< JetLink_t >("jetLink") )
  {
    ATH_MSG_WARNING("Jet link not available");
  }

  JetLink_t xJetLink = xDiTau.jetLink();
  if (!xJetLink.isValid())
  {
    ATH_MSG_WARNING("Jet link is not valid");
  }

  // get clusters from the jet
  const xAOD::Jet* xJet = (*xJetLink);
  const xAOD::JetConstituentVector vConst = xJet->getConstituents();
  if (vConst.size() == 0) {
    ATH_MSG_WARNING("Cluster constituents could not be retrieved from seed jet.");
  }

  // convert clusters to pseudojets
  std::vector<PseudoJet> vClusters;
  vClusters.clear();

  for (const auto *cl: vConst) 
  {
    double pt = cl->pt();
    double px = pt*cos(cl->phi());  
    double py = pt*sin(cl->phi());  
    double pz = pt*sinh(cl->eta()); 
    double e  = sqrt(px*px + py*py + pz*pz);
    PseudoJet c( px, py, pz, e);

    vClusters.push_back(c);
  }

  return vClusters;
}

// ----------------------------------------------------------------------------
int DiTauIDVarCalculator::n_antikt_subjets(std::vector<PseudoJet> vClusters)
{
  // build anti-kt-2 subjets
  AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(SelectorAbsRapMax(4.0)));
  JetDefinition subjet_def = JetDefinition(antikt_algorithm, 0.2);
  ClusterSequenceArea subjet_cs(vClusters, subjet_def, area_def); 
  std::vector<PseudoJet> vSubjet = sorted_by_pt( subjet_cs.inclusive_jets(10*1000) );

  return vSubjet.size();
}

// ----------------------------------------------------------------------------
int DiTauIDVarCalculator::n_ca_subjets(std::vector<PseudoJet> vClusters)
{
  // build anti-kt-10 fatjet
  AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(SelectorAbsRapMax(4.0)));
  JetDefinition fatjet_def = JetDefinition(antikt_algorithm, 1.0);
  ClusterSequenceArea fatjet_cs(vClusters, fatjet_def, area_def); 
  std::vector<PseudoJet> vJet = sorted_by_pt( fatjet_cs.inclusive_jets(10*1000) );

  PseudoJet jet = (*vJet.begin());

  // define subjet filter
  Filter filter(0.2, SelectorPtMin(10*1000)); 

  // filter seed jet for subjets
  PseudoJet filtered_jet = filter(jet);
  std::vector<PseudoJet> vSubjets = filtered_jet.pieces();

  return vSubjets.size();
}

// ----------------------------------------------------------------------------
void DiTauIDVarCalculator::mass_drop(const xAOD::DiTauJet& xDiTau, std::vector<PseudoJet> vClusters) const
{
  // build cambridge aachen fatjet
  AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(SelectorAbsRapMax(4.0)));
  JetDefinition fatjet_def = JetDefinition(cambridge_algorithm, 1.0);
  ClusterSequenceArea fatjet_cs(vClusters, fatjet_def, area_def); 
  std::vector<PseudoJet> vJet = sorted_by_pt( fatjet_cs.inclusive_jets(10*1000) );

  PseudoJet jet = (*vJet.begin());

  // define mass drop tagger
  MassDropTagger mdtagger(0.9, 0.01);

  PseudoJet tagged_jet = mdtagger(jet);

  double mu = m_dDefault;
  double y = m_dDefault;
  if (tagged_jet != 0)
  {
    mu = tagged_jet.structure_of<MassDropTagger>().mu();
    y = tagged_jet.structure_of<MassDropTagger>().y();
  }
  
  // check if mu = NaN, if NaN it is set to 0 
  if (mu != mu){
    mu = 0;
  }
  xDiTau.auxdecor< double >( "mu_massdrop" ) = mu;
  xDiTau.auxdecor< double >( "y_massdrop" ) = y;

}

//______________________________________________________________________________;
float DiTauIDVarCalculator::f_clusters(const xAOD::DiTauJet& xDiTau, std::vector<PseudoJet> vClusters) const
{
  double e_clust = 0;
  double e_all = 0;
  
  if ( xDiTau.auxdata<int>("n_subjets") < 2 ) {
    return m_dDefault;
  }

  double pt0 = xDiTau.subjetPt(0);
  double px0 = pt0*cos(xDiTau.subjetPhi(0));  
  double py0 = pt0*sin(xDiTau.subjetPhi(0));  
  double pz0 = pt0*sinh(xDiTau.subjetEta(0)); 
  double e0  = sqrt(px0*px0 + py0*py0 + pz0*pz0);
  PseudoJet PLeadSubjet( px0, py0, pz0, e0);

  double pt1 = xDiTau.subjetPt(1);
  double px1 = pt1*cos(xDiTau.subjetPhi(1));  
  double py1 = pt1*sin(xDiTau.subjetPhi(1));  
  double pz1 = pt1*sinh(xDiTau.subjetEta(1)); 
  double e1  = sqrt(px1*px1 + py1*py1 + pz1*pz1);
  PseudoJet PSubleadSubjet( px1, py1, pz1, e1);

  for (const auto cl: vClusters) {
    PseudoJet PCluster( cl.px(),
      cl.py(),
      cl.pz(),
      cl.e());

    e_all += PCluster.E();
    
    double r_clus_lead = PCluster.delta_R(PLeadSubjet);
    double r_clus_subl = PCluster.delta_R(PSubleadSubjet);
    double r_lead_subl = PLeadSubjet.delta_R(PSubleadSubjet);
    if (r_clus_lead>0.2 && r_clus_subl>0.2 && ((r_clus_lead + r_clus_subl) <= (r_lead_subl +0.4))){
      e_clust += PCluster.E();
    }
  }
  return e_clust/e_all;
}


//______________________________________________________________________________;
StatusCode DiTauIDVarCalculator::decorNtracks (const xAOD::DiTauJet& xDiTau)
{
  if (!xDiTau.isAvailable< TrackParticleLinks_t >("trackLinks") )
  {
    Warning("decorNtracks()", "Track links not available.");
    return StatusCode::FAILURE;
  } 

  int nSubjets = xDiTau.auxdata<int>("n_subjets");

  float Rsubjet = xDiTau.auxdata<float>("R_subjet");
  std::vector<int> nTracks(nSubjets, 0);

  TrackParticleLinks_t xTracks = xDiTau.trackLinks();
  for (const auto xTrack: xTracks)
  {
    double dRmin = 1111;
    double itrmin = -1;

    for (int i=0; i<nSubjets; ++i)
    {
      TLorentzVector tlvSubjet = TLorentzVector();
      tlvSubjet.SetPtEtaPhiE(xDiTau.subjetPt(i),
                             xDiTau.subjetEta(i),
                             xDiTau.subjetPhi(i),
                             xDiTau.subjetE(i));
      double dR = tlvSubjet.DeltaR((*xTrack)->p4());


      if ((dR < Rsubjet) && (dR < dRmin))
      {
        dRmin = dR;
        itrmin = i;
      }
    } // loop over subjets
    if (itrmin > -1) nTracks[itrmin]++;
  } // loop over tracks

  xDiTau.auxdecor< std::vector<int> >("n_tracks") = nTracks;

  return StatusCode::SUCCESS;
}
