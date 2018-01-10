/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "SUSYTools/SUSYObjDef_xAOD.h"

// For making the systematics list and looping through it
#include "PATInterfaces/SystematicsUtil.h"
#include "PATInterfaces/SystematicRegistry.h"

#ifndef XAOD_STANDALONE // For now metadata is Athena-only
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

// Need path resolver for initialize()
#include "PathResolver/PathResolver.h"

// Including all the abstract interfaces - for systematics functions
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"

#include "JetInterface/IJetSelector.h"
#include "JetResolution/IJERTool.h"
#include "JetResolution/IJERSmearingTool.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCPInterfaces/ICPJetUncertaintiesTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "JetInterface/IJetModifier.h"
#include "JetJvtEfficiency/IJetJvtEfficiency.h"

#include "AsgAnalysisInterfaces/IEfficiencyScaleFactorTool.h"
#include "EgammaAnalysisInterfaces/IEgammaCalibrationAndSmearingTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"
#include "EgammaAnalysisInterfaces/IEGammaAmbiguityTool.h"

#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"

#include "TauAnalysisTools/ITauSelectionTool.h"
#include "TauAnalysisTools/ITauSmearingTool.h"
#include "TauAnalysisTools/ITauTruthMatchingTool.h"  
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/ITauOverlappingElectronLLHDecorator.h"
#include "tauRecTools/ITauToolBase.h"

#include "EgammaAnalysisInterfaces/IAsgPhotonEfficiencyCorrectionTool.h"

#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "IsolationSelection/IIsolationCloseByCorrectionTool.h"

#include "METInterface/IMETMaker.h"
#include "METInterface/IMETSystematicsTool.h"

#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
// Required to use some functions (see header explanation)
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "PATInterfaces/IWeightTool.h"
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"
#include "AssociationUtils/IOverlapRemovalTool.h"

// For configuration -- TEnv uses THashList
#include "THashList.h"

// system includes
#include <fstream>

namespace ST {

using namespace std;

using namespace Trig;
using namespace TrigConf;
using namespace xAOD;


SUSYObjDef_xAOD::SUSYObjDef_xAOD( const std::string& name )
  : asg::AsgMetadataTool( name ),
    m_dataSource(Undefined),
    m_jetInputType(xAOD::JetInput::Uncategorized),
    m_force_noElId(false),
    m_force_noMuId(false),
    m_doTTVAsf(true),
    m_jesNPset(-99),
    m_useBtagging(false),
    m_debug(false),
    m_strictConfigCheck(false),
    m_badJetCut(""),
    m_fatJetUncConfig(""),
    m_fatJetUncVars(""),
    m_tool_init(false),
    m_subtool_init(false),
    // set dummies for configuration
    m_eleTerm(""),
    m_gammaTerm(""),
    m_tauTerm(""),
    m_jetTerm(""),
    m_muonTerm(""),
    m_outMETTerm(""),
    m_metRemoveOverlappingCaloTaggedMuons(true),
    m_metDoSetMuonJetEMScale(true),
    m_metDoMuonJetOR(true),
    m_muUncert(-99.),
    m_electronTriggerSFStringSingle(""),
    m_electronTriggerSFStringDiLepton(""),
    m_electronTriggerSFStringMixedLepton(""),
    m_eleId(""),
    m_eleIdBaseline(""),
    m_muId(static_cast<int>(xAOD::Muon::Quality(xAOD::Muon::VeryLoose))),
    m_muIdBaseline(static_cast<int>(xAOD::Muon::Quality(xAOD::Muon::VeryLoose))),
    m_photonId(""),
    m_photonIdBaseline(""),
    m_tauId(""),
    m_tauIdBaseline(""),
    m_tauIDrecalc(false),
    m_eleIso_WP(""),
    m_eleChID_WP(""),
    m_runECIS(false),
    m_photonIso_WP(""),
    m_muIso_WP(""),
    m_BtagWP(""),
    m_BtagTagger(""),
    m_BtagSystStrategy(""),
    //configurable cuts here
    m_eleBaselinePt(-99.),
    m_eleBaselineEta(-99.),
    m_eleBaselineCrackVeto(false),
    m_elePt(-99.),
    m_eleEta(-99.),
    m_eleCrackVeto(false),
    m_eled0sig(-99.),
    m_elez0(-99.),
    m_elebaselined0sig(-99.),
    m_elebaselinez0(-99.),
    //
    m_muBaselinePt(-99.),
    m_muBaselineEta(-99.),
    m_muPt(-99.),
    m_muEta(-99.),
    m_mud0sig(-99.),
    m_muz0(-99.),
    m_mubaselined0sig(-99.),
    m_mubaselinez0(-99.),
    m_murequirepassedHighPtCuts(false),
    m_muCosmicz0(-99.),
    m_muCosmicd0(-99.),
    m_badmuQoverP(-99.),
    //
    m_photonBaselinePt(-99.),
    m_photonBaselineEta(-99.),
    m_photonEta(-99.),
    m_photonPt(-99.),
    m_photonBaselineCrackVeto(true),
    m_photonCrackVeto(true),
    m_photonAllowLate(false),
    //
    m_tauPrePtCut(-99.),
    m_tauPt(-99.),
    m_tauEta(-99.),
    m_tauConfigPath(""),
    m_tauConfigPathBaseline(""),
    m_tauDoTTM(false),
    m_tauRecalcOLR(false),
    //
    m_jetPt(-99.),
    m_jetEta(-99.),
    m_jetJvt(-99.),
    m_doFwdJVT(false),
    m_fwdjetEtaMin(-99.),
    m_fwdjetPtMax(-99.),
    m_fwdjetTightOp(false),
    m_JMScalib(""),
    //
    m_orDoTau(false),
    m_orDoPhoton(false),
    m_orDoEleJet(true),
    m_orDoMuonJet(true),
    m_orDoBjet(false),
    m_orDoElBjet(true),
    m_orDoMuBjet(true),
    m_orDoTauBjet(true),
    m_orDoBoostedElectron(false),
    m_orBoostedElectronC1(-999.), // set to positive value to activate
    m_orBoostedElectronC2(-999.), // set to positive value to activate
    m_orBoostedElectronMaxConeSize(-999.), // set to positive value to activate
    m_orDoBoostedMuon(true),
    m_orBoostedMuonC1(-999.), // set to positive value to activate
    m_orBoostedMuonC2(-999.), // set to positive value to activate
    m_orBoostedMuonMaxConeSize(-999.), // set to positive value to activate
    m_orApplyRelPt(false),
    m_orMuJetPtRatio(-999.),
    m_orMuJetTrkPtRatio(-999.),
    m_orMuJetInnerDR(-999),
    m_orDoMuonJetGhostAssociation(true),
    m_orRemoveCaloMuons(true),
    m_orBtagWP(""),
    m_orInputLabel(""),
    m_orDoFatjets(false),
    m_EleFatJetDR(-999.),
    m_JetFatJetDR(-999.),
    m_doIsoSignal(true),
    m_doElIsoSignal(true),
    m_doPhIsoSignal(true),
    m_doMuIsoSignal(true),
    m_doIsoCloseByOR(true),
    m_metJetSelection(""),
    //
    m_currentSyst(),
    m_EG_corrModel(""),
    m_applyJVTCut(true),
    //
    // set toolhandles empty by default
    m_jetCalibTool(""),
    m_jetFatCalibTool(""),
    m_jerTool(""),
    m_jerSmearingTool(""),
    m_jetUncertaintiesTool(""),
    m_fatjetUncertaintiesTool(""),
    m_jetCleaningTool(""),
    m_jetJvtUpdateTool(""),
    m_jetFwdJvtTool(""),
    m_jetJvtEfficiencyTool(""),
    //
    m_WTaggerTool(""),
    m_ZTaggerTool(""),
    //
    m_muonSelectionTool(""),
    m_muonSelectionHighPtTool(""),
    m_muonSelectionToolBaseline(""),
    m_muonCalibrationAndSmearingTool(""),
    m_muonEfficiencySFTool(""),
    m_muonEfficiencyBMHighPtSFTool(""),
    m_muonTTVAEfficiencySFTool(""),
    m_muonIsolationSFTool(""),
    m_muonTriggerSFTool(""),
    //
    m_elecEfficiencySFTool_reco(""),
    m_elecEfficiencySFTool_id(""),
    m_elecEfficiencySFTool_trig_singleLep(""),
    m_elecEfficiencySFTool_trig_diLep(""),
    m_elecEfficiencySFTool_trig_mixLep(""),
    m_elecEfficiencySFTool_iso(""),
    m_elecEfficiencySFTool_chf(""),
    //
    m_elecEfficiencySFTool_trigEff_singleLep(""),
    m_elecEfficiencySFTool_trigEff_diLep(""),
    m_elecEfficiencySFTool_trigEff_mixLep(""),
    //
    m_egammaCalibTool(""),
    m_elecSelLikelihood(""),
    m_elecSelLikelihoodBaseline(""),
    m_photonSelIsEM(""),
    m_photonSelIsEMBaseline(""),
    m_photonEfficiencySFTool(""),
    m_photonIsolationSFTool(""),
    m_electronPhotonShowerShapeFudgeTool(""),
    m_egammaAmbiguityTool(""),
    m_elecChargeIDSelectorTool(""),
    m_elecChargeEffCorrTool(""),
    //
    m_tauSelTool(""),
    m_tauSelToolBaseline(""),
    m_tauSmearingTool(""),
    m_tauTruthMatch(""),
    m_tauEffTool(""),
    m_tauTrigEffTool0(""),
    m_tauTrigEffTool1(""),
    m_tauTrigEffTool2(""),
    m_tauTrigEffTool3(""),
    m_tauTrigEffTool4(""),
    m_tauElORdecorator(""),
    //
    m_btagEffTool(""),
    m_btagSelTool(""),
    m_btagSelTool_OR(""),
    //
    m_metMaker(""),
    m_metSystTool(""),
    //
    m_trigConfTool(""),
    m_trigDecTool(""),
    m_trigMatchingTool(""),
    //
    m_isoCorrTool(""),
    m_isoTool(""),
    //
    m_prwTool(""),
    //
    m_orToolbox("ORToolbox",this),
    //
    m_pmgSHnjetWeighter(""),
    m_pmgSHnjetWeighterWZ("")
{
  //General settings
  declareProperty( "DataSource", m_dataSource = Undefined );
  declareProperty( "DebugMode",  m_debug );
  declareProperty( "ConfigFile",  m_configFile = "SUSYTools/SUSYTools_Default.conf" );
  //Overlap Removal
  declareProperty( "DoTauOR",       m_orDoTau );
  declareProperty( "DoPhotonOR",    m_orDoPhoton );
  declareProperty( "DoEleJetOR",    m_orDoEleJet );
  declareProperty( "DoMuonJetOR",   m_orDoMuonJet );
  declareProperty( "DoBjetOR",      m_orDoBjet );
  declareProperty( "DoElBjetOR",    m_orDoElBjet );
  declareProperty( "DoMuBjetOR",    m_orDoMuBjet );
  declareProperty( "DoTauBjetOR",    m_orDoTauBjet );
  declareProperty( "UseBtagging",   m_useBtagging );
  declareProperty( "DoBoostedElectronOR", m_orDoBoostedElectron );
  declareProperty( "BoostedElectronORC1", m_orBoostedElectronC1 );
  declareProperty( "BoostedElectronORC2", m_orBoostedElectronC2 );
  declareProperty( "BoostedElectronORMaxConeSize", m_orBoostedElectronMaxConeSize );
  declareProperty( "DoBoostedMuonOR", m_orDoBoostedMuon );
  declareProperty( "BoostedMuonORC1", m_orBoostedMuonC1 );
  declareProperty( "BoostedMuonORC2", m_orBoostedMuonC2 );
  declareProperty( "BoostedMuonORMaxConeSize", m_orBoostedMuonMaxConeSize );
  declareProperty( "ORDoMuonJetGhostAssociation", m_orDoMuonJetGhostAssociation );
  declareProperty( "ORRemoveCaloMuons", m_orRemoveCaloMuons );
  declareProperty( "ORMuJetApplyRelPt", m_orApplyRelPt);
  declareProperty( "ORMuJetPtRatio", m_orMuJetPtRatio);
  declareProperty( "ORMuJetInnerDR", m_orMuJetInnerDR );
  declareProperty( "ORJetTrkPtRatio", m_orMuJetTrkPtRatio);
  declareProperty( "ORInputLabel", m_orInputLabel);

  declareProperty( "DoFatJetOR", m_orDoFatjets);
  declareProperty( "OREleFatJetDR", m_EleFatJetDR);
  declareProperty( "ORJetFatJetDR", m_JetFatJetDR);


  //--- Object definitions
  //MET
  declareProperty( "METEleTerm",     m_eleTerm   );
  declareProperty( "METGammaTerm",   m_gammaTerm );
  declareProperty( "METTauTerm",     m_tauTerm   );
  declareProperty( "METJetTerm",     m_jetTerm   );
  declareProperty( "METMuonTerm",    m_muonTerm  );
  declareProperty( "METOutputTerm",  m_outMETTerm );
  declareProperty( "METDoTrkSyst",   m_trkMETsyst  );
  declareProperty( "METDoCaloSyst",  m_caloMETsyst );
  declareProperty( "METJetSelection",  m_metJetSelection );

  declareProperty( "METRemoveORCaloTaggedMuons", m_metRemoveOverlappingCaloTaggedMuons);
  declareProperty( "METDoSetMuonJetEMScale", m_metDoSetMuonJetEMScale);
  declareProperty( "METDoMuonJetOR",  m_metDoMuonJetOR );

  declareProperty( "METGreedyPhotons",  m_metGreedyPhotons );

  //JETS
  declareProperty( "FwdJetDoJVT",  m_doFwdJVT );
  declareProperty( "FwdJetUseTightOP",  m_fwdjetTightOp );

  declareProperty( "JetJMSCalib",  m_JMScalib );

  //BTAGGING
  declareProperty( "BtagTagger", m_BtagTagger); 
  declareProperty( "BtagWPOR", m_orBtagWP); //the one used in the Overlap Removal
  declareProperty( "BtagWP", m_BtagWP);     //the one used in FillJet() afterwards
  declareProperty( "BtagCalibPath", m_bTaggingCalibrationFilePath); 

  //ELECTRONS
  declareProperty( "EleBaselinePt", m_eleBaselinePt);
  declareProperty( "ElePt", m_elePt);
  declareProperty( "EleBaselineEta", m_eleBaselineEta);
  declareProperty( "EleEta", m_eleEta);
  declareProperty( "EleBaselineId", m_eleIdBaseline);
  declareProperty( "EleBaselineCrackVeto", m_eleBaselineCrackVeto);
  declareProperty( "EleId", m_eleId);
  declareProperty( "EleIso", m_eleIso_WP);
  declareProperty( "EleCFT", m_eleChID_WP);
  declareProperty( "EleD0sig", m_eled0sig);
  declareProperty( "EleZ0", m_elez0);
  declareProperty( "EleBaselineD0sig", m_elebaselined0sig);
  declareProperty( "EleBaselineZ0", m_elebaselinez0);
  declareProperty( "EleCrackVeto", m_eleCrackVeto);

  declareProperty( "EleForceNoId", m_force_noElId );

  //MUONS
  declareProperty( "MuonBaselinePt", m_muBaselinePt);
  declareProperty( "MuonPt", m_muPt);
  declareProperty( "MuonBaselineEta", m_muBaselineEta);
  declareProperty( "MuonEta", m_muEta);
  declareProperty( "MuonBaselineId", m_muIdBaseline);
  declareProperty( "MuonId", m_muId);
  declareProperty( "MuonIso", m_muIso_WP);
  declareProperty( "MuonD0sig", m_mud0sig);
  declareProperty( "MuonZ0", m_muz0);
  declareProperty( "MuonBaselineD0sig", m_mubaselined0sig);
  declareProperty( "MuonBaselineZ0", m_mubaselinez0);
  declareProperty( "MuonRequireHighPtCuts",  m_murequirepassedHighPtCuts);
  declareProperty( "MuonForceNoId", m_force_noMuId );
  declareProperty( "MuonTTVASF", m_doTTVAsf );

  //PHOTONS
  declareProperty( "PhotonBaselinePt", m_photonBaselinePt);
  declareProperty( "PhotonPt", m_photonPt);
  declareProperty( "PhotonBaselineEta", m_photonBaselineEta);
  declareProperty( "PhotonEta", m_photonEta);
  declareProperty( "PhotonBaselineId", m_photonIdBaseline);
  declareProperty( "PhotonId", m_photonId);
  declareProperty( "PhotonIso", m_photonIso_WP);
  declareProperty( "PhotonBaselineCrackVeto", m_photonBaselineCrackVeto);
  declareProperty( "PhotonCrackVeto", m_photonCrackVeto);
  declareProperty( "PhotonAllowLate", m_photonAllowLate);

  //TAUS
  declareProperty( "TauBaselineId", m_tauIdBaseline);
  declareProperty( "TauId", m_tauId);
  declareProperty( "TauIdConfigPathBaseline", m_tauConfigPathBaseline);
  declareProperty( "TauIdConfigPath", m_tauConfigPath);
  declareProperty( "TauMVACalibration", m_tauMVACalib);
  declareProperty( "TauDoTruthMatching", m_tauDoTTM);
  declareProperty( "TauRecalcElOLR", m_tauRecalcOLR);
  declareProperty( "TauIDRedecorate", m_tauIDrecalc); 

  //Leptons
  declareProperty( "SigLepRequireIso", m_doIsoSignal ); //leave here for back-compatibility
  declareProperty( "SigElRequireIso", m_doElIsoSignal ); 
  declareProperty( "SigMuRequireIso", m_doMuIsoSignal ); 
  declareProperty( "SigPhRequireIso", m_doPhIsoSignal ); 

  //Isolation correction for leptons and photons
  declareProperty( "SigLepPhIsoCloseByOR", m_doIsoCloseByOR );

  //--- Tools configuration
  //PRW
  declareProperty( "PRWConfigFiles",    m_prwConfFiles );
  declareProperty( "PRWLumiCalcFiles",  m_prwLcalcFiles );
  declareProperty( "PRWMuUncertainty",  m_muUncert); // = 0.2);
  //JES Unc.
  declareProperty( "JESNuisanceParameterSet", m_jesNPset );
  //LargeR uncertainties config, as from https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/JetUncertainties2016PrerecLargeR#Understanding_which_configuratio
  declareProperty( "JetLargeRuncConfig",  m_fatJetUncConfig );
  declareProperty( "JetLargeRuncVars",  m_fatJetUncVars );
  //Btagging MCtoMC SFs
  declareProperty( "ShowerType",    m_showerType = 0 );
  //Egamma NP correlation model
  declareProperty( "ElEffNPcorrModel", m_EG_corrModel );

  //For electron trigger SF tools
  declareProperty( "ElectronTriggerSFStringSingle",      m_electronTriggerSFStringSingle);
  declareProperty( "ElectronTriggerSFStringDi",          m_electronTriggerSFStringDiLepton);
  declareProperty( "ElectronTriggerSFStringMixedLepton", m_electronTriggerSFStringMixedLepton);

  //--- Tool pointers    /MT : we may want to improve the descriptions :)
  m_jetCalibTool.declarePropertyFor( this, "JetCalibTool", "The JetCalibTool" );
  m_jetFatCalibTool.declarePropertyFor( this, "FatJetCalibTool", "The JetCalibTool for large-R jets" );
  m_jerTool.declarePropertyFor( this, "JERTool", "The JERTool" );
  m_jerSmearingTool.declarePropertyFor( this, "JERSmearingTool", "The JERSmearingTool" );
  m_jetUncertaintiesTool.declarePropertyFor( this, "JetUncertaintiesTool", "The JetUncertaintiesTool" );
  m_fatjetUncertaintiesTool.declarePropertyFor( this, "FatJetUncertaintiesTool", "The JetUncertaintiesTool for large-R jets" );
  m_jetCleaningTool.declarePropertyFor( this, "JetCleaningTool", "The JetCleaningTool" );
  m_jetJvtUpdateTool.declarePropertyFor( this, "JetJvtUpdateTool", "The JetJvtUpdateTool" );
  m_jetJvtEfficiencyTool.declarePropertyFor( this, "JetJvtEfficiencyTool", "The JetJvtEfficiencyTool" );
  m_jetFwdJvtTool.declarePropertyFor( this, "JetFwdJvtEfficiencyTool", "The JetFwdJvtTool" );
  //
  m_muonSelectionTool.declarePropertyFor( this, "MuonSelectionTool", "The MuonSelectionTool for signal muons" );
  m_muonSelectionHighPtTool.declarePropertyFor( this, "MuonSelectionHighPtTool", "The MuonSelectionTool for signal muons (HighPt WP)" );
  m_muonSelectionToolBaseline.declarePropertyFor( this, "MuonSelectionToolBaseline", "The MuonSelectionTool for baseline muons" );
  m_muonCalibrationAndSmearingTool.declarePropertyFor( this, "MuonCalibrationAndSmearingTool", "The MuonCalibrationAndSmearingTool" );
  m_muonEfficiencySFTool.declarePropertyFor( this, "MuonEfficiencyScaleFactorsTool", "The MuonEfficiencySFTool" );
  m_muonEfficiencyBMHighPtSFTool.declarePropertyFor( this, "MuonBadMuonHighPtScaleFactorsTool", "The MuonBadMuonHighPtSFTool" );
  m_muonTTVAEfficiencySFTool.declarePropertyFor( this, "MuonTTVAEfficiencyScaleFactorsTool", "The MuonTTVAEfficiencySFTool" );
  m_muonIsolationSFTool.declarePropertyFor( this, "MuonIsolationScaleFactorsTool", "The MuonIsolationSFTool" );
  m_muonTriggerSFTool.declarePropertyFor( this, "MuonTriggerScaleFactorsTool", "The MuonTriggerSFTool" );
  //
  m_elecEfficiencySFTool_reco.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_reco", "The ElectronEfficiencyCorrectionTool for reconstruction SFs" );
  m_elecEfficiencySFTool_trig_singleLep.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_trig_singleLep", "The ElectronEfficiencyCorrectionTool for single-e triggers" );
  m_elecEfficiencySFTool_trig_diLep.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_trig_diLep", "The ElectronEfficiencyCorrectionTool for di-e triggers" );
  m_elecEfficiencySFTool_trig_mixLep.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_trig_mixLep", "The ElectronEfficiencyCorrectionTool for mixed e-mu triggers" );


  m_elecEfficiencySFTool_id.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_id", "The ElectronEfficiencyCorrectionTool for ID SFs" );
  m_elecEfficiencySFTool_iso.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_iso" , "The ElectronEfficiencyCorrectionTool for iso SFs" );
  m_elecEfficiencySFTool_chf.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_chf" , "The ElectronEfficiencyCorrectionTool for charge-flip SFs" );
  m_elecSelLikelihood.declarePropertyFor( this, "ElectronLikelihoodTool" , "The ElectronSelLikelihoodTool for signal electrons" );
  m_elecSelLikelihoodBaseline.declarePropertyFor( this, "ElectronLikelihoodToolBaseline" , "The ElectronSelLikelihoodTool for baseline electrons" );
  m_egammaAmbiguityTool.declarePropertyFor( this, "EgammaAmbiguityTool", "The EgammaAmbiguityTool" );
  m_elecChargeIDSelectorTool.declarePropertyFor( this, "ElectronChargeIDSelectorTool", "The ElectronChargeIDSelectorTool" );
  m_elecChargeEffCorrTool.declarePropertyFor( this, "ElectronChargeEffCorrectionTool", "The ElectronChargeEffCorrectionTool" );
  //
  m_photonSelIsEM.declarePropertyFor( this, "PhotonIsEMSelector" , "The PhotonIsEMSelectorTool for signal photons" );
  m_photonSelIsEMBaseline.declarePropertyFor( this, "PhotonIsEMSelectorBaseline" , "The PhotonIsEMSelectorTool for baseline photons" );
  m_photonEfficiencySFTool.declarePropertyFor( this, "PhotonEfficiencyCorrectionTool", "The PhotonEfficiencyCorrectionTool for reco SFs" );
  m_photonIsolationSFTool.declarePropertyFor( this, "PhotonIsolationCorrectionTool", "The PhotonEfficiencyCorrectionTool for iso SFs" );
  m_electronPhotonShowerShapeFudgeTool.declarePropertyFor( this, "PhotonShowerShapeFudgeTool", "The ElectronPhotonShowerShapeFudgeTool" );
  //
  m_egammaCalibTool.declarePropertyFor( this, "EgammaCalibrationAndSmearingTool", "The EgammaCalibrationAndSmearingTool");
  //
  m_tauSelTool.declarePropertyFor( this, "TauSelectionTool", "The TauSelectionTool for signal taus" );
  m_tauSelToolBaseline.declarePropertyFor( this, "TauSelectionToolBaseline", "The TauSelectionTool for baseline taus" );
  m_tauSmearingTool.declarePropertyFor( this, "TauSmearingTool", "The TauSmearingTool" );
  m_tauTruthMatch.declarePropertyFor( this, "TauTruthMatch", "The TTMT" );
  m_tauEffTool.declarePropertyFor( this, "TauEfficiencyCorrectionsTool", "The TauEfficiencyCorrectionsTool" );
  m_tauTrigEffTool0.declarePropertyFor( this, "TauTrigEfficiencyCorrectionsTool0", "The TauEfficiencyCorrectionsTool for trigger 0" );
  m_tauTrigEffTool1.declarePropertyFor( this, "TauTrigEfficiencyCorrectionsTool1", "The TauEfficiencyCorrectionsTool for trigger 1" );
  m_tauTrigEffTool2.declarePropertyFor( this, "TauTrigEfficiencyCorrectionsTool2", "The TauEfficiencyCorrectionsTool for trigger 2" );
  m_tauTrigEffTool3.declarePropertyFor( this, "TauTrigEfficiencyCorrectionsTool3", "The TauEfficiencyCorrectionsTool for trigger 3" );
  m_tauTrigEffTool4.declarePropertyFor( this, "TauTrigEfficiencyCorrectionsTool4", "The TauEfficiencyCorrectionsTool for trigger 4" );
  m_tauElORdecorator.declarePropertyFor( this, "TauOverlappingElectronLLHDecorator", "The TauOverlappingElectronLLHDecorator tool" );
  //
  m_btagEffTool.declarePropertyFor( this, "BTaggingEfficiencyTool", "The BTaggingEfficiencyTool" );
  m_btagSelTool.declarePropertyFor( this, "BTaggingSelectionTool", "The main BTaggingSelectionTool" );
  m_btagSelTool_OR.declarePropertyFor( this, "BTaggingSelectionTool_OR", "The BTaggingSelectionTool used to select b-jets for overlap removal" );
  //
  m_metMaker.declarePropertyFor( this, "METMaker", "The METMaker instance");
  m_metSystTool.declarePropertyFor( this, "METSystTool", "The METSystematicsTool");
  //
  m_trigConfTool.declarePropertyFor( this, "TrigConfigTool", "The TrigConfigTool" );
  m_trigDecTool.declarePropertyFor( this, "TrigDecisionTool", "The TrigDecisionTool" );
  m_trigMatchingTool.declarePropertyFor( this, "TrigMatchTool", "The TrigMatchingTool" );
  //
  m_isoCorrTool.declarePropertyFor( this, "IsolationCorrectionTool", "The IsolationCorrectionTool" );
  m_isoTool.declarePropertyFor( this, "IsolationSelectionTool", "The IsolationSelectionTool");
  m_isoCloseByTool.declarePropertyFor( this, "IsolationCloseByCorrectionTool", "The IsolationCloseByCorrectionTool");
  //
  m_prwTool.declarePropertyFor( this, "PileupReweightingTool", "The PRW tool" );
  //
  m_pmgSHnjetWeighter.declarePropertyFor( this, "PMGSHVjetReweightTool", "The PMGSHVjetReweightTool (AntiKt4TruthJets)" );
  m_pmgSHnjetWeighterWZ.declarePropertyFor( this, "PMGSHVjetReweightWZTool", "The PMGSHVjetReweightTool (AntiKt4TruthWZJets)" );
  //
  m_tauJetORtool.declarePropertyFor( this, "TauJetOverlapTool", "The TauJetOverlapTool");
  //m_orToolbox.declarePropertyFor( this, "OverlapRemovalTool", "The overlap removal tool");

  //load supported WPs (by tightness order)
  el_id_support.push_back("VeryLooseLLH_Rel20p7");
  el_id_support.push_back("VeryLooseLLH");
  el_id_support.push_back("LooseLLH_Rel20p7");
  el_id_support.push_back("LooseLLH");
  el_id_support.push_back("LooseAndBLayerLLH_Rel20p7"); 
  el_id_support.push_back("LooseAndBLayerLLH");
  el_id_support.push_back("MediumLLH_Rel20p7"); 
  el_id_support.push_back("MediumLLH");
  el_id_support.push_back("TightLLH_Rel20p7");
  el_id_support.push_back("TightLLH");

  ph_id_support.push_back("Loose");
  ph_id_support.push_back("Medium");
  ph_id_support.push_back("Tight");

  mu_id_support = 4; //maximum value supported

  //load tau trigger support
  // https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauEfficiencyCorrectionsTool_Trigger.rst#supported-tau-trigger
  tau_trig_support.push_back("HLT_tau25_medium1_tracktwo");
  tau_trig_support.push_back("HLT_tau35_medium1_tracktwo");
  tau_trig_support.push_back("HLT_tau50_medium1_tracktwo_L1TAU12");
  tau_trig_support.push_back("HLT_tau80_medium1_tracktwo");
  tau_trig_support.push_back("HLT_tau125_medium1_tracktwo");

}

#define CHECK_TOOL_RETRIEVE( TOOLHANDLE )         \
  ATH_MSG_VERBOSE("Try to retrieve " << #TOOLHANDLE);       \
  if( TOOLHANDLE.retrieve().isFailure()) {          \
    ATH_MSG_ERROR("Failed to retrieve tool " << #TOOLHANDLE); \
    return StatusCode::FAILURE;             \
  }

#define CHECK_TOOL_RETRIEVE_NONEMPTY( TOOLHANDLE )      \
  CHECK_TOOL_RETRIEVE_CONDITIONAL( TOOLHANDLE, !TOOLHANDLE.empty())

#define CHECK_TOOL_RETRIEVE_CONDITIONAL( TOOLHANDLE, CONDITION )  \
  if( CONDITION) {              \
    ATH_MSG_VERBOSE("Try to retrieve " << #TOOLHANDLE);     \
    if(TOOLHANDLE.retrieve().isFailure()) {       \
      ATH_MSG_ERROR("Failed to retrieve tool " << #TOOLHANDLE);   \
      return StatusCode::FAILURE;         \
    }                 \
  }

//--- override AsgTools behaviour for HyperBool properties
StatusCode SUSYObjDef_xAOD::setBoolProperty(const std::string& name, const bool& property){
  m_bool_prop_set.insert(name);
  return this->setProperty(name, property);
}
//---

StatusCode SUSYObjDef_xAOD::initialize() {

  if (m_tool_init) {
    ATH_MSG_ERROR("Initialising for a second time -- something is wrong!");
    return StatusCode::FAILURE;
  }

  // Greet the user:
  ATH_MSG_INFO( "Initialising... " );

  bool autoconf(false);
#ifndef XAOD_STANDALONE // For now metadata is Athena-only
  if ( m_dataSource < 0 ) {
    bool local_isData(false), local_isAtlfast(false);
    autoconf = true;
    ATH_MSG_INFO("Autoconfigure isData and isAtlfast");
    std::string projectName = "";
    ATH_CHECK( AthAnalysisHelper::retrieveMetadata("/TagInfo", "project_name", projectName, inputMetaStore() ) );
    if ( projectName == "IS_SIMULATION" ) local_isData = false;
    else if (projectName.compare(0, 4, "data") == 0 ) local_isData = true;
    else {
      ATH_MSG_ERROR("Failed to autoconfigure -- project_name matches neither IS_SIMULATION nor data!");
      return StatusCode::FAILURE;
    }

    if (!local_isData) {
      std::string simFlavour = "";
      ATH_CHECK( AthAnalysisHelper::retrieveMetadata("/Simulation/Parameters", "SimulationFlavour", simFlavour, inputMetaStore() ) );
      boost::to_upper(simFlavour);
      local_isAtlfast = (simFlavour.find("ATLFASTII") != std::string::npos);
    }
    if (local_isData) {m_dataSource = Data;}
    else {
      if (local_isAtlfast) {m_dataSource = AtlfastII;}
      else {m_dataSource = FullSim;}
    }
  }
#endif

  m_configFile = (PathResolverFindCalibFile(m_configFile)).c_str();

  //Read configuration from file (stored as Property)
  ATH_CHECK( readConfig() );

  if (m_dataSource < 0) {
    ATH_MSG_FATAL( "Data source incorrectly configured!!" );
    ATH_MSG_FATAL("You must set the DataSource property to Data, FullSim or AtlfastII !!");
    if (autoconf) ATH_MSG_FATAL("Autoconfiguration seems to have failed!");
    // if(m_useLeptonTrigger<0) ATH_MSG_ERROR( " UseLeptonTrigger not set");
    ATH_MSG_FATAL( "Exiting... " );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Configured: IsData: " << isData() );
  ATH_MSG_DEBUG( "Configured: IsAtlfast: " << isAtlfast() );

  switch (m_jetInputType) {
  case xAOD::JetInput::LCTopo:
  case xAOD::JetInput::EMTopo:
  case xAOD::JetInput::EMPFlow:
    break;
  default:
    ATH_MSG_ERROR( "Invalid jet input type " << xAOD::JetInput::typeName(xAOD::JetInput::Type(m_jetInputType)) );
    return StatusCode::FAILURE;
  }

  m_inputMETSuffix = "AntiKt4" + xAOD::JetInput::typeName(xAOD::JetInput::Type(m_jetInputType));
  m_defaultJets = m_inputMETSuffix + "Jets";
  ATH_MSG_INFO( "Configured for jet collection " << m_defaultJets );

  m_inputMETCore = "MET_Core_" + m_inputMETSuffix;
  m_inputMETMap = "METAssoc_" + m_inputMETSuffix;
  ATH_MSG_INFO("Build MET with map: " << m_inputMETMap);

  ATH_CHECK( this->SUSYToolsInit() );

  ATH_MSG_VERBOSE("Done with tool retrieval");

  m_tool_init = true;

  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode SUSYObjDef_xAOD::autoconfigurePileupRWTool() {
  // autoconfigure PRW tool using run number and DSID
  // based on: https://twiki.cern.ch/twiki/bin/view/AtlasComputing/ConditionsRun1RunNumbers
  if ( !m_prwTool.empty() && !isData() ) {
    const xAOD::EventInfo* evtInfo = 0;
    ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
    uint32_t runNum = evtInfo->runNumber();
    uint32_t dsid = evtInfo->mcChannelNumber();
    TString prwConfigFile = PathResolverFindCalibDirectory("dev/SUSYTools/PRW_AUTOCONGIF/files/");
    prwConfigFile += "pileup_mc16";
    switch(runNum) {
      case 284500 : prwConfigFile+="a_"; 
	break;
      case 300000 : prwConfigFile+="c_"; 
	break;
	//  case XXXXXX : prwConfigFile+="d_";
	//    break;
      default : ATH_MSG_ERROR( "autoconfigurePileupRWTool(): unrecognized MC run number, " << runNum << "! Impossible to autocongigure PRW. Aborting." );
	return StatusCode::FAILURE;
	break;
    }
    prwConfigFile += "dsid" + std::to_string(dsid) + ".root";
    TFile testF(prwConfigFile,"read");
    if(testF.IsZombie()) {
      ATH_MSG_ERROR( "autoconfigurePileupRWTool(): file not found -> " << prwConfigFile );
      return StatusCode::FAILURE;
    }
    ATH_CHECK( m_prwTool.setProperty("ConfigFiles", prwConfigFile) );
    ATH_MSG_INFO( "autoconfigurePileupRWTool(): configured PRW tool with using " << prwConfigFile );
  }
  // Return gracefully
  return StatusCode::SUCCESS;
}
  
void SUSYObjDef_xAOD::setDataSource(int source) {
  if (source == 0) m_dataSource = Data;
  else if (source == 1) m_dataSource = FullSim;
  else if (source == 2) m_dataSource = AtlfastII;
  else ATH_MSG_ERROR("Trying to set data source to unknown type (" << source << ")");
}


bool SUSYObjDef_xAOD::check_isOption(const std::string& wp, const std::vector<std::string>& list) const {
  //check if the selected WP is supported
  return (std::find(list.begin(), list.end(),wp)  != list.end());
}

bool SUSYObjDef_xAOD::check_isTighter(const std::string& wp1, const std::string& wp2, const std::vector<std::string>& list) const {
  //check if WP1 is tighter than WP2
  //it is assumed that both WPs are supported
  return (std::find(list.begin(),list.end(),wp1) > std::find(list.begin(), list.end(),wp2)); 
}


std::string SUSYObjDef_xAOD::EG_WP(const std::string& wp) const {
  //translate our electron wps to EGamma internal jargon
  //@ElectronPhotonSelectorTools/EGSelectorConfigurationMapping.h
  return TString(wp).Copy().ReplaceAll("AndBLayer","BL").ReplaceAll("LLH","LHElectron").Data();
}

std::vector<std::string> SUSYObjDef_xAOD::getElSFkeys(const std::string& mapFile) const {
  
  if( mapFile.empty() )
    return {};

  std::vector<std::string> theKeys;

  std::string filename = PathResolverFindCalibFile(mapFile);
  std::ifstream input( filename );
  for( std::string line; getline( input, line ); ){
    std::vector<std::string> tokens = split((const std::string)line,"=");
    if(tokens.size()){
      theKeys.push_back(tokens.at(0)); 
    }
  }
    
  return theKeys;
}

void SUSYObjDef_xAOD::configFromFile(bool& property, const std::string& propname, TEnv& rEnv,
                                     bool defaultValue)
{
  if(m_bool_prop_set.find(m_conf_to_prop[propname])!=m_bool_prop_set.end()){
    ATH_MSG_INFO( "configFromFile(): property \"" << propname << "\" already set with value " << property << ". Ignoring change request." );
    rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject(propname.c_str() ) );
    return;
  }
  property = rEnv.GetValue(propname.c_str(), (int) defaultValue);
  ATH_MSG_INFO( "configFromFile(): Loaded property \"" << propname << "\" with value " << property );
  // Remove the item from the table
  rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject(propname.c_str() ) );
}


void SUSYObjDef_xAOD::configFromFile(double& property, const std::string& propname, TEnv& rEnv,
                                     double defaultValue)
{
  // ignore if already configured
  if (property > -90.) { 
    ATH_MSG_INFO( "configFromFile(): property \"" << propname << "\" already set with value " << property << ". Ignoring change request." );
    rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject(propname.c_str() ) );
    return;
  }
  property = rEnv.GetValue(propname.c_str(), defaultValue);
  ATH_MSG_INFO( "configFromFile(): Loaded property \"" << propname << "\" with value " << property );
  // Remove the item from the table
  rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject(propname.c_str() ) );
}


void SUSYObjDef_xAOD::configFromFile(int& property, const std::string& propname, TEnv& rEnv,
                                     int defaultValue)
{
  // ignore if already configured
  if (property > -90){
    ATH_MSG_INFO( "configFromFile(): property \"" << propname << "\" already set with value " << property << ". Ignoring change request." );
    rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject(propname.c_str() ) );
    return;
  }
  property = rEnv.GetValue(propname.c_str(), defaultValue);
  ATH_MSG_INFO( "configFromFile(): Loaded property \"" << propname << "\" with value " << property );
  // Remove the item from the table
  rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject(propname.c_str() ) );
}


void SUSYObjDef_xAOD::configFromFile(std::string& property, const std::string& propname, TEnv& rEnv,
                                     const std::string& defaultValue)
{
  // ignore if already configured
  if (!property.empty()){
    ATH_MSG_INFO( "configFromFile(): property \"" << propname << "\" already set with value " << property << ". Ignoring change request." );
    rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject(propname.c_str() ) );
    return;
  }
  property = rEnv.GetValue(propname.c_str(), defaultValue.c_str());
  if (property.empty()) {
    ATH_MSG_FATAL("Read empty string property from text file (property name: " << propname << ")");
  }

  // post-processing to get rid of leading/trailing spaces and comments
  std::string tmp_prop = property.substr(0, property.find("#", 0));
  property = TString(tmp_prop).ReplaceAll(" ","").Data();

  // Interpret None as an empty string
  if (property=="None" || property=="NONE" || property=="none") {
    ATH_MSG_VERBOSE( "Property \"" << propname << "\" being set to empty string due to specification of \"" << property << "\"" );
    property = "";
  }

  ATH_MSG_INFO( "configFromFile(): Loaded property \"" << propname << "\" with value " << property );
  // Remove the item from the table
  rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject(propname.c_str() ) );
}


StatusCode SUSYObjDef_xAOD::readConfig()
{

  ATH_MSG_INFO( "*****     *****     *****     *****" );
  ATH_MSG_INFO( "Configuring from file " << m_configFile );

  TEnv rEnv;
  int success = -1;
  success = rEnv.ReadFile(m_configFile.c_str(), kEnvAll);
  if (success != 0) return StatusCode::FAILURE;

  ATH_MSG_INFO( "Config file opened" );

  if (m_jetInputType == xAOD::JetInput::Uncategorized) {
    m_jetInputType = xAOD::JetInput::Type(rEnv.GetValue("Jet.InputType", 1));
    ATH_MSG_INFO( "readConfig(): Loaded property Jet.InputType with value " << (int)m_jetInputType);
    // Remove the item from the table
    rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject("Jet.InputType") );
  }
  
  if (m_muId == static_cast<int>(xAOD::Muon::Quality(xAOD::Muon::VeryLoose))) {
    int muIdTmp = rEnv.GetValue("Muon.Id", 1);
    m_muId = (muIdTmp<4 ? (int)xAOD::Muon::Quality(muIdTmp) : muIdTmp);
    ATH_MSG_INFO( "readConfig(): Loaded property Muon.Id with value " << m_muId);
    // Remove the item from the table
    rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject("Muon.Id") );
  }
  if (m_muIdBaseline == static_cast<int>(xAOD::Muon::Quality(xAOD::Muon::VeryLoose))) {
    int muIdTmp = rEnv.GetValue("MuonBaseline.Id", 1);
    m_muIdBaseline = (muIdTmp<4 ? (int)xAOD::Muon::Quality(muIdTmp) : muIdTmp);
    ATH_MSG_INFO( "readConfig(): Loaded property MuonBaseline.Id with value " << m_muIdBaseline);
    // Remove the item from the table
    rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject("MuonBaseline.Id") );
  }

  //load config file to Properties map  (only booleans for now)
  m_conf_to_prop["StrictConfigCheck"] = "StrictConfigCheck"; 
  m_conf_to_prop["Btag.enable"] = "UseBtagging"; 
  m_conf_to_prop["Ele.CrackVeto"] = "EleCrackVeto";
  m_conf_to_prop["EleBaseline.CrackVeto"] = "EleBaselineCrackVeto";
  m_conf_to_prop["Ele.ForceNoId"] = "EleForceNoId";
  m_conf_to_prop["Muon.ForceNoId"] = "MuonForceNoId";
  m_conf_to_prop["Muon.TTVASF"] = "MuonTTVASF";
  m_conf_to_prop["Muon.passedHighPt"] = "MuonRequireHighPtCuts";
  m_conf_to_prop["PhotonBaseline.CrackVeto"] = "PhotonBaselineCrackVeto";
  m_conf_to_prop["Photon.CrackVeto"] = "PhotonCrackVeto";
  m_conf_to_prop["Photon.AllowLate"] = "PhotonAllowLate";


  m_conf_to_prop["FwdJet.doJVT"] = "FwdJetDoJVT";
  m_conf_to_prop["FwdJet.JvtUseTightOP"] = "FwdJetUseTightOP";

  m_conf_to_prop["OR.DoBoostedElectron"] = "DoBoostedElectronOR";
  m_conf_to_prop["OR.DoBoostedMuon"] = "DoBoostedMuonOR";
  m_conf_to_prop["OR.DoMuonJetGhostAssociation"] = "ORDoMuonJetGhostAssociation";
  m_conf_to_prop["OR.DoTau"] = "DoTauOR";
  m_conf_to_prop["OR.DoPhoton"] = "DoPhotonOR";
  m_conf_to_prop["OR.DoEleJet"] = "DoEleJetOR";
  m_conf_to_prop["OR.DoMuonJet"] = "DoMuonJetOR";
  m_conf_to_prop["OR.Bjet"] = "DoBjetOR";
  m_conf_to_prop["OR.ElBjet"] = "DoElBjetOR";
  m_conf_to_prop["OR.MuBjet"] = "DoMuBjetOR";
  m_conf_to_prop["OR.TauBjet"] = "DoTauBjetOR";
  m_conf_to_prop["OR.DoFatJets"] = "DoFatJetOR";
  m_conf_to_prop["OR.RemoveCaloMuons"] = "ORRemoveCaloMuons";
  m_conf_to_prop["OR.MuJetApplyRelPt"] = "ORMuJetApplyRelPt";
  m_conf_to_prop["OR.InputLabel"] = "ORInputLabel";
 
  m_conf_to_prop["SigLep.RequireIso"] = "SigLepRequireIso";
  m_conf_to_prop["SigEl.RequireIso"] = "SigElRequireIso";
  m_conf_to_prop["SigMu.RequireIso"] = "SigMuRequireIso";
  m_conf_to_prop["SigPh.RequireIso"] = "SigPhRequireIso";
  m_conf_to_prop["SigLepPh.IsoCloseByOR"] = "SigLepPhIsoCloseByOR";
  m_conf_to_prop["MET.RemoveOverlappingCaloTaggedMuons"] = "METRemoveORCaloTaggedMuons";
  m_conf_to_prop["MET.DoSetMuonJetEMScale"] = "METDoSetMuonJetEMScale";
  m_conf_to_prop["MET.DoMuonJetOR"] = "METDoMuonJetOR";
  m_conf_to_prop["MET.DoTrkSyst"] = "METDoTrkSyst";
  m_conf_to_prop["MET.DoCaloSyst"] = "METDoCaloSyst";
  m_conf_to_prop["MET.GreedyPhotons"] = "METGreedyPhotons";

  m_conf_to_prop["Tau.MVACalibration"] = "TauMVACalibration";
  m_conf_to_prop["Tau.DoTruthMatching"] = "TauDoTruthMatching";
  m_conf_to_prop["Tau.RecalcElOLR"] = "TauRecalcElOLR";
  m_conf_to_prop["Tau.IDRedecorate"] = "TauIDRedecorate";  
  //

  //
  configFromFile(m_eleBaselinePt, "EleBaseline.Pt", rEnv, 10000.);
  configFromFile(m_eleBaselineEta, "EleBaseline.Eta", rEnv, 2.47);
  configFromFile(m_eleIdBaseline, "EleBaseline.Id", rEnv, "LooseAndBLayerLLH");
  configFromFile(m_eleBaselineCrackVeto, "EleBaseline.CrackVeto", rEnv, false);
  configFromFile(m_force_noElId, "Ele.ForceNoId", rEnv, false);
  //
  configFromFile(m_elePt, "Ele.Et", rEnv, 25000.);
  configFromFile(m_eleEta, "Ele.Eta", rEnv, 2.47);
  configFromFile(m_eleCrackVeto, "Ele.CrackVeto", rEnv, false);
  configFromFile(m_eleIso_WP, "Ele.Iso", rEnv, "GradientLoose");
  configFromFile(m_eleChID_WP, "Ele.CFT", rEnv, "None"); //Medium is the only one supported for the moment, and not many clients yet.
  configFromFile(m_eleId, "Ele.Id", rEnv, "TightLLH");
  configFromFile(m_eled0sig, "Ele.d0sig", rEnv, 5.);
  configFromFile(m_elez0, "Ele.z0", rEnv, 0.5);
  configFromFile(m_elebaselined0sig, "EleBaseline.d0sig", rEnv, -99.);
  configFromFile(m_elebaselinez0, "EleBaseline.z0", rEnv, -99.);
  configFromFile(m_EG_corrModel, "Ele.EffNPcorrModel", rEnv, "TOTAL");
  configFromFile(m_electronTriggerSFStringSingle, "Ele.TriggerSFStringSingle", rEnv, "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0");
  configFromFile(m_electronTriggerSFStringDiLepton, "Ele.TriggerSFStringDi", rEnv, "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0");
  configFromFile(m_electronTriggerSFStringMixedLepton, "Ele.TriggerSFStringMixedLepton", rEnv, "DI_E_2015_e17_lhloose_2016_e17_lhloose");
  //
  configFromFile(m_muBaselinePt, "MuonBaseline.Pt", rEnv, 10000.);
  configFromFile(m_muBaselineEta, "MuonBaseline.Eta", rEnv, 2.7);
  configFromFile(m_force_noMuId, "Muon.ForceNoId", rEnv, false);
  configFromFile(m_doTTVAsf, "Muon.TTVASF", rEnv, true);
  //
  configFromFile(m_muPt, "Muon.Pt", rEnv, 25000.);
  configFromFile(m_muEta, "Muon.Eta", rEnv, 2.7);
  configFromFile(m_muIso_WP, "Muon.Iso", rEnv, "GradientLoose");
  configFromFile(m_mud0sig, "Muon.d0sig", rEnv, 3.);
  configFromFile(m_muz0, "Muon.z0", rEnv, 0.5);
  configFromFile(m_mubaselined0sig, "MuonBaseline.d0sig", rEnv, -99.);
  configFromFile(m_mubaselinez0, "MuonBaseline.z0", rEnv, -99.);
  configFromFile(m_murequirepassedHighPtCuts, "Muon.passedHighPt", rEnv, false);
  //
  configFromFile(m_muCosmicz0, "MuonCosmic.z0", rEnv, 1.);
  configFromFile(m_muCosmicd0, "MuonCosmic.d0", rEnv, 0.2);
  //
  configFromFile(m_badmuQoverP, "BadMuon.qoverp", rEnv, 0.2);
  //
  configFromFile(m_photonBaselinePt, "PhotonBaseline.Pt", rEnv, 25000.);
  configFromFile(m_photonBaselineEta, "PhotonBaseline.Eta", rEnv, 2.37);
  configFromFile(m_photonIdBaseline, "PhotonBaseline.Id", rEnv, "Tight");
  configFromFile(m_photonBaselineCrackVeto, "PhotonBaseline.CrackVeto", rEnv, true);
  //
  configFromFile(m_photonPt, "Photon.Pt", rEnv, 130000.);
  configFromFile(m_photonEta, "Photon.Eta", rEnv, 2.37);
  configFromFile(m_photonId, "Photon.Id", rEnv, "Tight");
  configFromFile(m_photonIso_WP, "Photon.Iso", rEnv, "FixedCutTight");
  configFromFile(m_photonCrackVeto, "Photon.CrackVeto", rEnv, true);
  configFromFile(m_photonAllowLate, "Photon.AllowLate", rEnv, false);
  //
  configFromFile(m_tauPrePtCut, "Tau.PrePtCut", rEnv, 0.);
  configFromFile(m_tauPt, "Tau.Pt", rEnv, 20000.);
  configFromFile(m_tauEta, "Tau.Eta", rEnv, 2.5);
  configFromFile(m_tauId, "Tau.Id", rEnv, "Medium");
  configFromFile(m_tauConfigPath, "Tau.ConfigPath", rEnv, "default");
  configFromFile(m_tauIdBaseline, "TauBaseline.Id", rEnv, "Medium");
  configFromFile(m_tauConfigPathBaseline, "TauBaseline.ConfigPath", rEnv, "default");
  configFromFile(m_tauMVACalib, "Tau.MVACalibration", rEnv, false);
  configFromFile(m_tauDoTTM, "Tau.DoTruthMatching", rEnv, false);
  configFromFile(m_tauRecalcOLR, "Tau.RecalcElOLR", rEnv, false);
  configFromFile(m_tauIDrecalc, "Tau.IDRedecorate", rEnv, false);
  //
  configFromFile(m_jetPt, "Jet.Pt", rEnv, 20000.);
  configFromFile(m_jetEta, "Jet.Eta", rEnv, 2.8);
  configFromFile(m_JVT_WP, "Jet.JVT_WP", rEnv, "Medium");
  configFromFile(m_jesNPset, "Jet.JESNPSet", rEnv, 1);
  configFromFile(m_fatJets, "Jet.LargeRcollection", rEnv, "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets");
  configFromFile(m_fatJetUncConfig, "Jet.LargeRuncConfig", rEnv, "UJ2016_CombinedMass_medium.config"); //MultiTagging_medium.config");
  configFromFile(m_fatJetUncVars, "Jet.LargeRuncVars", rEnv, "default"); //do all if not specified
  configFromFile(m_WtagConfig, "Jet.WtaggerConfig", rEnv, "SmoothedWZTaggers/SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC15c_20161215.dat");
  configFromFile(m_ZtagConfig, "Jet.ZtaggerConfig", rEnv, "SmoothedWZTaggers/SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC15c_20161215.dat");
  //
  configFromFile(m_badJetCut, "BadJet.Cut", rEnv, "LooseBad");
  //
  configFromFile(m_doFwdJVT, "FwdJet.doJVT", rEnv, false);
  configFromFile(m_fwdjetEtaMin, "FwdJet.JvtEtaMin", rEnv, 2.5);
  configFromFile(m_fwdjetPtMax, "FwdJet.JvtPtMax", rEnv, 50e3);
  configFromFile(m_fwdjetTightOp, "FwdJet.JvtUseTightOP", rEnv, false);
  configFromFile(m_JMScalib, "Jet.JMSCalib", rEnv, "None");
  //
  configFromFile(m_useBtagging, "Btag.enable", rEnv, true);
  configFromFile(m_BtagTagger, "Btag.Tagger", rEnv, "MV2c10");
  configFromFile(m_BtagWP, "Btag.WP", rEnv, "FixedCutBEff_77");
  configFromFile(m_bTaggingCalibrationFilePath, "Btag.CalibPath", rEnv, "xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2017-07-02_v1.root");
  configFromFile(m_BtagSystStrategy, "Btag.SystStrategy", rEnv, "Envelope");
  //
  configFromFile(m_orDoBoostedElectron, "OR.DoBoostedElectron", rEnv, false);
  configFromFile(m_orBoostedElectronC1, "OR.BoostedElectronC1", rEnv, -999.); // set to positive number to override default
  configFromFile(m_orBoostedElectronC2, "OR.BoostedElectronC2", rEnv, -999.); // set to positive number to override default - specify in MeV (i.e. "10*GeV" will nor work in the config file)
  configFromFile(m_orBoostedElectronMaxConeSize, "OR.BoostedElectronMaxConeSize", rEnv, -999.); // set to positive number to override default
  configFromFile(m_orDoBoostedMuon, "OR.DoBoostedMuon", rEnv, true);
  configFromFile(m_orBoostedMuonC1, "OR.BoostedMuonC1", rEnv, -999.); // set to positive number to override default
  configFromFile(m_orBoostedMuonC2, "OR.BoostedMuonC2", rEnv, -999.); // set to positive number to override default - specify in MeV (i.e. "10*GeV" will nor work in the config file)
  configFromFile(m_orBoostedMuonMaxConeSize, "OR.BoostedMuonMaxConeSize", rEnv, -999.); // set to positive number to override default
  configFromFile(m_orDoMuonJetGhostAssociation, "OR.DoMuonJetGhostAssociation", rEnv, true);
  configFromFile(m_orDoTau, "OR.DoTau", rEnv, false);
  configFromFile(m_orDoPhoton, "OR.DoPhoton", rEnv, false);
  configFromFile(m_orDoEleJet, "OR.EleJet", rEnv, true);
  configFromFile(m_orDoMuonJet, "OR.MuonJet", rEnv, true);
  configFromFile(m_orDoBjet, "OR.Bjet", rEnv, true);
  configFromFile(m_orDoElBjet, "OR.ElBjet", rEnv, true);
  configFromFile(m_orDoMuBjet, "OR.MuBjet", rEnv, true);
  configFromFile(m_orDoTauBjet, "OR.TauBjet", rEnv, true);
  configFromFile(m_orApplyRelPt, "OR.MuJetApplyRelPt", rEnv, false);
  configFromFile(m_orMuJetPtRatio, "OR.MuJetPtRatio", rEnv, -999.);
  configFromFile(m_orMuJetTrkPtRatio, "OR.MuJetTrkPtRatio", rEnv, -999.);
  configFromFile(m_orRemoveCaloMuons, "OR.RemoveCaloMuons", rEnv, true);
  configFromFile(m_orMuJetInnerDR, "OR.MuJetInnerDR", rEnv, -999.);
  configFromFile(m_orBtagWP, "OR.BtagWP", rEnv, "FixedCutBEff_85");
  configFromFile(m_orInputLabel, "OR.InputLabel", rEnv, "selected"); //"baseline"
  //
  configFromFile(m_orDoFatjets, "OR.DoFatJets", rEnv, false);
  configFromFile(m_EleFatJetDR, "OR.EleFatJetDR", rEnv, -999.);
  configFromFile(m_JetFatJetDR, "OR.JetFatJetDR", rEnv, -999.);
  //
  configFromFile(m_doIsoSignal, "SigLep.RequireIso", rEnv, true);
  configFromFile(m_doElIsoSignal, "SigEl.RequireIso", rEnv, m_doIsoSignal);
  configFromFile(m_doMuIsoSignal, "SigMu.RequireIso", rEnv, m_doIsoSignal);
  configFromFile(m_doPhIsoSignal, "SigPh.RequireIso", rEnv, m_doIsoSignal);
  configFromFile(m_doIsoCloseByOR, "SigLepPh.IsoCloseByOR", rEnv, false);
  //
  configFromFile(m_eleTerm, "MET.EleTerm", rEnv, "RefEle");
  configFromFile(m_gammaTerm, "MET.GammaTerm", rEnv, "RefGamma");
  configFromFile(m_tauTerm, "MET.TauTerm", rEnv, "RefTau");
  configFromFile(m_jetTerm, "MET.JetTerm", rEnv, "RefJet");
  configFromFile(m_muonTerm, "MET.MuonTerm", rEnv, "Muons");
  configFromFile(m_outMETTerm, "MET.OutputTerm", rEnv, "Final");
  configFromFile(m_metRemoveOverlappingCaloTaggedMuons, "MET.RemoveOverlappingCaloTaggedMuons", rEnv, true);
  configFromFile(m_metDoSetMuonJetEMScale, "Met.DoSetMuonJetEMScale", rEnv, true);
  configFromFile(m_metDoMuonJetOR, "MET.DoMuonJetOR", rEnv, true);
  configFromFile(m_trkMETsyst, "MET.DoTrkSyst", rEnv, true);
  configFromFile(m_caloMETsyst, "MET.DoCaloSyst", rEnv, false);
  configFromFile(m_metGreedyPhotons, "MET.GreedyPhotons", rEnv, false);
  configFromFile(m_metJetSelection, "MET.JetSelection", rEnv, "Tight"); // set to non-empty to override default
  //
  configFromFile(m_muUncert, "PRW.MuUncertainty", rEnv, 0.2);
  //
  configFromFile(m_strictConfigCheck, "StrictConfigCheck", rEnv, false);

  // By now rEnv should be empty!
  if (rEnv.GetTable() && rEnv.GetTable()->GetSize()>0){
    ATH_MSG_ERROR("Found " << rEnv.GetTable()->GetSize() << " unparsed environment options:");
    rEnv.Print();
    ATH_MSG_ERROR("Please fix your configuration!");
    return StatusCode::FAILURE;
  }


  //** validate configuration
  ATH_CHECK( validConfig(m_strictConfigCheck) );

  //** cache trigger chains for matching 
  GetTriggerTokens(m_electronTriggerSFStringSingle, v_trigs15_cache_single, v_trigs16_cache_single);
  GetTriggerTokens(m_electronTriggerSFStringDiLepton, v_trigs15_cache_dilep, v_trigs16_cache_dilep);
  GetTriggerTokens(m_electronTriggerSFStringMixedLepton, v_trigs15_cache_mixlep, v_trigs16_cache_mixlep);

  return StatusCode::SUCCESS;
}


const std::vector<std::string> SUSYObjDef_xAOD::split(const std::string& s, const std::string& delim) const {
  assert(delim.length() == 1);
  std::vector<std::string> retval;
  retval.reserve(std::count(s.begin(), s.end(), delim[0]) + 1);
  size_t last = 0;
  size_t next = 0;
  bool gothere=false;
  while ((next = s.find(delim, last)) != std::string::npos) {
    retval.emplace_back(s.substr(last, next - last));
    last = next + delim.length();
    gothere=true;
  }
  if(gothere)
    retval.emplace_back(s.substr(last));
  
  return retval;
}

void SUSYObjDef_xAOD::getTauConfig(const std::string tauConfigPath, std::vector<float>& pT_window, std::vector<float>& eta_window, bool &eleOLR, bool &muVeto, bool &muOLR) const {

  if(tauConfigPath == "") return;
  
  TEnv rEnv;
  auto filename = PathResolverFindCalibFile(tauConfigPath);
  rEnv.ReadFile(filename.c_str(), kEnvAll);
  
  std::vector<std::string> cuts;
  if (rEnv.Defined("SelectionCuts")) {
    cuts = split(rEnv.GetValue("SelectionCuts", " "), " ");
  } else {
    auto l = rEnv.GetTable();
    for( Int_t i = 0; i < l->GetEntries(); ++i ) {
      cuts.push_back( l->At(i)->GetName() );
    }
  }

  //default OR settings
  eleOLR=false;
  muOLR=false;
  muVeto=false;

  static const std::string trueBool = "TRUE";

  // Now find the pT and eta window
  std::vector<std::string> _pT_window;
  std::vector<std::string> _eta_window;
  pT_window.clear();
  eta_window.clear();
  float pT_min = -99.0;
  float pT_max = -99.0;
  float eta_min = -99.0;
  float eta_max = -99.0;
  for (auto cut : cuts) {
    if(cut == "PtRegion") {
      _pT_window = split(rEnv.GetValue("PtRegion", ""), ";");
      std::transform(std::begin(_pT_window),
		     std::end(_pT_window), 
		     std::back_inserter(pT_window),
		     [](const std::string& s) { return std::stof(s); } 
		     );
    } else if (cut == "PtMin") {
      pT_min = rEnv.GetValue("PtMin", NAN);
    } else if (cut == "PtMax") {
      pT_max = rEnv.GetValue("PtMax", NAN);
    } else if (cut == "AbsEtaRegion") {
      _eta_window = split(rEnv.GetValue("AbsEtaRegion", ""), ";");
      std::transform(std::begin(_eta_window),
		     std::end(_eta_window), 
		     std::back_inserter(eta_window),
		     [](const std::string& s) { return std::stof(s); } 
		     );
    } else if (cut == "AbsEtaMin") {
      eta_min = rEnv.GetValue("AbsEtaMin", NAN);
    } else if (cut == "AbsEtaMax") {
      eta_max = rEnv.GetValue("AbsEtaMax", NAN);
    }
    
    //OR settings
    else if (cut == "EleOLR"){
      eleOLR = (rEnv.GetValue("EleOLR", "FALSE") == trueBool);
    }
    else if (cut == "MuonVeto"){
      muVeto = (rEnv.GetValue("MuonVeto", "FALSE") == trueBool);
    }
    else if (cut == "MuonOLR"){
      muOLR = (rEnv.GetValue("MuonOLR", "FALSE") == trueBool);
    }
  }
  
  if(pT_window.size() == 0) {
    if(pT_min == pT_min) {
      // fails on NaN
      pT_window.push_back(pT_min);
    } else {
      pT_window.push_back(-std::numeric_limits<float>::infinity());
    }
    
    if(pT_max == pT_max) {
      // fails on NaN
      pT_window.push_back(pT_max);
    } else {
      pT_window.push_back(std::numeric_limits<float>::infinity());
    }
  }
  
  if(eta_window.size() == 0) {
    if(eta_min == eta_min) {
      // fails on NaN
      eta_window.push_back(eta_min);
    } else {
      eta_window.push_back(-std::numeric_limits<float>::infinity());
    }
    
    if(eta_max == eta_max) {
      // fails on NaN
      eta_window.push_back(eta_max);
    } else {
      eta_window.push_back(std::numeric_limits<float>::infinity());
    }
  }  
}  
  
StatusCode SUSYObjDef_xAOD::validConfig(bool strict) const {
  // Validate configuration (i.e. that signal settings are tighter than baseline, etc)
  // :: Throw SC::FAILURE if strict mode is enabled, just a WARNING if not
  
  // Electrons
  if( m_elePt < m_eleBaselinePt ){
    ATH_MSG_WARNING("Your electron pt configuration is inconsistent!  Signal : " << m_elePt << " < Baseline : " << m_eleBaselinePt);
    if(strict) return StatusCode::FAILURE;
  }
  if( m_eleEta > m_eleBaselineEta ){
    ATH_MSG_WARNING("Your electron eta configuration is inconsistent!  Signal : " << m_eleEta << " < Baseline : " << m_eleBaselineEta);
    if(strict) return StatusCode::FAILURE;
  }

  if( check_isTighter(m_eleIdBaseline, m_eleId, el_id_support) ){
    ATH_MSG_WARNING("Your electron ID configuration is inconsistent!  Signal : " << m_eleId << " looser than Baseline : " << m_eleIdBaseline);
    if(strict) return StatusCode::FAILURE;
  }
  if( !m_eleCrackVeto && m_eleBaselineCrackVeto){
    ATH_MSG_WARNING("Your electron crack removal is inconsistent!  Signal : " << m_eleCrackVeto << " while Baseline : " << m_eleBaselineCrackVeto);
    if(strict) return StatusCode::FAILURE;
  }

  // Muons
  if( m_muPt < m_muBaselinePt ){
    ATH_MSG_WARNING("Your muon pt configuration is inconsistent!  Signal : " << m_muPt << " < Baseline : " << m_muBaselinePt);
    if(strict) return StatusCode::FAILURE;
  }
  if( m_muEta > m_muBaselineEta ){
    ATH_MSG_WARNING("Your muon eta configuration is inconsistent!  Signal : " << m_muEta << " < Baseline : " << m_muBaselineEta);
    if(strict) return StatusCode::FAILURE;
  }
  if( m_muId > m_muIdBaseline ){
    ATH_MSG_WARNING("Your muon ID configuration is inconsistent!  Signal : " << m_muId << " < Baseline : " << m_muIdBaseline);
    if(strict) return StatusCode::FAILURE;
  }

  // Photons
  if( m_photonPt < m_photonBaselinePt ){
    ATH_MSG_WARNING("Your photon pt configuration is inconsistent!  Signal : " << m_photonPt << " < Baseline : " << m_photonBaselinePt);
    if(strict) return StatusCode::FAILURE;
  }
  if( m_photonEta > m_photonBaselineEta ){
    ATH_MSG_WARNING("Your photon eta configuration is inconsistent!  Signal : " << m_photonEta << " < Baseline : " << m_photonBaselineEta);
    if(strict) return StatusCode::FAILURE;
  }
  if( m_photonId=="Loose" && m_photonIdBaseline=="Tight" ){
    ATH_MSG_WARNING("Your photon ID configuration is inconsistent!  Signal : " << m_photonId << " < Baseline : " << m_photonIdBaseline);
    if(strict) return StatusCode::FAILURE;
  }
  if( !m_photonCrackVeto && m_photonBaselineCrackVeto){
    ATH_MSG_WARNING("Your photon crack removal is inconsistent!  Signal : " << m_photonCrackVeto << " while Baseline : " << m_photonBaselineCrackVeto);
    if(strict) return StatusCode::FAILURE;
  }

  //Btagging //OR-wp tighter than signal-wp?
  if( m_BtagWP.compare(0, m_BtagWP.size()-3, m_orBtagWP) == 0 ){ //same tagger WP (FlatEff or FixedCut)
    if( atoi(m_BtagWP.substr(m_BtagWP.size()-2, m_BtagWP.size()).c_str()) < atoi(m_orBtagWP.substr(m_orBtagWP.size()-2, m_orBtagWP.size()).c_str()) ){ 
      ATH_MSG_WARNING("Your btagging configuration is inconsistent!  Signal : " << m_BtagWP << " is looser than OR-Baseline : " << m_orBtagWP);
      if(strict) return StatusCode::FAILURE;
    }
  }


  //Taus
  ///baseline vs signal pt check 
  if(m_tauPrePtCut > 0 and m_tauPrePtCut > m_tauPt) {
    ATH_MSG_WARNING("Your tau pt configuration is inconsistent! Signal pt cut : " << m_tauPt << " < Baseline (pre) pt cut : " << m_tauPrePtCut);
    if(strict) return StatusCode::FAILURE;
  }

  /// check pt and eta range
  std::vector<float> pT_window;
  std::vector<float> eta_window;
  bool elOLR=false;
  bool muVeto=false;
  bool muOLR=false;

  if(m_tauConfigPathBaseline != "") { //baseline taus

    std::string theConfig = m_tauConfigPathBaseline; 
    if( m_tauConfigPathBaseline=="default" ){
      if (m_tauId == "Loose") theConfig = "SUSYTools/tau_selection_loose.conf";
      if (m_tauId == "Medium") theConfig = "SUSYTools/tau_selection_medium.conf";
      else if (m_tauId == "Tight") theConfig = "SUSYTools/tau_selection_tight.conf";
    }

    //read config
    getTauConfig(theConfig, pT_window, eta_window, elOLR, muVeto, muOLR);

    //pt-eta checks
    if( m_tauPrePtCut > 0 and (m_tauPrePtCut != 1000*pT_window[0] or (pT_window[1] > 0 and m_tauPrePtCut > 1000*pT_window[1]))) {
      ATH_MSG_WARNING("Your baseline tau pt configuration is inconsistent! pre pT cut : " << m_tauPrePtCut << " / TauSelectionTool window (in MeV) : [" << 1000*pT_window[0] << ", " << 1000*pT_window[1] << "]");
      if(strict) return StatusCode::FAILURE;
    }
    
    if( m_tauPt > 0 and (m_tauPt != 1000*pT_window[0] or (pT_window[1] > 0 and m_tauPt > 1000*pT_window[1]))) {
      ATH_MSG_WARNING("Your baseline tau pt configuration is inconsistent! pT cut : " << m_tauPt << " / TauSelectionTool window (in Mev) : [" << 1000*pT_window[0] << ", " << 1000*pT_window[1] << "]");
      if(strict) return StatusCode::FAILURE;
    }
    
    if( m_tauEta > 0 and m_tauEta != eta_window[eta_window.size()-1]) { // eta window can have 4 entries
      ATH_MSG_WARNING("Your baseline tau eta configuration is inconsistent! eta cut : " << m_tauEta << " != TauSelectionTool max eta : " << eta_window[eta_window.size()-1]);
      if(strict) return StatusCode::FAILURE;
    }

    //OR checks
    if( m_orDoTau && !(elOLR && muOLR) ){
      ATH_MSG_WARNING("Your baseline tau - lep OR/Veto settings look suspicious! You have enabled tau-e/mu OR (cone based), but set (EleOLR,MuonOLR)=(" << elOLR << "," << muOLR << ") in your config file! PLEASE CHECK!");
      //if(strict) return StatusCode::FAILURE;
    }
  }

  if(m_tauConfigPath != "") { //signal taus

    std::string theConfig = m_tauConfigPath;
    if( m_tauConfigPath=="default" ){
      if (m_tauId == "Loose") theConfig = "SUSYTools/tau_selection_loose.conf";
      if (m_tauId == "Medium") theConfig = "SUSYTools/tau_selection_medium.conf";
      else if (m_tauId == "Tight") theConfig = "SUSYTools/tau_selection_tight.conf";
    }    

    getTauConfig(theConfig, pT_window, eta_window, elOLR, muVeto, muOLR);

    if( m_tauPrePtCut > 0 and (m_tauPrePtCut != 1000*pT_window[0] or (pT_window[1] > 0 and m_tauPrePtCut > 1000*pT_window[1]))) {
      ATH_MSG_WARNING("Your tau pt configuration is inconsistent! pre pT cut : " << m_tauPrePtCut << " / TauSelectionTool window (in MeV) : [" << 1000*pT_window[0] << ", " << 1000*pT_window[1] << "]");
      if(strict) return StatusCode::FAILURE;
    }
    
    if( m_tauPt > 0 and (m_tauPt != 1000*pT_window[0] or (pT_window[1] > 0 and m_tauPt > 1000*pT_window[1]))) {
      ATH_MSG_WARNING("Your tau pt configuration is inconsistent! pT cut : " << m_tauPt << " / TauSelectionTool window (in Mev) : [" << 1000*pT_window[0] << ", " << 1000*pT_window[1] << "]");
      if(strict) return StatusCode::FAILURE;
    }
    
    if( m_tauEta > 0 and m_tauEta != eta_window[eta_window.size()-1]) { // eta window can have 4 entries
      ATH_MSG_WARNING("Your tau eta configuration is inconsistent! eta cut : " << m_tauEta << " != TauSelectionTool max eta : " << eta_window[eta_window.size()-1]);
      if(strict) return StatusCode::FAILURE;
    }
  }

    
  return StatusCode::SUCCESS;
}

  

std::string SUSYObjDef_xAOD::TrigSingleLep() const { return m_electronTriggerSFStringSingle; }
std::string SUSYObjDef_xAOD::TrigDiLep()     const { return m_electronTriggerSFStringDiLepton; }
std::string SUSYObjDef_xAOD::TrigMixLep()    const { return m_electronTriggerSFStringMixedLepton; }

CP::SystematicCode SUSYObjDef_xAOD::resetSystematics() {   
  return this->applySystematicVariation(m_defaultSyst);
}

const CP::SystematicSet& SUSYObjDef_xAOD::currentSystematic() const {
  return m_currentSyst;
}

bool SUSYObjDef_xAOD::isNominal(const CP::SystematicSet& syst) const {
  return syst.name().empty();
}

bool SUSYObjDef_xAOD::isWeight(const CP::SystematicSet& systSet) const {
  // returns true if all systematics do _not_ affect kinematics and _any_ of them affects the weight
  bool affectsWeights = false;
  for (const auto& sys : systSet) {
    auto info = getSystInfo(sys);
    if(info.affectsKinematics) { return false; }
    affectsWeights = affectsWeights or info.affectsWeights;
  }
  return affectsWeights;
}

bool SUSYObjDef_xAOD::isVariation(const CP::SystematicSet& systSet) const {
  // returns true if _any_ of the systematics affect kinematics. ignores effect on weights.
  for (const auto& sys : systSet) {
    auto info = getSystInfo(sys);
    if(info.affectsKinematics) { return true; }
  }
  return false;
}

bool SUSYObjDef_xAOD::currentSystematicIsNominal() const {
  return isNominal(m_currentSyst);
}

bool SUSYObjDef_xAOD::currentSystematicIsVariation() const {
  return isVariation(m_currentSyst);
}

bool SUSYObjDef_xAOD::currentSystematicIsWeight() const {
  return isWeight(m_currentSyst);
}



CP::SystematicCode SUSYObjDef_xAOD::applySystematicVariation( const CP::SystematicSet& systConfig )
{
  if (!m_tool_init) {
    ATH_MSG_ERROR("SUSYTools was not initialized!!");
    return CP::SystematicCode::Unsupported;
  }

  ATH_MSG_DEBUG(" in SUSYObjDef_xAOD::applySystematicVariation \"" << systConfig.name() << "\" size " << systConfig.size());

  m_currentSyst = systConfig;

  // NB: SystematicSet typically has only one component (see SUSYToolsTester macro)
  if (!m_jetUncertaintiesTool.empty()) {
    CP::SystematicCode ret = m_jetUncertaintiesTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_VERBOSE("Cannot configure JetUncertaintiesTool for systematic var. " << systConfig.name() );
    } else {
      ATH_MSG_VERBOSE("Configured JetUncertaintiesTool for systematic var. " << systConfig.name() );
    }
  }
  if (!m_fatjetUncertaintiesTool.empty()) {
    CP::SystematicCode ret = m_fatjetUncertaintiesTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_VERBOSE("Cannot configure (Fat)JetUncertaintiesTool for systematic var. " << systConfig.name() );
    } else {
      ATH_MSG_VERBOSE("Configured (Fat)JetUncertaintiesTool for systematic var. " << systConfig.name() );
    }
  }
  if (!m_jetJvtEfficiencyTool.empty()) {
    CP::SystematicCode ret = m_jetJvtEfficiencyTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_VERBOSE("Cannot configure JVTEfficiency for systematic var. " << systConfig.name() );
    } else {
      ATH_MSG_VERBOSE("Configured JVTEfficiency for systematic var. " << systConfig.name() );
    }
  }
  if (!m_jerSmearingTool.empty()) {
    CP::SystematicCode ret = m_jerSmearingTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure JERSmearingTool for  systematic var. " << systConfig.name());
    } else {
      ATH_MSG_VERBOSE("Configured JERSmearing for systematic var. " << systConfig.name() );
    }
  }
  if (!m_muonCalibrationAndSmearingTool.empty()) {
    CP::SystematicCode ret =   m_muonCalibrationAndSmearingTool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure MuonCalibrationAndSmearingTool for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("MuonCalibrationAndSmearingTool configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_muonEfficiencySFTool.empty()) {
    CP::SystematicCode ret  = m_muonEfficiencySFTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure MuonEfficiencyScaleFactors for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("MuonEfficiencyScaleFactors configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_muonEfficiencyBMHighPtSFTool.empty()) {
    CP::SystematicCode ret  = m_muonEfficiencyBMHighPtSFTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure MuonBadMuonHighPtScaleFactors for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("MuonBadMuonHighPtScaleFactors configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_muonTTVAEfficiencySFTool.empty()) {
    CP::SystematicCode ret  = m_muonTTVAEfficiencySFTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure MuonTTVAEfficiencyScaleFactors for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("MuonTTVAEfficiencyScaleFactors configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_muonIsolationSFTool.empty()) {
    CP::SystematicCode ret  = m_muonIsolationSFTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure MuonIsolationScaleFactors for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("MuonIsolationScaleFactors configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_muonTriggerSFTool.empty()) {
    CP::SystematicCode ret  = m_muonTriggerSFTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure MuonTriggerScaleFactors for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("MuonTriggerScaleFactors configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecEfficiencySFTool_reco.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_reco->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (reco) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (reco) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecEfficiencySFTool_id.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_id->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (id) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (id) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecEfficiencySFTool_trig_singleLep.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_trig_singleLep->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (trigger) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecEfficiencySFTool_trig_diLep.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_trig_diLep->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (trigger) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecEfficiencySFTool_trig_mixLep.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_trig_mixLep->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (trigger) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecEfficiencySFTool_trigEff_singleLep.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_trigEff_singleLep->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (trigger) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecEfficiencySFTool_trigEff_diLep.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_trigEff_diLep->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (trigger) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecEfficiencySFTool_trigEff_mixLep.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_trigEff_mixLep->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (trigger) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecEfficiencySFTool_iso.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_iso->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (iso) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (iso) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecEfficiencySFTool_chf.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_chf->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (chf) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (chf) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_elecChargeEffCorrTool.empty()) {
    CP::SystematicCode ret = m_elecChargeEffCorrTool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure ElectronChargeEffCorrectionTool for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("ElectronChargeEffCorrectionTool configured for systematic var. " << systConfig.name() );
    }
  }
  if (!isData() && !m_photonEfficiencySFTool.empty()) {
    CP::SystematicCode ret = m_photonEfficiencySFTool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgPhotonEfficiencyCorrectionTool for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgPhotonEfficiencyCorrectionTool configured for systematic var. " << systConfig.name() );
    }
  }
  if (!isData() && !m_photonIsolationSFTool.empty()) {
    CP::SystematicCode ret = m_photonIsolationSFTool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgPhotonEfficiencyCorrectionTool for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgPhotonEfficiencyCorrectionTool configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_egammaCalibTool.empty()) {
    CP::SystematicCode ret = m_egammaCalibTool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure EgammaCalibrationAndSmearingTool for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("EgammaCalibrationAndSmearingTool configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_isoCorrTool.empty()) {
    CP::SystematicCode ret = m_isoCorrTool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure IsolationCorrectionTool for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("IsolationCorrectionTool configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_btagEffTool.empty()) {
    //if ( systConfig.size() > 0 && m_btagEffTool->isAffectedBySystematic(*systConfig.begin()) ) {
    CP::SystematicCode ret = m_btagEffTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure xAODBTaggingEfficiency for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured xAODBTaggingEfficiency for systematic var. " << systConfig.name() );
    }
    //}
  }
  if (!m_tauSmearingTool.empty()) {
    CP::SystematicCode ret = m_tauSmearingTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure TauSmearingTool for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured TauSmearingTool for systematic var. " << systConfig.name() );
    }
  }
  if (!m_tauEffTool.empty()) {
    CP::SystematicCode ret = m_tauEffTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured TauEfficiencyCorrectionsTool for systematic var. " << systConfig.name() );
    }
  }
  if (!m_tauTrigEffTool0.empty()) {
    CP::SystematicCode ret = m_tauTrigEffTool0->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool0 for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured TauEfficiencyCorrectionsTool0 for systematic var. " << systConfig.name() );
    }
  }  if (!m_tauTrigEffTool1.empty()) {
    CP::SystematicCode ret = m_tauTrigEffTool1->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool1 for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured TauEfficiencyCorrectionsTool1 for systematic var. " << systConfig.name() );
    }
  }
  if (!m_tauTrigEffTool2.empty()) {
    CP::SystematicCode ret = m_tauTrigEffTool2->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool2 for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured TauEfficiencyCorrectionsTool2 for systematic var. " << systConfig.name() );
    }
  }
  if (!m_tauTrigEffTool3.empty()) {
    CP::SystematicCode ret = m_tauTrigEffTool3->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool3 for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured TauEfficiencyCorrectionsTool3 for systematic var. " << systConfig.name() );
    }
  }
  if (!m_tauTrigEffTool4.empty()) {
    CP::SystematicCode ret = m_tauTrigEffTool4->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure TauEfficiencyCorrectionsTool4 for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured TauEfficiencyCorrectionsTool4 for systematic var. " << systConfig.name() );
    }
  }
  if (!m_metSystTool.empty()) {
    CP::SystematicCode ret = m_metSystTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure METSystematicsTool for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured METSystematicsTool for systematic var. " << systConfig.name() );
    }
  }
  if (!m_prwTool.empty()) {
    CP::SystematicCode ret = m_prwTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure PileupReweightingTool for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured PileupReweightingTool for systematic var. " << systConfig.name() );
    }
  }

  return CP::SystematicCode::Ok;
}

std::vector<ST::SystInfo> SUSYObjDef_xAOD::getSystInfoList() const {
  if (!m_tool_init) {
    ATH_MSG_ERROR("SUSYTools was not initialized!!");
    return vector<ST::SystInfo>();
  }

  ATH_MSG_INFO("Extracting systematics info list");

  // Now we can look at systematics:
  const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();

  vector<SystInfo> sysInfoList;
  sysInfoList.reserve(recommendedSystematics.size() * 2); // allow for continuous systematics

  // this is the nominal set
  SystInfo infodef;
  infodef.affectsKinematics = false;
  infodef.affectsWeights = false;
  infodef.affectsType = Unknown;
  infodef.affectedWeights.clear();
  sysInfoList.push_back(infodef);

  // add all recommended systematics 
  for (const auto& systSet : CP::make_systematics_vector(recommendedSystematics)) {
    for (const auto& sys : systSet) {

      sysInfoList.push_back(getSystInfo(sys));
    }
  }

  ATH_MSG_INFO("Returning list of " << sysInfoList.size() << " systematic variations");
  return sysInfoList;
}

ST::SystInfo SUSYObjDef_xAOD::getSystInfo(const CP::SystematicVariation& sys) const {

  SystInfo sysInfo;
  sysInfo.affectsKinematics = false;
  sysInfo.affectsWeights = false;
  sysInfo.affectedWeights.clear();
  sysInfo.systset.insert(sys);
  
  if (!m_jerSmearingTool.empty()) {
    if ( m_jerSmearingTool->isAffectedBySystematic( sys ) ) {
      sysInfo.affectsKinematics = true;
      sysInfo.affectsType = SystObjType::Jet;
    }
  }
  if (!m_jetJvtEfficiencyTool.empty()) {
    if ( m_jetJvtEfficiencyTool->isAffectedBySystematic( sys ) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Jet;
      sysInfo.affectedWeights.insert(ST::Weights::Jet::JVT);
    }
  }
  if (!m_jetUncertaintiesTool.empty()) {
    if ( m_jetUncertaintiesTool->isAffectedBySystematic( CP::SystematicVariation(sys.basename(), CP::SystematicVariation::CONTINUOUS) ) ) {
      sysInfo.affectsKinematics = true;
      sysInfo.affectsType = SystObjType::Jet;
    }
  }
  if (!m_fatjetUncertaintiesTool.empty()) {
    if ( m_fatjetUncertaintiesTool->isAffectedBySystematic( CP::SystematicVariation(sys.basename(), CP::SystematicVariation::CONTINUOUS) ) ) {
      sysInfo.affectsKinematics = true;
      sysInfo.affectsType = SystObjType::Jet;
    }
  }
  if (!m_muonCalibrationAndSmearingTool.empty()) {
    if ( m_muonCalibrationAndSmearingTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsKinematics = true;
      sysInfo.affectsType = SystObjType::Muon;
    }
  }
  if (!m_muonEfficiencySFTool.empty()) {
    if ( m_muonEfficiencySFTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Muon;
      sysInfo.affectedWeights.insert(ST::Weights::Muon::Reconstruction);
    }
  }
  if (!m_muonEfficiencyBMHighPtSFTool.empty()) {
    if ( m_muonEfficiencyBMHighPtSFTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Muon;
      sysInfo.affectedWeights.insert(ST::Weights::Muon::Reconstruction);
    }
  }
  if (!m_muonTTVAEfficiencySFTool.empty()) {
    if ( m_muonTTVAEfficiencySFTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Muon;
      sysInfo.affectedWeights.insert(ST::Weights::Muon::Reconstruction);
    }
  }
  if (!m_muonIsolationSFTool.empty()) {
    if ( m_muonIsolationSFTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Muon;
      sysInfo.affectedWeights.insert(ST::Weights::Muon::Isolation);
    }
  }
  if (!m_muonTriggerSFTool.empty()) {
    if ( m_muonTriggerSFTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Muon;
      sysInfo.affectedWeights.insert(ST::Weights::Muon::Trigger);
    }
  }
  if (!m_elecEfficiencySFTool_reco.empty()) {
    if ( m_elecEfficiencySFTool_reco->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::Reconstruction);
    }
  }
  if (!m_elecEfficiencySFTool_id.empty()) {
    if ( m_elecEfficiencySFTool_id->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::ID);
    }
  }
  if (!m_elecEfficiencySFTool_trig_singleLep.empty()) {
    if ( m_elecEfficiencySFTool_trig_singleLep->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::Trigger);
    }
  }
  if (!m_elecEfficiencySFTool_trig_diLep.empty()) {
    if ( m_elecEfficiencySFTool_trig_diLep->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::Trigger);
    }
  }
  if (!m_elecEfficiencySFTool_trig_mixLep.empty()) {
    if ( m_elecEfficiencySFTool_trig_mixLep->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::Trigger);
    }
  }
  if (!m_elecEfficiencySFTool_trigEff_singleLep.empty()) {
    if ( m_elecEfficiencySFTool_trigEff_singleLep->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::Trigger);
    }
  }
  if (!m_elecEfficiencySFTool_trigEff_diLep.empty()) {
    if ( m_elecEfficiencySFTool_trigEff_diLep->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::Trigger);
    }
  }
  if (!m_elecEfficiencySFTool_trigEff_mixLep.empty()) {
    if ( m_elecEfficiencySFTool_trigEff_mixLep->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::Trigger);
    }
  }
  if (!m_elecEfficiencySFTool_iso.empty() ) {
    if ( m_elecEfficiencySFTool_iso->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::Isolation);
    }
  }
  if (!m_elecEfficiencySFTool_chf.empty() ) {
    if ( m_elecEfficiencySFTool_chf->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::ChargeID);
    }
  }
  if (!m_elecChargeEffCorrTool.empty() ) {
    if ( m_elecChargeEffCorrTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::ChargeID);
    }
  }
  if (!m_egammaCalibTool.empty()) {
    if ( m_egammaCalibTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsKinematics = true;
      if (sys.basename().compare(0, 3, "EG_") == 0) {
	sysInfo.affectsType = SystObjType::Egamma;
      } else if (sys.basename().compare(0, 3, "PH_") == 0) {
	sysInfo.affectsType = SystObjType::Photon;
      } else if (sys.basename().compare(0, 3, "EL_") == 0) {
	sysInfo.affectsType = SystObjType::Electron;
      }
    }
  }
  if (!isData() && !m_isoCorrTool.empty()) {
    if (m_isoCorrTool->isAffectedBySystematic(sys)){
      sysInfo.affectsWeights = false;
      sysInfo.affectsKinematics = true;
      sysInfo.affectsType = SystObjType::Photon;
    }
  }
  if (!isData() && !m_photonEfficiencySFTool.empty()) {
    if (m_photonEfficiencySFTool->isAffectedBySystematic(CP::SystematicVariation(sys.basename(), CP::SystematicVariation::CONTINUOUS)) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Photon;
      sysInfo.affectedWeights.insert(ST::Weights::Photon::Reconstruction);
    }
  }
  if (!isData() && !m_photonIsolationSFTool.empty()) {
    if (m_photonIsolationSFTool->isAffectedBySystematic(sys)) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Photon;
      sysInfo.affectedWeights.insert(ST::Weights::Photon::Isolation);
    }
  }
  if ( !m_btagEffTool.empty() ) {
    if ( m_btagEffTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::BTag;
      sysInfo.affectedWeights.insert(ST::Weights::Jet::Btag);
    }
  }

  if (!m_tauSmearingTool.empty()) {
    if ( m_tauSmearingTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsKinematics = true;
      sysInfo.affectsType = SystObjType::Tau;
    }
  }
  if (!m_tauEffTool.empty()) {
    if ( m_tauEffTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Tau;
      sysInfo.affectedWeights.insert(ST::Weights::Tau::Reconstruction);
    }
  }
  if (!m_tauTrigEffTool0.empty()) {
    if ( m_tauTrigEffTool0->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Tau;
      sysInfo.affectedWeights.insert(ST::Weights::Tau::Trigger);
    }
  }
  if (!m_tauTrigEffTool1.empty()) {
    if ( m_tauTrigEffTool1->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Tau;
      sysInfo.affectedWeights.insert(ST::Weights::Tau::Trigger);
    }
  }
  if (!m_tauTrigEffTool2.empty()) {
    if ( m_tauTrigEffTool2->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Tau;
      sysInfo.affectedWeights.insert(ST::Weights::Tau::Trigger);
    }
  }
  if (!m_tauTrigEffTool3.empty()) {
    if ( m_tauTrigEffTool3->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Tau;
      sysInfo.affectedWeights.insert(ST::Weights::Tau::Trigger);
    }
  }
  if (!m_tauTrigEffTool4.empty()) {
    if ( m_tauTrigEffTool4->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Tau;
      sysInfo.affectedWeights.insert(ST::Weights::Tau::Trigger);
    }
  }
  if (!m_metSystTool.empty()) {
    if ( m_metSystTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsKinematics = true;
      switch (met::getSystType(sys)) {
      case met::SOFTCALO:
	sysInfo.affectsType = SystObjType::MET_CST;
	break;
      case met::SOFTTRK:
	sysInfo.affectsType = SystObjType::MET_TST;
	break;
      default:
	ATH_MSG_ERROR("Unsupported systematic!");
      }
    }
  }
  if (!m_prwTool.empty()) {
    if ( m_prwTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::EventWeight;
    }
  }
  
  std::string affectedType;
  switch (sysInfo.affectsType) {
  case Unknown     : affectedType = "UNKNOWN";  break;
  case Jet         : affectedType = "JET";      break;
  case Egamma      : affectedType = "EGAMMA";   break;
  case Electron    : affectedType = "ELECTRON"; break;
  case Photon      : affectedType = "PHOTON";   break;
  case Muon        : affectedType = "MUON";     break;
  case Tau         : affectedType = "TAU";      break;
  case BTag        : affectedType = "BTAG";     break;
  case MET_TST     : affectedType = "MET_TST";  break;
  case MET_CST     : affectedType = "MET_CST";  break;
  case EventWeight : affectedType = "EVENT WEIGHT"; break;
  }
  
  ATH_MSG_VERBOSE("Variation " << sys.name() << " affects "
		  << ( sysInfo.affectsWeights ? "weights " : "" )
		  << ( sysInfo.affectsKinematics ? "kinematics " : "" )
		  << "for " << affectedType );
  
  return sysInfo;
  
}


// Temporary function for Sherpa 2.2 V+jets n-jets reweighting 
// (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/CentralMC15ProductionList#NEW_Sherpa_v2_2_V_jets_NJet_rewe)
float SUSYObjDef_xAOD::getSherpaVjetsNjetsWeight() const {

  //Retrieve the truth jets
  if( evtStore()->contains<xAOD::JetContainer>("AntiKt4TruthWZJets") ){
    return getSherpaVjetsNjetsWeight("AntiKt4TruthWZJets");
  }
  else if( evtStore()->contains<xAOD::JetContainer>("AntiKt4TruthJets")){
    return getSherpaVjetsNjetsWeight("AntiKt4TruthJets" );
  }
  else{
    ATH_MSG_WARNING("No TruthJetContainer found! Dummy null weight retrieved.");
  }
  return 0.;
}

float SUSYObjDef_xAOD::getSherpaVjetsNjetsWeight(const std::string& jetContainer) const {

  if(jetContainer=="AntiKt4TruthWZJets"){
    return m_pmgSHnjetWeighterWZ->getWeight();
  }
  else if (jetContainer=="AntiKt4TruthJets"){
    return m_pmgSHnjetWeighter->getWeight();
  }
  else{
    ATH_MSG_WARNING(jetContainer << " is no supported by PMGSherpa22VJetsWeightTool! Please check...");
  }
  return 1.;

}


const xAOD::Vertex* SUSYObjDef_xAOD::GetPrimVtx() const {
  const xAOD::VertexContainer* vertices(0);
  if ( evtStore()->retrieve( vertices, "PrimaryVertices" ).isSuccess() ) {
    for ( const auto& vx : *vertices ) {
      if (vx->vertexType() == xAOD::VxType::PriVtx) {
        ATH_MSG_DEBUG("PrimaryVertex found with z=" << vx->z());
        return vx;
      }
    }
  } else {
    ATH_MSG_WARNING("Failed to retrieve VertexContainer \"PrimaryVertices\", returning NULL");
  }
  return NULL;
}


StatusCode SUSYObjDef_xAOD::OverlapRemoval(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons, 
					   const xAOD::JetContainer *jets, const xAOD::PhotonContainer* gamma, 
					   const xAOD::TauJetContainer* taujet, const xAOD::JetContainer* fatjets)
{

  if (this->GetPrimVtx() == nullptr) {
    ATH_MSG_WARNING( "Will now call the OR tool on an event without a primary vertex, and it will likely crash. Please require a PV earlier in your analysis code!");
  }

  ATH_CHECK( m_orToolbox.masterTool->removeOverlaps(electrons, muons, jets, taujet, gamma, fatjets) );

  /*
  // Count number of objects after overlap removal
  int Nel = 0;
  for (const auto& el : *electrons) {
    if (dec_passOR( *el )) Nel++;
  }

  int Nmu = 0;
  for (const auto& mu : *muons) {
    if (dec_passOR( *mu )) Nmu++;
  }

  int Njet = 0;
  for (const auto& jet : *jets) {
    if (dec_passOR( *jet )) Njet++;
  }

  ATH_MSG_VERBOSE( " After overlap removal: Nel=" << Nel << ", Nmu=" << Nmu << ", Njet=" << Njet );
  */

  return StatusCode::SUCCESS;
}


StatusCode SUSYObjDef_xAOD::NearbyLeptonCorrections(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons) const
{
  //apply close-by corrections to isolation
  // stores the electrons in a vector
  std::vector<const xAOD::IParticle*> pVec;
  for(auto pobj: *electrons) {
    pVec.push_back((const xAOD::IParticle*) pobj);
  }
  // stores the muons in a vector
  for(auto pobj: *muons) {
    pVec.push_back((const xAOD::IParticle*) pobj);
  }

  //correct isolation and propagate to signal deco for electrons
  for (const auto& electron : *electrons) {
    dec_isol(*electron) = m_isoCloseByTool->acceptCorrected(*electron, pVec);
    if(m_doElIsoSignal) dec_signal(*electron) &= acc_isol(*electron); //add isolation to signal deco if requested
  }

  //correct isolation and propagate to signal deco for electrons
  for (const auto& muon : *muons) {
    dec_isol(*muon) = m_isoCloseByTool->acceptCorrected(*muon, pVec);
    if(m_doMuIsoSignal) dec_signal(*muon) &= acc_isol(*muon); //add isolation to signal deco if requested
  }

  // All done, all good :-)
  return StatusCode::SUCCESS;
}



float SUSYObjDef_xAOD::GetPileupWeight() {
  
  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );

  float pu_weight = m_prwTool->getCombinedWeight(*evtInfo);

  if(!isfinite(pu_weight)) pu_weight = 1.;

  return pu_weight;
}

ULong64_t SUSYObjDef_xAOD::GetPileupWeightHash() {
  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
  return m_prwTool->getPRWHash( *evtInfo );
}

float SUSYObjDef_xAOD::GetDataWeight(const std::string& trig) {
  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
  return m_prwTool->getDataWeight( *evtInfo, trig );
}

float SUSYObjDef_xAOD::GetCorrectedAverageInteractionsPerCrossing(bool includeDataSF) {

  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
  return m_prwTool->getCorrectedAverageInteractionsPerCrossing( *evtInfo, includeDataSF );
}

double SUSYObjDef_xAOD::GetSumOfWeights(int channel) {
  return m_prwTool->GetSumOfEventWeights(channel);
}

unsigned int SUSYObjDef_xAOD::GetRandomRunNumber(bool muDependentRRN) {

  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
  if (randomrunnumber.isAvailable(*(evtInfo)) && muDependentRRN) {
    return randomrunnumber(*(evtInfo));
  }
  else if (!muDependentRRN) {
    return m_prwTool->getRandomRunNumber( *evtInfo, muDependentRRN );
  }
  ATH_MSG_ERROR ( "Failed to find RandomRunNumber decoration! You need to call ApplyPRWTool() beforehand!" );
  return 0;
}


StatusCode SUSYObjDef_xAOD::ApplyPRWTool(bool muDependentRRN) {

  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
  ATH_CHECK( m_prwTool->apply( *evtInfo, muDependentRRN ) );
  return StatusCode::SUCCESS;
}

unsigned int SUSYObjDef_xAOD::GetRunNumber() const {

  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );

  // For data, we can just directly use the run number 
  if (isData()) { return evtInfo->runNumber(); }

  //  else it's MC as we need the RRN assigned by the PRW tool 
  if (!randomrunnumber.isAvailable(*(evtInfo))) {
    ATH_MSG_ERROR ( "Failed to find RandomRunNumber decoration! You need to call ApplyPRWTool() beforehand!" );
  }
  return randomrunnumber(*(evtInfo));

}

int SUSYObjDef_xAOD::treatAsYear(const int runNumber) const {
  // Use the run number we are passed if we are passed one, otherwise
  //  use the run number from the GetRunNumber function
  int theRunNumber = runNumber>0?runNumber:GetRunNumber();
  // Split between 2015 and 2016 run numbers: 290000
  // Split between 2016 and 2017 run numbers: 320000
  if (theRunNumber<290000) return 2015;
  else if (theRunNumber<320000) return 2016;
  return 2017;
}

SUSYObjDef_xAOD::~SUSYObjDef_xAOD() {

#ifdef XAOD_STANDALONE
  // remove all tools from the asg::ToolStore (and delete them)
  // so that they don't get re-used if we set up another SUSYTools
  // instance, e.g. when processing two datasets in one EventLoop
  // job
  if (!m_trigDecTool.empty()){
    if (asg::ToolStore::contains<Trig::TrigDecisionTool>("ToolSvc.TrigDecisionTool") ){
      // Ignore both of these so that we are safe if others have cleaned up
      m_trigDecTool->finalize().ignore();
      asg::ToolStore::remove("ToolSvc.TrigDecisionTool").ignore();
    }
  }
#endif
}


}
