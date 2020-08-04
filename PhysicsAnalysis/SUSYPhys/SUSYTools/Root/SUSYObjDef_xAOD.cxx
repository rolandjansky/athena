/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCPInterfaces/ICPJetUncertaintiesTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "JetInterface/IJetModifier.h"
#include "JetAnalysisInterfaces/IJetJvtEfficiency.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"

#include "AsgAnalysisInterfaces/IEfficiencyScaleFactorTool.h"
#include "EgammaAnalysisInterfaces/IEgammaCalibrationAndSmearingTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
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

#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationSelection/IIsolationLowPtPLVTool.h"
#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "IsolationSelection/IIsolationCloseByCorrectionTool.h"

#include "METInterface/IMETMaker.h"
#include "METInterface/IMETSystematicsTool.h"
#include "METInterface/IMETSignificance.h"

#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
// Required to use some functions (see header explanation)
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "PATInterfaces/IWeightTool.h"
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"
#include "AssociationUtils/IOverlapRemovalTool.h"
#include "BoostedJetTaggers/SmoothedWZTagger.h"
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"
#include "ParticleJetTools/JetTruthLabelingTool.h"

// For reading metadata
#include "xAODMetaData/FileMetaData.h"

// For configuration -- TEnv uses THashList
#include "THashList.h"

// system includes
#include <fstream>
#include <regex>

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
    m_doModifiedEleId(false),
    m_upstreamTriggerMatching(false),
    m_trigMatchingPrefix(""),
    m_useBtagging(false),
    m_useBtagging_trkJet(false),
    m_debug(false),
    m_strictConfigCheck(false),
    m_badJetCut(""),
    m_fatJetUncConfig(""),
    m_fatJetUncVars(""),
    m_TCCJetUncConfig(""),
    m_WtagConfig(""),
    m_ZtagConfig(""),
    m_WZTaggerCalibArea(""),
    m_ToptagConfig(""),
    m_TopTaggerCalibArea(""),
    m_tool_init(false),
    m_subtool_init(false),
    // set dummies for configuration
    m_eleTerm(""),
    m_gammaTerm(""),
    m_tauTerm(""),
    m_jetTerm(""),
    m_muonTerm(""),
    m_inputMETSuffix(""),
    m_outMETTerm(""),
    m_metRemoveOverlappingCaloTaggedMuons(true),
    m_metDoSetMuonJetEMScale(true),
    m_metDoRemoveMuonJets(true),
    m_metUseGhostMuons(false),
    m_metDoMuonEloss(false),
    m_metGreedyPhotons(false),
    m_metVeryGreedyPhotons(false),
    m_metsysConfigPrefix(""),
    m_trkMETsyst(true),
    m_caloMETsyst(false),
    m_softTermParam(-99),
    m_treatPUJets(true),
    m_doPhiReso(true),
    m_autoconfigPRW(false),
    m_autoconfigPRWPath(""),
    m_autoconfigPRWFile(""),
    m_autoconfigPRWCombinedmode(false),
    m_autoconfigPRWRPVmode(false),
    m_autoconfigPRWHFFilter(""),
    m_autoconfigPRWRtags(""),
    m_mcCampaign(""),
    m_mcChannel(-99),
    m_prwDataSF(-99.),
    m_prwDataSF_UP(-99.),
    m_prwDataSF_DW(-99.),
    m_runDepPrescaleWeightPRW(false),
    m_electronTriggerSFStringSingle(""),
    m_eleId(""),
    m_eleIdBaseline(""),
    m_eleConfig(""),
    m_eleConfigBaseline(""),
    m_eleBaselineIso_WP(""),
    m_eleIdExpert(false),
    m_muId(static_cast<int>(xAOD::Muon::Quality(xAOD::Muon::VeryLoose))),
    m_muIdBaseline(static_cast<int>(xAOD::Muon::Quality(xAOD::Muon::VeryLoose))),
    m_photonId(""),
    m_photonIdBaseline(""),
    m_tauId(""),
    m_tauIdBaseline(""),
    m_eleIso_WP(""),
    m_eleIsoHighPt_WP(""),
    m_eleIsoHighPtThresh(-99.),
    m_eleChID_WP(""),
    m_eleChIso(true),
    m_eleChID_signal(false),
    m_runECIS(false),
    m_photonBaselineIso_WP(""),
    m_photonIso_WP(""),
    m_photonTriggerName(""),
    m_muBaselineIso_WP(""),
    m_muIso_WP(""),
    m_muIsoHighPt_WP(""),
    m_muIsoHighPtThresh(-99.),
    m_BtagWP(""),
    m_BtagTagger(""),
    m_BtagMinPt(-99.),
    m_BtagTimeStamp(""),
    m_BtagKeyOverride(""),
    m_BtagSystStrategy(""),
    m_BtagWP_trkJet(""),
    m_BtagTagger_trkJet(""),
    m_BtagMinPt_trkJet(-99.),
    m_BtagTimeStamp_trkJet(""),
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
    m_elebaselinez0(-99),
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
    m_muCalibrationMode(-99.),
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
    //
    m_jetPt(-99.),
    m_jetEta(-99.),
    m_jetJvt(-99.),
    m_JvtWP(""),
    m_JvtPtMax(-99.),
    m_JvtConfig(""),
    m_trkJetPt(-99.),
    m_trkJetEta(-99.),
    m_doFwdJVT(false),
    m_fJvtWP(""),
    m_fJvtPtMax(-99.),
    m_fJvtEtaMin(-99.),
    m_fJvtConfig(""),
    m_JMScalib(false),
    //
    m_orDoTau(false),
    m_orDoPhoton(false),
    m_orDoEleJet(true),
    m_orDoElEl(false),
    m_orDoElMu(false),
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
    m_orPhotonFavoured(false),
    m_orBJetPtUpperThres(-999.),
    m_orLinkOverlapObjects(false),
    m_orDoFatjets(false),
    m_EleFatJetDR(-999.),
    m_JetFatJetDR(-999.),
    m_doIsoSignal(true),
    m_doElIsoSignal(true),
    m_doPhIsoSignal(true),
    m_doMuIsoSignal(true),

    m_useSigLepForIsoCloseByOR(false),
    m_IsoCloseByORpassLabel(""),

    m_useTRUTH3(true),

    m_metJetSelection(""),
    m_fatJets(""),
    m_TCCJets(""),
    //
    m_currentSyst(),
    m_EG_corrModel(""),
    m_EG_corrFNList(""),
    m_applyJVTCut(true),
    //
    // set toolhandles empty by default
    m_jetCalibTool(""),
    m_jetFatCalibTool(""),
    m_jetUncertaintiesTool(""),
    m_jetUncertaintiesPDSmearTool(""),
    m_fatjetUncertaintiesTool(""),
    m_TCCjetUncertaintiesTool(""),
    m_jetCleaningTool(""),
    m_jetJvtUpdateTool(""),
    m_jetFwdJvtTool(""),
    m_jetJvtEfficiencyTool(""),
    m_jetFwdJvtEfficiencyTool(""),
    //
    m_WTaggerTool(""),
    m_ZTaggerTool(""),
    m_TopTaggerTool(""),
    m_jetTruthLabelingTool(""),
    //
    m_muonSelectionTool(""),
    m_muonSelectionHighPtTool(""),
    m_muonSelectionToolBaseline(""),
    m_muonCalibrationAndSmearingTool(""),
    m_muonEfficiencySFTool(""),
    m_muonEfficiencyBMHighPtSFTool(""),
    m_muonTTVAEfficiencySFTool(""),
    m_muonIsolationSFTool(""),
    m_muonHighPtIsolationSFTool(""),
    m_muonTriggerSFTool(""),
    //
    m_elecEfficiencySFTool_reco(""),
    m_elecEfficiencySFTool_id(""),
    m_elecEfficiencySFTool_trig_singleLep(""),
    m_elecEfficiencySFTool_trigEff_singleLep(""),
    m_elecEfficiencySFTool_iso(""),
    m_elecEfficiencySFTool_isoHighPt(""),
    m_elecEfficiencySFTool_chf(""),
    //
    m_egammaCalibTool(""),
    m_elecSelLikelihood(""),
    m_elecSelLikelihoodBaseline(""),
    m_photonSelIsEM(""),
    m_photonSelIsEMBaseline(""),
    m_photonEfficiencySFTool(""),
    m_photonIsolationSFTool(""),
    m_photonTriggerSFTool(""),
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
    m_tauElORdecorator(""),
    //
    m_btagEffTool(""),
    m_btagSelTool(""),
    m_btagSelTool_OR(""),
    m_btagEffTool_trkJet(""),
    m_btagSelTool_trkJet(""),
    //
    m_metMaker(""),
    m_metSystTool(""),
    m_metSignif(""),
    //
    m_trig2015combination_singleLep(""),
    m_trig2016combination_singleLep(""),
    m_trig2017combination_singleLep(""),
    m_trig2018combination_singleLep(""),
    m_trig2015combination_diLep(""),
    m_trig2016combination_diLep(""),
    m_trig2017combination_diLep(""),
    m_trig2018combination_diLep(""),
    m_trigGlobalEffCorrTool_diLep(""),
    m_trig2015combination_multiLep(""),
    m_trig2016combination_multiLep(""),
    m_trig2017combination_multiLep(""),
    m_trig2018combination_multiLep(""),
    m_trigGlobalEffCorrTool_multiLep(""),
    m_trig2015combination_diPhoton(""),
    m_trig2016combination_diPhoton(""),
    m_trig2017combination_diPhoton(""),
    m_trig2018combination_diPhoton(""),
    m_trigGlobalEffCorrTool_diPhoton(""),
    m_trigConfTool(""),
    m_trigDecTool(""),
    m_trigMatchingTool(""),
    //
    m_isoCorrTool(""),
    m_isoTool(""),
    m_isoToolLowPtPLV(""),
    m_isoBaselineTool(""),
    m_isoHighPtTool(""),
    //
    m_prwTool(""),
    //
    m_orToolbox("ORToolbox",this),
    //
    m_pmgSHnjetWeighter(""),
    m_pmgSHnjetWeighterWZ(""),
    //
    m_acc_eleIdBaseline(""),
    m_acc_eleId(""),
    m_acc_photonIdBaseline(""),
    m_acc_photonId(""),
    //
    m_acc_jetClean("")
{
  //General settings
  declareProperty( "DataSource", m_dataSource = Undefined );
  declareProperty( "DebugMode",  m_debug );
  declareProperty( "ConfigFile",  m_configFile = "SUSYTools/SUSYTools_Default.conf" );
  //Overlap Removal
  declareProperty( "DoTauOR",       m_orDoTau );
  declareProperty( "DoPhotonOR",    m_orDoPhoton );
  declareProperty( "DoEleJetOR",    m_orDoEleJet );
  declareProperty( "DoElElOR",    m_orDoElEl );
  declareProperty( "DoElMuOR",    m_orDoElMu );
  declareProperty( "DoMuonJetOR",   m_orDoMuonJet );
  declareProperty( "DoBjetOR",      m_orDoBjet );
  declareProperty( "DoElBjetOR",    m_orDoElBjet );
  declareProperty( "DoMuBjetOR",    m_orDoMuBjet );
  declareProperty( "DoTauBjetOR",    m_orDoTauBjet );
  declareProperty( "UseBtagging",   m_useBtagging );
  declareProperty( "UseBtaggingTrkJet",   m_useBtagging_trkJet );
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
  declareProperty( "ORPhotonFavoured", m_orPhotonFavoured);
  declareProperty( "ORBJetPtUpperThres", m_orBJetPtUpperThres);
  declareProperty( "ORLinkOverlapObjects", m_orLinkOverlapObjects);

  declareProperty( "DoFatJetOR", m_orDoFatjets);
  declareProperty( "OREleFatJetDR", m_EleFatJetDR);
  declareProperty( "ORJetFatJetDR", m_JetFatJetDR);
  declareProperty( "TriggerUpstreamMatching", m_upstreamTriggerMatching, "Use alternative trigger matching tool based on upstream (in-derivation) matching");
  declareProperty( "TriggerMatchingPrefix", m_trigMatchingPrefix, "Prefix for trigger matching containers (Analysis for PHSYLITE derivations");

  //--- Object definitions
  //MET
  declareProperty( "METEleTerm",     m_eleTerm   );
  declareProperty( "METGammaTerm",   m_gammaTerm );
  declareProperty( "METTauTerm",     m_tauTerm   );
  declareProperty( "METJetTerm",     m_jetTerm   );
  declareProperty( "METMuonTerm",    m_muonTerm  );
  declareProperty( "METInputSuffix", m_inputMETSuffix );
  declareProperty( "METOutputTerm",  m_outMETTerm );
  declareProperty( "METDoTrkSyst",   m_trkMETsyst  );
  declareProperty( "METDoCaloSyst",  m_caloMETsyst );
  declareProperty( "METDoTrkJetSyst",  m_trkJetsyst );
  declareProperty( "METJetSelection",  m_metJetSelection );
  declareProperty( "METSysConfigPrefix",  m_metsysConfigPrefix );

  declareProperty( "METRemoveORCaloTaggedMuons", m_metRemoveOverlappingCaloTaggedMuons);
  declareProperty( "METDoSetMuonJetEMScale", m_metDoSetMuonJetEMScale);
  declareProperty( "METDoRemoveMuonJets",  m_metDoRemoveMuonJets );
  declareProperty( "METUseGhostMuons",  m_metUseGhostMuons );
  declareProperty( "METDoMuonEloss",  m_metDoMuonEloss );
  declareProperty( "METDoGreedyPhotons",  m_metGreedyPhotons );
  declareProperty( "METDoVeryGreedyPhotons",  m_metVeryGreedyPhotons );


  declareProperty( "SoftTermParam",  m_softTermParam);
  declareProperty( "TreatPUJets",  m_treatPUJets);
  declareProperty( "DoPhiReso",  m_doPhiReso);

  //JETS
  declareProperty( "JetInputType",  m_jetInputType );

  declareProperty( "FwdJetDoJVT",  m_doFwdJVT );

  declareProperty( "JetJMSCalib",  m_JMScalib );
  declareProperty( "JetLargeRcollection",  m_fatJets );
  declareProperty( "JetTCCcollection",  m_TCCJets );

  //BTAGGING
  declareProperty( "BtagTagger", m_BtagTagger);
  declareProperty( "BtagWPOR", m_orBtagWP); //the one used in the Overlap Removal
  declareProperty( "BtagWP", m_BtagWP);     //the one used in FillJet() afterwards
  declareProperty( "BtagMinPt", m_BtagMinPt);    // minimum jetPt cut (MR31061) >=20 GeV EM jets & >=10 GeV TrackJets (not calibrated below)
  declareProperty( "BtagTimeStamp", m_BtagTimeStamp); /// Time stamp of the b-tagging containers introduced in p3954
  declareProperty( "BtagKeyOverride", m_BtagKeyOverride); /// Override for the b-tagging jet collection
  declareProperty( "BtagCalibPath", m_bTaggingCalibrationFilePath);
  declareProperty( "BtagTaggerTrkJet", m_BtagTagger_trkJet);
  declareProperty( "BtagMinPtTrkJet", m_BtagMinPt_trkJet);    // minimum jetPt cut (MR31061) >=20 GeV EM jets & >=10 GeV TrackJets (not calibrated below)
  declareProperty( "BtagWPTrkJet", m_BtagWP_trkJet);  //the one used in FillTrackJet() afterwards
  declareProperty( "BtagTimeStampTrkJet", m_BtagTimeStamp_trkJet);  //the one used in FillTrackJet() afterwards
  //ELECTRONS
  declareProperty( "EleBaselinePt", m_eleBaselinePt);
  declareProperty( "ElePt", m_elePt);
  declareProperty( "EleBaselineEta", m_eleBaselineEta);
  declareProperty( "EleEta", m_eleEta);
  declareProperty( "EleBaselineId", m_eleIdBaseline);
  declareProperty( "EleBaselineConfig", m_eleConfigBaseline);
  declareProperty( "EleBaselineCrackVeto", m_eleBaselineCrackVeto);
  declareProperty( "EleModifiedId", m_doModifiedEleId );
  declareProperty( "EleId", m_eleId);
  declareProperty( "EleConfig", m_eleConfig);
  declareProperty( "EleIso", m_eleIso_WP);
  declareProperty( "EleIsoHighPt", m_eleIsoHighPt_WP);
  declareProperty( "EleIsoHighPtThresh", m_eleIsoHighPtThresh);
  declareProperty( "EleCFT", m_eleChID_WP);
  declareProperty( "EleCFTIso", m_eleChIso);
  declareProperty( "EleCFTSignal", m_eleChID_signal);
  declareProperty( "EleD0sig", m_eled0sig);
  declareProperty( "EleZ0", m_elez0);
  declareProperty( "EleBaselineD0sig", m_elebaselined0sig);
  declareProperty( "EleBaselineZ0", m_elebaselinez0);
  declareProperty( "EleCrackVeto", m_eleCrackVeto);
  declareProperty( "EleForceNoId", m_force_noElId );
  declareProperty( "EleEffMapFilePath", m_eleEffMapFilePath);

  //MUONS
  declareProperty( "MuonBaselinePt", m_muBaselinePt);
  declareProperty( "MuonPt", m_muPt);
  declareProperty( "MuonBaselineEta", m_muBaselineEta);
  declareProperty( "MuonEta", m_muEta);
  declareProperty( "MuonBaselineId", m_muIdBaseline);
  declareProperty( "MuonId", m_muId);
  declareProperty( "MuonIso", m_muIso_WP);
  declareProperty( "MuonIsoHighPt", m_muIsoHighPt_WP);
  declareProperty( "MuonIsoHighPtThresh", m_muIsoHighPtThresh);
  declareProperty( "MuonD0sig", m_mud0sig);
  declareProperty( "MuonZ0", m_muz0);
  declareProperty( "MuonBaselineD0sig", m_mubaselined0sig);
  declareProperty( "MuonBaselineZ0", m_mubaselinez0);
  declareProperty( "MuonRequireHighPtCuts",  m_murequirepassedHighPtCuts);
  declareProperty( "MuonForceNoId", m_force_noMuId );
  declareProperty( "MuonTTVASF", m_doTTVAsf );
  declareProperty( "MuonCalibrationMode", m_muCalibrationMode);

  //PHOTONS
  declareProperty( "PhotonBaselinePt", m_photonBaselinePt);
  declareProperty( "PhotonPt", m_photonPt);
  declareProperty( "PhotonBaselineEta", m_photonBaselineEta);
  declareProperty( "PhotonEta", m_photonEta);
  declareProperty( "PhotonBaselineId", m_photonIdBaseline);
  declareProperty( "PhotonId", m_photonId);
  declareProperty( "PhotonIso", m_photonIso_WP);
  declareProperty( "PhotonTriggerName", m_photonTriggerName);
  declareProperty( "PhotonBaselineCrackVeto", m_photonBaselineCrackVeto);
  declareProperty( "PhotonCrackVeto", m_photonCrackVeto);
  declareProperty( "PhotonAllowLate", m_photonAllowLate);

  //TAUS
  declareProperty( "TauBaselineId", m_tauIdBaseline);
  declareProperty( "TauId", m_tauId);
  declareProperty( "TauIdConfigPathBaseline", m_tauConfigPathBaseline);
  declareProperty( "TauIdConfigPath", m_tauConfigPath);
  declareProperty( "TauDoTruthMatching", m_tauDoTTM);

  //Leptons
  declareProperty( "SigLepRequireIso", m_doIsoSignal ); //leave here for back-compatibility
  declareProperty( "SigElRequireIso", m_doElIsoSignal );
  declareProperty( "SigMuRequireIso", m_doMuIsoSignal );
  declareProperty( "SigPhRequireIso", m_doPhIsoSignal );

  //Isolation correction for leptons and photons
  declareProperty( "UseSigLepForIsoCloseByOR", m_useSigLepForIsoCloseByOR );
  declareProperty( "IsoCloseByORpassLabel", m_IsoCloseByORpassLabel );

  //Truth
  declareProperty( "UseTRUTH3", m_useTRUTH3 ); // true if using TRUTH3 type containers

  //--- Tools configuration
  //PRW
  declareProperty( "AutoconfigurePRWTool", m_autoconfigPRW );
  declareProperty( "AutoconfigurePRWToolPath", m_autoconfigPRWPath ); // e.g. dev/PileupReweighting/share/ 
  declareProperty( "AutoconfigurePRWToolFile", m_autoconfigPRWFile ); // e.g. DSID407xxx/pileup_mc16a_dsid407352_FS.root
  declareProperty( "AutoconfigurePRWToolCombinedmode", m_autoconfigPRWCombinedmode );
  declareProperty( "AutoconfigurePRWToolRPVmode", m_autoconfigPRWRPVmode );
  declareProperty( "AutoconfigurePRWToolHFFilter", m_autoconfigPRWHFFilter );
  declareProperty( "AutoconfigurePRWToolRtags", m_autoconfigPRWRtags );
  declareProperty( "mcCampaign",           m_mcCampaign );
  declareProperty( "mcChannel",            m_mcChannel );
  declareProperty( "PRWConfigFiles",       m_prwConfFiles );
  declareProperty( "PRWLumiCalcFiles",     m_prwLcalcFiles );
  declareProperty( "PRWActualMu2017File",  m_prwActualMu2017File );
  declareProperty( "PRWActualMu2018File",  m_prwActualMu2018File );
  declareProperty( "PRWDataScaleFactor",   m_prwDataSF);
  declareProperty( "PRWDataScaleFactorUP", m_prwDataSF_UP);
  declareProperty( "PRWDataScaleFactorDOWN", m_prwDataSF_DW);
  declareProperty( "PRWUseRunDependentPrescaleWeight", m_runDepPrescaleWeightPRW);
  //LargeR uncertainties config, as from https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/JetUncertainties2016PrerecLargeR#Understanding_which_configuratio
  declareProperty( "JetLargeRuncConfig",  m_fatJetUncConfig );
  declareProperty( "JetLargeRuncVars",  m_fatJetUncVars );
  declareProperty( "JetTCCuncConfig",  m_TCCJetUncConfig );
  declareProperty( "JetWtaggerConfig",  m_WtagConfig );
  declareProperty( "JetZtaggerConfig",  m_ZtagConfig );
  declareProperty( "JetWZTaggerCalibArea",  m_WZTaggerCalibArea );
  declareProperty( "JetToptaggerConfig",  m_ToptagConfig );
  declareProperty( "JetTopTaggerCalibArea",  m_TopTaggerCalibArea );
  //Btagging MCtoMC SFs
  declareProperty( "ShowerType",    m_showerType = 0 );
  //Egamma NP correlation model
  declareProperty( "EleEffNPcorrModel", m_EG_corrModel );
  //Egamma correction file list override
  declareProperty( "EleEffCorrFNList", m_EG_corrFNList );

  //For electron trigger SF tools
  declareProperty( "ElectronTriggerSFStringSingle",      m_electronTriggerSFStringSingle);

  //--- Tool pointers    /MT : we may want to improve the descriptions :)
  m_jetCalibTool.declarePropertyFor( this, "JetCalibTool", "The JetCalibTool" );
  m_jetFatCalibTool.declarePropertyFor( this, "FatJetCalibTool", "The JetCalibTool for large-R jets" );
  m_jetUncertaintiesTool.declarePropertyFor( this, "JetUncertaintiesTool", "The JetUncertaintiesTool" );
  m_jetUncertaintiesPDSmearTool.declarePropertyFor( this, "JetPDSmearUncertaintiesTool", "The JetPDSmearUncertaintiesTool" );
  m_fatjetUncertaintiesTool.declarePropertyFor( this, "FatJetUncertaintiesTool", "The JetUncertaintiesTool for large-R jets" );
  m_TCCjetUncertaintiesTool.declarePropertyFor( this, "TCCJetUncertaintiesTool", "The JetUncertaintiesTool for TCC jets" );
  m_jetCleaningTool.declarePropertyFor( this, "JetCleaningTool", "The JetCleaningTool" );
  m_jetJvtUpdateTool.declarePropertyFor( this, "JetJvtUpdateTool", "The JetJvtUpdateTool" );
  m_jetJvtEfficiencyTool.declarePropertyFor( this, "JetJvtEfficiencyTool", "The JetJvtEfficiencyTool" );
  m_jetFwdJvtTool.declarePropertyFor( this, "JetFwdJvtTool", "The JetFwdJvtTool" );
  m_jetFwdJvtEfficiencyTool.declarePropertyFor( this, "JetFwdJvtEfficiencyTool", "The JetFwdJvtEfficiencyTool" );
  //
  m_WTaggerTool.declarePropertyFor( this, "WTaggerTool", "The SmoothedWZTaggerTool" );
  m_ZTaggerTool.declarePropertyFor( this, "ZTaggerTool", "The SmoothedWZTaggerTool" );
  m_TopTaggerTool.declarePropertyFor( this, "TopTaggerTool", "The DNNTopTaggerTool" );
  m_jetTruthLabelingTool.declarePropertyFor( this, "JetTruthLabelingTool", "The JetTruthLabelingTool" );
  //
  m_muonSelectionTool.declarePropertyFor( this, "MuonSelectionTool", "The MuonSelectionTool for signal muons" );
  m_muonSelectionHighPtTool.declarePropertyFor( this, "MuonSelectionHighPtTool", "The MuonSelectionTool for signal muons (HighPt WP)" );
  m_muonSelectionToolBaseline.declarePropertyFor( this, "MuonSelectionToolBaseline", "The MuonSelectionTool for baseline muons" );
  m_muonCalibrationAndSmearingTool.declarePropertyFor( this, "MuonCalibrationAndSmearingTool", "The MuonCalibrationAndSmearingTool" );
  m_muonEfficiencySFTool.declarePropertyFor( this, "MuonEfficiencyScaleFactorsTool", "The MuonEfficiencySFTool" );
  m_muonEfficiencyBMHighPtSFTool.declarePropertyFor( this, "MuonBadMuonHighPtScaleFactorsTool", "The MuonBadMuonHighPtSFTool" );
  m_muonTTVAEfficiencySFTool.declarePropertyFor( this, "MuonTTVAEfficiencyScaleFactorsTool", "The MuonTTVAEfficiencySFTool" );
  m_muonIsolationSFTool.declarePropertyFor( this, "MuonIsolationScaleFactorsTool", "The MuonIsolationSFTool" );
  m_muonHighPtIsolationSFTool.declarePropertyFor( this, "MuonHighPtIsolationScaleFactorsTool", "The MuonIsolationSFTool" );
  m_muonTriggerSFTool.declarePropertyFor( this, "MuonTriggerScaleFactorsTool", "The MuonTriggerSFTool" );
  //
  m_elecEfficiencySFTool_reco.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_reco", "The ElectronEfficiencyCorrectionTool for reconstruction SFs" );
  m_elecEfficiencySFTool_trig_singleLep.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_trig_singleLep", "The ElectronEfficiencyCorrectionTool for single-e triggers" );
  m_elecEfficiencySFTool_id.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_id", "The ElectronEfficiencyCorrectionTool for ID SFs" );
  m_elecEfficiencySFTool_iso.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_iso" , "The ElectronEfficiencyCorrectionTool for iso SFs" );
  m_elecEfficiencySFTool_isoHighPt.declarePropertyFor( this, "ElectronEfficiencyCorrectionTool_isoHigPt" , "The ElectronEfficiencyCorrectionTool for iso high-pt SFs" );
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
  m_photonTriggerSFTool.declarePropertyFor( this, "PhotonTriggerEfficiencyCorrectionTool", "The PhotonEfficiencyCorrectionTool for trigger SFs" );
  m_electronPhotonShowerShapeFudgeTool.declarePropertyFor( this, "PhotonShowerShapeFudgeTool", "The ElectronPhotonShowerShapeFudgeTool" );
  //
  m_egammaCalibTool.declarePropertyFor( this, "EgammaCalibrationAndSmearingTool", "The EgammaCalibrationAndSmearingTool");
  //
  m_tauSelTool.declarePropertyFor( this, "TauSelectionTool", "The TauSelectionTool for signal taus" );
  m_tauSelToolBaseline.declarePropertyFor( this, "TauSelectionToolBaseline", "The TauSelectionTool for baseline taus" );
  m_tauSmearingTool.declarePropertyFor( this, "TauSmearingTool", "The TauSmearingTool" );
  m_tauTruthMatch.declarePropertyFor( this, "TauTruthMatch", "The TTMT" );
  m_tauEffTool.declarePropertyFor( this, "TauEfficiencyCorrectionsTool", "The TauEfficiencyCorrectionsTool" );
  m_tauElORdecorator.declarePropertyFor( this, "TauOverlappingElectronLLHDecorator", "The TauOverlappingElectronLLHDecorator tool" );
  //
  m_btagEffTool.declarePropertyFor( this, "BTaggingEfficiencyTool", "The BTaggingEfficiencyTool" );
  m_btagSelTool.declarePropertyFor( this, "BTaggingSelectionTool", "The main BTaggingSelectionTool" );
  m_btagSelTool_OR.declarePropertyFor( this, "BTaggingSelectionTool_OR", "The BTaggingSelectionTool used to select b-jets for overlap removal" );
  m_btagEffTool_trkJet.declarePropertyFor( this, "BTaggingEfficiencyTool_trkJet", "The BTaggingEfficiencyTool for track jets" );
  m_btagSelTool_trkJet.declarePropertyFor( this, "BTaggingSelectionTool_trkJet", "The main BTaggingSelectionTool for track jets" );
  //
  m_metMaker.declarePropertyFor( this, "METMaker", "The METMaker instance");
  m_metSystTool.declarePropertyFor( this, "METSystTool", "The METSystematicsTool");
  m_metSignif.declarePropertyFor( this, "METSignificance", "The METSignifiance instance");
  //
  m_trigGlobalEffCorrTool_diLep.declarePropertyFor( this, "TrigGlobalEfficiencyCorrection_diLep", "The TrigGlobalEfficiencyCorrection tool for dilepton" );
  m_trigGlobalEffCorrTool_multiLep.declarePropertyFor( this, "TrigGlobalEfficiencyCorrection_multiLep", "The TrigGlobalEfficiencyCorrection tool for trilepton" );
  m_trigGlobalEffCorrTool_diPhoton.declarePropertyFor( this, "TrigGlobalEfficiencyCorrection_diPhoton", "The TrigGlobalEfficiencyCorrection tool for asymmetric diphoton" );
  m_trigConfTool.declarePropertyFor( this, "TrigConfigTool", "The TrigConfigTool" );
  m_trigDecTool.declarePropertyFor( this, "TrigDecisionTool", "The TrigDecisionTool" );
  m_trigMatchingTool.declarePropertyFor( this, "TrigMatchTool", "The TrigMatchingTool" );
  //
  m_isoCorrTool.declarePropertyFor( this, "IsolationCorrectionTool", "The IsolationCorrectionTool" );
  m_isoTool.declarePropertyFor( this, "IsolationSelectionTool", "The IsolationSelectionTool");
  m_isoToolLowPtPLV.declarePropertyFor( this, "IsolationLowPtPLVTool", "The IsolationLowPtPLVTool");
  m_isoBaselineTool.declarePropertyFor( this, "IsolationSelectionTool_Baseline", "The IsolationSelectionTool for baseline objects");
  m_isoHighPtTool.declarePropertyFor( this, "IsolationSelectionTool_HighPt", "The IsolationSelectionTool for High Pt");
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
  m_el_id_support.push_back("VeryLooseLLH");
  m_el_id_support.push_back("LooseLLH");
  m_el_id_support.push_back("LooseAndBLayerLLH");
  m_el_id_support.push_back("MediumLLH");
  m_el_id_support.push_back("TightLLH");

  m_ph_id_support.push_back("Loose");
  m_ph_id_support.push_back("Tight");
  m_ph_id_support.push_back("TightPtIncl"); // Should not be mixed with Tight

  m_mu_id_support = 4; //maximum value supported

  // Tau ID WPs
  m_tau_id_support.push_back("VeryLoose");
  m_tau_id_support.push_back("Loose");
  m_tau_id_support.push_back("Medium");
  m_tau_id_support.push_back("Tight");

  // Iso WPs
  // -- see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/RecommendedIsolationWPs#Current_official_working_points
  // -- the el iso points are those which have (or will have) SFs available
  m_el_iso_support = {
     "FCLoose", "FCTight",             // current WPs
     "FCHighPtCaloOnly",               // current HighPtCaloOnly WPs
     "Gradient"                        // 
  };
  // -- the muon iso points are those which have SFs available
  // -- more details https://indico.cern.ch/event/878781/contributions/3721998/attachments/1976194/3289315/20200127_IFFshort_2.pdf
  m_mu_iso_support = {
     "PflowLoose_FixedRad", "PflowLoose_VarRad", "PflowTight_FixedRad", "PflowTight_VarRad",  // PFlow (new naming) recommended WPs
     "TightTrackOnly_FixedRad", "TightTrackOnly_VarRad", "HighPtTrackOnly",                   // TrackOnly (new naming) recommended WPs
     "PLVLoose", "PLVTight",                                                                  // PLV recommended WPs 
     "Loose_VarRad", "Loose_FixedRad", "Tight_VarRad", "Tight_FixedRad",                      // Other WPs (new naming)
  };

  // Construct electron fallback WPs for SFs
  for (auto x : m_el_iso_support) { m_el_iso_fallback[x] = x; } // all current WPs
  m_el_iso_fallback["PLVTight"] = "FCTight";                    // plus actual fallback
  m_el_iso_fallback["PLVLoose"] = "FCLoose";

  // Construct muon fallback WPs for SFs
  m_mu_iso_fallback = {};
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
    // if(m_useLeptonTrigger<0) ATH_MSG_ERROR( "UseLeptonTrigger not set");
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

  if (m_inputMETSuffix==""){
    m_inputMETSuffix = "AntiKt4" + xAOD::JetInput::typeName(xAOD::JetInput::Type(m_jetInputType));
  }
  m_defaultJets = "AntiKt4" + xAOD::JetInput::typeName(xAOD::JetInput::Type(m_jetInputType)) + "Jets";
  ATH_MSG_INFO( "Configured for jet collection: " << m_defaultJets );

  m_defaultTruthJets = "AntiKt4TruthJets";
  const xAOD::FileMetaData* fmd = 0;
  std::string dataType;
  if ( inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData") && inputMetaStore()->retrieve(fmd,"FileMetaData").isSuccess() )
     fmd->value(xAOD::FileMetaData::dataType, dataType);
  if ( dataType.compare("StreamDAOD_PHYS")==0 || dataType.compare("StreamDAOD_PHYSLITE")==0 ) m_defaultTruthJets = "AntiKt4TruthDressedWZJets";
  ATH_MSG_INFO( "Configured for truth jet collection: " << m_defaultTruthJets );

  m_inputMETCore = "MET_Core_" + m_inputMETSuffix;
  m_inputMETMap = "METAssoc_" + m_inputMETSuffix;
  ATH_MSG_INFO("Build MET with map: " << m_inputMETMap);

  m_jetCleanDFName = TString::Format("DFCommonJets_jetClean_%s", m_badJetCut.c_str());
  m_acc_jetClean = m_jetCleanDFName;

  m_eleIdBaselineDFName = "DFCommonElectronsLH";
  m_eleIdBaselineDFName += TString(m_eleIdBaseline).ReplaceAll("LooseAndBLayer","LooseBL").ReplaceAll("LLH","").Data();
  m_acc_eleIdBaseline = m_eleIdBaselineDFName;

  m_eleIdDFName = "DFCommonElectronsLH";
  m_eleIdDFName += TString(m_eleId).ReplaceAll("LooseAndBLayer","LooseBL").ReplaceAll("LLH","").Data();
  m_acc_eleId = m_eleIdDFName;

  m_photonIdBaselineDFName = "DFCommonPhotonsIsEM";
  m_photonIdBaselineDFName += TString(m_photonIdBaseline).Data();
  m_acc_photonIdBaseline = m_photonIdBaselineDFName;

  m_photonIdDFName = "DFCommonPhotonsIsEM";
  m_photonIdDFName += TString(m_photonId).Data();
  m_acc_photonId = m_photonIdDFName;

  // autoconfigure PRW tool if m_autoconfigPRW==true
  if (m_autoconfigPRWPath == "dev/PileupReweighting/share/")
    ATH_CHECK( autoconfigurePileupRWTool(m_autoconfigPRWPath, m_autoconfigPRWFile, true, m_autoconfigPRWRPVmode, m_autoconfigPRWCombinedmode, m_autoconfigPRWHFFilter) );
  else
    // need to set a full path if you don't use the one in CVMFS
    ATH_CHECK( autoconfigurePileupRWTool(m_autoconfigPRWPath, m_autoconfigPRWFile, false, m_autoconfigPRWRPVmode, m_autoconfigPRWCombinedmode, m_autoconfigPRWHFFilter) );

  ATH_CHECK( this->SUSYToolsInit() );

  ATH_MSG_VERBOSE("Done with tool retrieval");

  m_tool_init = true;

  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode SUSYObjDef_xAOD::autoconfigurePileupRWTool(const std::string& PRWfilesDir, const std::string& PRWfileName, bool usePathResolver, bool RPVLLmode, bool Combinedmode, const std::string& HFFilter ) {

  std::string prwConfigFile("");

  if ( !isData() && m_autoconfigPRW ) {

    prwConfigFile = PRWfilesDir;

    float dsid = -999;
    std::string amiTag("");
    std::string mcCampaignMD("");
    std::string simType = (isAtlfast() ? "AFII" : "FS");
    const xAOD::FileMetaData* fmd = 0;

    // configure PRW rtag options from m_autoconfigPRWRtags string
    // e.g. "mc16a:r9364_r11505_r11285,mc16c:r9781,mc16d:r10201_r11506_r11279,mc16e:r10724_r11507_r11249,mc16ans:r10740_r10832_r10847_r11008_r11036,mc16dns:r10739_r10833_r10848_r11009_r11037,mc16ens:r10790_r11038_r11265"
    std::map<std::string,std::vector<std::string>> PRWRtags = {};
    std::string allcampaigns = "mc16a.mc16c.mc16d.mc16e.mc16ans.mc16dns.mc16ens";
    bool standard_like = true;
    for ( auto campaign_rtags : split( m_autoconfigPRWRtags, "," ) ) {                                          // split string by ","
       std::string icampaign = campaign_rtags.substr(0, campaign_rtags.find(":"));                              // first field = campaign, split by ":"
       std::vector<std::string> irtags = split( campaign_rtags.substr(campaign_rtags.find(":")+1), "_" );       // remaining fields = rtags, split by "_"
       PRWRtags[icampaign] = irtags;
       ATH_MSG_DEBUG( "PRW autoconfigure considering rtags " <<  campaign_rtags.substr(campaign_rtags.find("_")+1) << " for campaign " << icampaign );
    }
    for ( auto x : PRWRtags ) {
       if ( allcampaigns.find(x.first)==string::npos ) {
          ATH_MSG_ERROR("m_autoconfigPRWRtags contains invalid campaign: " << x.first << " (" << m_autoconfigPRWRtags << ")");
          ATH_MSG_ERROR("use any of " << allcampaigns);
          return StatusCode::FAILURE;
       }
    }

    // let's use MetaData to extract sample information
    if ( inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData") && inputMetaStore()->retrieve(fmd,"FileMetaData").isSuccess() ) {
      fmd->value(xAOD::FileMetaData::mcProcID, dsid);
      fmd->value(xAOD::FileMetaData::amiTag, amiTag);
      bool found = false;
      while ( mcCampaignMD.empty() ) {
         for ( auto campaign_rtags : PRWRtags ) {                                 // consider all campaigns
            for ( auto rtag: campaign_rtags.second ) {                            // consider all rtags
               if (found) continue;
               if (amiTag.find(rtag)!=string::npos) {                             // find matching tag
                  mcCampaignMD = campaign_rtags.first.substr(0,5);                // save campaign
                  standard_like = (campaign_rtags.first.find("ns")?true:false);   // track non-standard r-tags
                  found = true;                                                   // break if found
               }
            }
         }
         // in case not found
         if (!found) {
         	ATH_MSG_ERROR( "autoconfigurePileupRWTool(): unrecognized xAOD::FileMetaData::amiTag, \'" << amiTag << "'. Please check your input sample.");
	         return StatusCode::FAILURE;
         }
         //
         ATH_MSG_INFO( "Setting up autoconfigurePileupRWTool for mc campaign " << mcCampaignMD << " (from amiTag " << amiTag << ") (standard-like = " << (standard_like?"true":"false") << ")." );
         if ( (!standard_like) && (!RPVLLmode) )
            ATH_MSG_WARNING("Non-standard r-tag found (" << amiTag.data() << "), but RPVLLmode not set. Perhaps you want to set \"PRW.autoconfigPRWRPVmode: 1\" in the config?");
      }
    } else {
#ifndef XAOD_STANDALONE
      ATH_MSG_ERROR( "autoconfigurePileupRWTool(): access to FileMetaData failed, can't get mc channel number.");
      return StatusCode::FAILURE;
#else

      if ( m_mcCampaign == "mc16a" || m_mcCampaign == "mc16c" || m_mcCampaign == "mc16d" || m_mcCampaign == "mc16e") {
	// First see if the user set the mcCampaign/run number by property (hopefully temporary workaround)
	if ( m_mcChannel > 0) {
	  ATH_MSG_WARNING( "autoconfigurePileupRWTool(): access to FileMetaData failed -> getting the mc channel number (DSID) and campaign from configuration." );
	  std::string NoMetadataButPropertyOK("");
	  NoMetadataButPropertyOK += "autoconfigurePileupRWTool(): 'mcCampaign' is used and passed to SUSYTools as '";
	  NoMetadataButPropertyOK += m_mcCampaign;
	  NoMetadataButPropertyOK += "'. 'mcChannel' is used and passed to SUSYTools as '";
	  NoMetadataButPropertyOK += std::to_string(m_mcChannel);
	  NoMetadataButPropertyOK += "'. Autocongiguring PRW accordingly.";
	  ATH_MSG_WARNING( NoMetadataButPropertyOK );
	  mcCampaignMD = m_mcCampaign;
	  dsid = m_mcChannel;
	} else {
	  // OK, this is a fall-back option without using MetaData but one has to manually set 'mcCampaign' property
	  ATH_MSG_WARNING( "autoconfigurePileupRWTool(): access to FileMetaData failed -> getting the mc channel number (DSID) from the event store." );
	  const xAOD::EventInfo* evtInfo = 0;
	  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
	  dsid = evtInfo->mcChannelNumber();

	  std::string NoMetadataButPropertyOK("");
	  NoMetadataButPropertyOK += "autoconfigurePileupRWTool(): 'mcCampaign' is used and passed to SUSYTools as '";
	  NoMetadataButPropertyOK += m_mcCampaign;
	  NoMetadataButPropertyOK += "'. Autoconfiguring PRW accordingly.";
	  ATH_MSG_WARNING( NoMetadataButPropertyOK );
	  mcCampaignMD = m_mcCampaign;
	}
      } else {
	ATH_MSG_ERROR( "autoconfigurePileupRWTool(): `mcCampaign' is not set properly.");
	return StatusCode::FAILURE;
      }
#endif
    }

    // Retrieve the input file
    int DSID_INT = (int) dsid;
    prwConfigFile += "DSID" + std::to_string(DSID_INT/1000) + "xxx/pileup_" + mcCampaignMD + "_dsid" + std::to_string(DSID_INT) + "_" + simType + ".root";

    if (RPVLLmode) prwConfigFile = TString(prwConfigFile).ReplaceAll(".root","_rpvll.root").Data();

    // PRW file specified by user
    // e.g. DSID407xxx/pileup_mc16a_dsid407352_FS.root
    if (!PRWfileName.empty()) {
      prwConfigFile = PRWfilesDir + PRWfileName;
      ATH_MSG_INFO( "autoconfigurePileupRWTool(): PRW file was specifed by user: " << prwConfigFile.data() );
    }

    // Patch for MC16 Znunu metadata bug  (updated 2019.05.30)
    if (!HFFilter.empty() && dsid>=366001 && dsid<= 366008) {
      ATH_MSG_WARNING ("Samples metadata for Znunu samples is corrupted! Remapping to grab the correct RPW file. Only MC16e is supported for now.");
      if (HFFilter == "BFilter") {
        prwConfigFile = TString(prwConfigFile).ReplaceAll(std::to_string(DSID_INT),std::to_string(DSID_INT+9)).Data();
      } else if (HFFilter == "CFilterBVeto") {
        prwConfigFile = TString(prwConfigFile).ReplaceAll(std::to_string(DSID_INT),std::to_string(DSID_INT+18)).Data();
      } else if (HFFilter == "CVetoBVeto") {
        prwConfigFile = TString(prwConfigFile).ReplaceAll(std::to_string(DSID_INT),std::to_string(DSID_INT+27)).Data();
      } else {
        ATH_MSG_ERROR ("Heavy flavor filter naming is wrong and cannot re-map dsid! SHould be BFilter, CFilterBVeto, or CVetoBVeto.");
        return StatusCode::FAILURE;
      }
    }

    m_prwConfFiles.clear();

    // Combined mode can be only used when running with full data with the same MC samples
    if (Combinedmode) {
      prwConfigFile = TString(prwConfigFile).ReplaceAll(mcCampaignMD,"mc16a").Data();
      m_prwConfFiles.push_back( prwConfigFile );
      prwConfigFile = TString(prwConfigFile).ReplaceAll("mc16a","mc16d").Data();
      m_prwConfFiles.push_back( prwConfigFile );
      prwConfigFile = TString(prwConfigFile).ReplaceAll("mc16d","mc16e").Data();
      m_prwConfFiles.push_back( prwConfigFile );
      m_prwConfFiles.push_back( PathResolverFindCalibFile(m_prwActualMu2017File) );
      m_prwConfFiles.push_back( PathResolverFindCalibFile(m_prwActualMu2018File) );
    } else {
      m_prwConfFiles.push_back( prwConfigFile );
      if ( mcCampaignMD == "mc16c" || mcCampaignMD == "mc16d") {
        m_prwConfFiles.push_back( PathResolverFindCalibFile(m_prwActualMu2017File) );
      } else if (mcCampaignMD == "mc16e") {
        m_prwConfFiles.push_back( PathResolverFindCalibFile(m_prwActualMu2018File) );
      }
    }
    prwConfigFile = usePathResolver ? PathResolverFindCalibFile(prwConfigFile) : prwConfigFile;

    TFile testF(prwConfigFile.data(),"read");
    if (testF.IsZombie()) {
      ATH_MSG_ERROR( "autoconfigurePileupRWTool(): file not found -> " << prwConfigFile.data() << " ! Impossible to autoconfigure PRW. Aborting." );
      if ( (!standard_like) && (!RPVLLmode) )
         ATH_MSG_WARNING("Running with non-standard r-tag (" << amiTag.data() << "), but RPVLLmode not set. Perhaps you want to set \"PRW.autoconfigPRWRPVmode: 1\" in the config?");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "autoconfigurePileupRWTool(): configuring PRW tool using " << prwConfigFile.data() );
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
                                     const std::string& defaultValue, bool allowEmpty)
{
  // ignore if already configured
  if (!property.empty()){
    ATH_MSG_INFO( "configFromFile(): property \"" << propname << "\" already set with value " << property << ". Ignoring change request." );
    rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject(propname.c_str() ) );
    return;
  }
  property = rEnv.GetValue(propname.c_str(), defaultValue.c_str());
  if (property.empty() && !allowEmpty) {
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
    m_jetInputType = xAOD::JetInput::Type(rEnv.GetValue("Jet.InputType", 9));
    ATH_MSG_INFO( "readConfig(): Loaded property Jet.InputType with value " << (int)m_jetInputType);
  }
  // Remove the item from the table
  rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject("Jet.InputType") );

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

  // Deprecation warning
  std::string prop = rEnv.GetValue("Jet.JVT_WP", "");
  if ( !prop.empty() ) {
     ATH_MSG_WARNING("readConfig(): Found deprecated property name Jet.JVT_WP. Please move to using Jet.JvtWP. Propagating for now.");
     rEnv.SetValue("Jet.JvtWP", prop.c_str());
     rEnv.GetTable()->Remove( rEnv.GetTable()->FindObject("Jet.JVT_WP") );
  }

  //load config file to Properties map  (only booleans for now)
  m_conf_to_prop["StrictConfigCheck"] = "StrictConfigCheck";
  m_conf_to_prop["Btag.enable"] = "UseBtagging";
  m_conf_to_prop["BtagTrkJet.enable"] = "UseBtaggingTrkJet";
  m_conf_to_prop["Ele.CrackVeto"] = "EleCrackVeto";
  m_conf_to_prop["EleBaseline.CrackVeto"] = "EleBaselineCrackVeto";
  m_conf_to_prop["Ele.ForceNoId"] = "EleForceNoId";
  m_conf_to_prop["Ele.DoModifiedId"] = "EleModifiedId";
  m_conf_to_prop["Muon.ForceNoId"] = "MuonForceNoId";
  m_conf_to_prop["Muon.TTVASF"] = "MuonTTVASF";
  m_conf_to_prop["Muon.passedHighPt"] = "MuonRequireHighPtCuts";
  m_conf_to_prop["PhotonBaseline.CrackVeto"] = "PhotonBaselineCrackVeto";
  m_conf_to_prop["Photon.CrackVeto"] = "PhotonCrackVeto";
  m_conf_to_prop["Photon.AllowLate"] = "PhotonAllowLate";

  m_conf_to_prop["FwdJet.doJVT"] = "FwdJetDoJVT";

  m_conf_to_prop["OR.DoBoostedElectron"] = "DoBoostedElectronOR";
  m_conf_to_prop["OR.DoBoostedMuon"] = "DoBoostedMuonOR";
  m_conf_to_prop["OR.DoMuonJetGhostAssociation"] = "ORDoMuonJetGhostAssociation";
  m_conf_to_prop["OR.DoTau"] = "DoTauOR";
  m_conf_to_prop["OR.DoPhoton"] = "DoPhotonOR";
  m_conf_to_prop["OR.DoEleJet"] = "DoEleJetOR";
  m_conf_to_prop["OR.DoElEl"] = "DoElElOR";
  m_conf_to_prop["OR.DoElMu"] = "DoElMuOR";
  m_conf_to_prop["OR.DoMuonJet"] = "DoMuonJetOR";
  m_conf_to_prop["OR.Bjet"] = "DoBjetOR";
  m_conf_to_prop["OR.ElBjet"] = "DoElBjetOR";
  m_conf_to_prop["OR.MuBjet"] = "DoMuBjetOR";
  m_conf_to_prop["OR.TauBjet"] = "DoTauBjetOR";
  m_conf_to_prop["OR.DoFatJets"] = "DoFatJetOR";
  m_conf_to_prop["OR.RemoveCaloMuons"] = "ORRemoveCaloMuons";
  m_conf_to_prop["OR.MuJetApplyRelPt"] = "ORMuJetApplyRelPt";
  m_conf_to_prop["OR.InputLabel"] = "ORInputLabel";
  m_conf_to_prop["Trigger.UpstreamMatching"] = "TriggerUpstreamMatching";

  m_conf_to_prop["SigLep.RequireIso"] = "SigLepRequireIso";
  m_conf_to_prop["SigEl.RequireIso"] = "SigElRequireIso";
  m_conf_to_prop["SigMu.RequireIso"] = "SigMuRequireIso";
  m_conf_to_prop["SigPh.RequireIso"] = "SigPhRequireIso";
  m_conf_to_prop["SigLepPh.IsoCloseByOR"] = "SigLepPhIsoCloseByOR";
  m_conf_to_prop["MET.RemoveOverlappingCaloTaggedMuons"] = "METRemoveORCaloTaggedMuons";
  m_conf_to_prop["MET.DoSetMuonJetEMScale"] = "METDoSetMuonJetEMScale";
  m_conf_to_prop["MET.DoRemoveMuonJets"] = "METDoRemoveMuonJets";
  m_conf_to_prop["MET.DoUseGhostMuons"] = "METUseGhostMuons";
  m_conf_to_prop["MET.DoMuonEloss"] = "METDoMuonEloss";

  m_conf_to_prop["MET.DoTrkSyst"] = "METDoTrkSyst";
  m_conf_to_prop["MET.DoCaloSyst"] = "METDoCaloSyst";

  m_conf_to_prop["Tau.DoTruthMatching"] = "TauDoTruthMatching";
  //

  //
  configFromFile(m_eleBaselinePt, "EleBaseline.Pt", rEnv, 10000.);
  configFromFile(m_eleBaselineEta, "EleBaseline.Eta", rEnv, 2.47);
  configFromFile(m_eleIdBaseline, "EleBaseline.Id", rEnv, "LooseAndBLayerLLH");
  configFromFile(m_eleBaselineIso_WP, "EleBaseline.Iso", rEnv, "None");
  configFromFile(m_eleConfigBaseline, "EleBaseline.Config", rEnv, "None");
  configFromFile(m_eleBaselineCrackVeto, "EleBaseline.CrackVeto", rEnv, false);
  configFromFile(m_force_noElId, "Ele.ForceNoId", rEnv, false);
  //
  configFromFile(m_elePt, "Ele.Et", rEnv, 25000.);
  configFromFile(m_eleEta, "Ele.Eta", rEnv, 2.47);
  configFromFile(m_eleCrackVeto, "Ele.CrackVeto", rEnv, false);
  configFromFile(m_eleIso_WP, "Ele.Iso", rEnv, "FCLoose");
  configFromFile(m_eleIsoHighPt_WP, "Ele.IsoHighPt", rEnv, "FCHighPtCaloOnly");
  configFromFile(m_eleIsoHighPtThresh, "Ele.IsoHighPtThresh", rEnv, 200e3);
  configFromFile(m_eleChID_WP, "Ele.CFT", rEnv, "None"); // Loose is the only one supported for the moment, and not many clients yet.
  configFromFile(m_eleChIso, "Ele.CFTIso", rEnv, true); // use charge ID SFs without iso applied
  configFromFile(m_eleChID_signal, "Ele.CFTSignal", rEnv, !m_eleChID_WP.empty()); // Require ECID as part of the signal lepton definition
  configFromFile(m_doModifiedEleId, "Ele.DoModifiedId", rEnv, false);
  configFromFile(m_eleId, "Ele.Id", rEnv, "TightLLH");
  configFromFile(m_eleConfig, "Ele.Config", rEnv, "None");
  configFromFile(m_eled0sig, "Ele.d0sig", rEnv, 5.);
  configFromFile(m_elez0, "Ele.z0", rEnv, 0.5);
  configFromFile(m_elebaselined0sig, "EleBaseline.d0sig", rEnv, -99.);
  configFromFile(m_elebaselinez0, "EleBaseline.z0", rEnv, 0.5);
  configFromFile(m_eleIdExpert, "Ele.IdExpert", rEnv, false);
  configFromFile(m_EG_corrModel, "Ele.EffNPcorrModel", rEnv, "TOTAL");
  configFromFile(m_EG_corrFNList, "Ele.EffCorrFNList", rEnv, "None");
  configFromFile(m_electronTriggerSFStringSingle, "Ele.TriggerSFStringSingle", rEnv, "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_2018_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0");
  configFromFile(m_eleEffMapFilePath, "Ele.EffMapFilePath", rEnv, "ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/map3.txt");
  configFromFile(m_trig2015combination_singleLep, "Trig.Singlelep2015", rEnv, "e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose || mu20_iloose_L1MU15_OR_mu50"); 
  configFromFile(m_trig2016combination_singleLep, "Trig.Singlelep2016", rEnv, "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium_OR_mu50"); 
  configFromFile(m_trig2017combination_singleLep, "Trig.Singlelep2017", rEnv, "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium_OR_mu50"); 
  configFromFile(m_trig2018combination_singleLep, "Trig.Singlelep2018", rEnv, "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium_OR_mu50"); 
  configFromFile(m_trig2015combination_diLep, "Trig.Dilep2015", rEnv, "e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose || mu20_iloose_L1MU15_OR_mu50 || 2e12_lhloose_L12EM10VH || e17_lhloose_mu14 || e7_lhmedium_mu24 || mu18_mu8noL1 || 2mu10"); 
  configFromFile(m_trig2016combination_diLep, "Trig.Dilep2016", rEnv, "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium_OR_mu50 || 2e17_lhvloose_nod0 || e17_lhloose_nod0_mu14 || e7_lhmedium_nod0_mu24 || e26_lhmedium_nod0_L1EM22VHI_mu8noL1 || mu22_mu8noL1 || 2mu14"); 
  configFromFile(m_trig2017combination_diLep, "Trig.Dilep2017", rEnv, "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium_OR_mu50 || 2e24_lhvloose_nod0 || e17_lhloose_nod0_mu14 || e7_lhmedium_nod0_mu24 || e26_lhmedium_nod0_mu8noL1 || mu22_mu8noL1 || 2mu14"); 
  configFromFile(m_trig2018combination_diLep, "Trig.Dilep2018", rEnv, "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium_OR_mu50 || 2e24_lhvloose_nod0 || e17_lhloose_nod0_mu14 || e7_lhmedium_nod0_mu24 || e26_lhmedium_nod0_mu8noL1 || mu22_mu8noL1 || 2mu14"); 
  configFromFile(m_trig2015combination_multiLep, "Trig.Multi2015", rEnv, "e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose || mu20_iloose_L1MU15_OR_mu50 || 2e12_lhloose_L12EM10VH || e17_lhloose_2e9_lhloose || 2e12_lhloose_mu10 || e12_lhloose_2mu10 || e17_lhloose_mu14 || e7_lhmedium_mu24 || mu18_mu8noL1 || 2mu10 || 3mu6"); 
  configFromFile(m_trig2016combination_multiLep, "Trig.Multi2016", rEnv, "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium_OR_mu50 || 2e17_lhvloose_nod0 || e17_lhloose_nod0_mu14 || e7_lhmedium_nod0_mu24 || e26_lhmedium_nod0_L1EM22VHI_mu8noL1 || e17_lhloose_nod0_2e9_lhloose_nod0 || e12_lhloose_nod0_2mu10 || 2e12_lhloose_nod0_mu10 || mu22_mu8noL1 || 2mu14 || 3mu6"); 
  configFromFile(m_trig2017combination_multiLep, "Trig.Multi2017", rEnv, "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium_OR_mu50 || 2e24_lhvloose_nod0 || e17_lhloose_nod0_mu14 || e7_lhmedium_nod0_mu24 || e26_lhmedium_nod0_mu8noL1 || e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH || e12_lhloose_nod0_2mu10 || 2e12_lhloose_nod0_mu10 || mu22_mu8noL1 || 2mu14 || 3mu6"); 
  configFromFile(m_trig2018combination_multiLep, "Trig.Multi2018", rEnv, "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 || mu26_ivarmedium_OR_mu50 || 2e24_lhvloose_nod0 || e17_lhloose_nod0_mu14 || e7_lhmedium_nod0_mu24 || e26_lhmedium_nod0_mu8noL1 || e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH || e12_lhloose_nod0_2mu10 || 2e12_lhloose_nod0_mu10 || mu22_mu8noL1 || 2mu14 || 3mu6"); 
  configFromFile(m_trig2015combination_diPhoton, "Trig.Diphoton2015", rEnv, "g35_loose_g25_loose");
  configFromFile(m_trig2016combination_diPhoton, "Trig.Diphotonp2016", rEnv, "g35_loose_g25_loose"); 
  configFromFile(m_trig2017combination_diPhoton, "Trig.Diphotonp2017", rEnv, "g35_medium_g25_medium_L12EM20VH"); 
  configFromFile(m_trig2018combination_diPhoton, "Trig.Diphotonp2018", rEnv, "g35_medium_g25_medium_L12EM20VH"); 
  //
  configFromFile(m_muBaselinePt, "MuonBaseline.Pt", rEnv, 10000.);
  configFromFile(m_muBaselineEta, "MuonBaseline.Eta", rEnv, 2.7);
  configFromFile(m_force_noMuId, "Muon.ForceNoId", rEnv, false);
  configFromFile(m_muBaselineIso_WP, "MuonBaseline.Iso", rEnv, "None");
  configFromFile(m_doTTVAsf, "Muon.TTVASF", rEnv, true);
  //
  configFromFile(m_muPt, "Muon.Pt", rEnv, 25000.);
  configFromFile(m_muEta, "Muon.Eta", rEnv, 2.7);
  configFromFile(m_muIso_WP, "Muon.Iso", rEnv, "Loose_VarRad");
  configFromFile(m_muIsoHighPt_WP, "Muon.IsoHighPt", rEnv, "Loose_VarRad");
  configFromFile(m_muIsoHighPtThresh, "Muon.IsoHighPtThresh", rEnv, 200e3);
  configFromFile(m_mud0sig, "Muon.d0sig", rEnv, 3.);
  configFromFile(m_muz0, "Muon.z0", rEnv, 0.5);
  configFromFile(m_mubaselined0sig, "MuonBaseline.d0sig", rEnv, -99.);
  configFromFile(m_mubaselinez0, "MuonBaseline.z0", rEnv, 0.5);
  configFromFile(m_murequirepassedHighPtCuts, "Muon.passedHighPt", rEnv, false);
  //
  configFromFile(m_muCosmicz0, "MuonCosmic.z0", rEnv, 1.);
  configFromFile(m_muCosmicd0, "MuonCosmic.d0", rEnv, 0.2);
  //
  configFromFile(m_badmuQoverP, "BadMuon.qoverp", rEnv, 0.4);
  //
  configFromFile(m_muCalibrationMode, "Muon.CalibrationMode", rEnv, 1); // 0: "setup1"(correctData), 1: "setup2"(additionalMCSys)
  //
  configFromFile(m_photonBaselinePt, "PhotonBaseline.Pt", rEnv, 25000.);
  configFromFile(m_photonBaselineEta, "PhotonBaseline.Eta", rEnv, 2.37);
  configFromFile(m_photonIdBaseline, "PhotonBaseline.Id", rEnv, "Tight");
  configFromFile(m_photonBaselineCrackVeto, "PhotonBaseline.CrackVeto", rEnv, true);
  configFromFile(m_photonBaselineIso_WP, "PhotonBaseline.Iso", rEnv, "None");
  //
  configFromFile(m_photonPt, "Photon.Pt", rEnv, 130000.);
  configFromFile(m_photonEta, "Photon.Eta", rEnv, 2.37);
  configFromFile(m_photonId, "Photon.Id", rEnv, "Tight");
  configFromFile(m_photonIso_WP, "Photon.Iso", rEnv, "FixedCutTight");
  configFromFile(m_photonTriggerName, "Photon.TriggerName", rEnv, "DI_PH_2015_2016_g20_tight_2016_g22_tight_2017_2018_g20_tight_icalovloose_L1EM15VHI"); // for symmetric diphoton triggers
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
  configFromFile(m_tauDoTTM, "Tau.DoTruthMatching", rEnv, false);
  //
  configFromFile(m_jetPt, "Jet.Pt", rEnv, 20000.);
  configFromFile(m_jetEta, "Jet.Eta", rEnv, 2.8);
  configFromFile(m_JvtWP, "Jet.JvtWP", rEnv, "Default"); // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PileupJetRecommendations
  configFromFile(m_JvtPtMax, "Jet.JvtPtMax", rEnv, 60.0e3);
  configFromFile(m_JvtConfig, "Jet.JvtConfig", rEnv, "Moriond2018/");
  configFromFile(m_jetUncertaintiesConfig, "Jet.UncertConfig", rEnv, "rel21/Summer2019/R4_SR_Scenario1_SimpleJER.config"); // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertaintiesRel21Summer2018SmallR
  configFromFile(m_jetUncertaintiesAnalysisFile, "Jet.AnalysisFile", rEnv, "default"); // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertaintiesRel21Summer2018SmallR
  configFromFile(m_jetUncertaintiesCalibArea, "Jet.UncertCalibArea", rEnv, "default"); // Defaults to default area set by tool
  configFromFile(m_jetUncertaintiesPDsmearing, "Jet.UncertPDsmearing", rEnv, false); // for non "SimpleJER" config, run the PDSmear systematics. This are labelled with an __2 if they are being used, but otherwise will have the same tree name as the JET_JER systematic trees.
  configFromFile(m_fatJets, "Jet.LargeRcollection", rEnv, "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"); // set to "None" to turn off large jets 
  configFromFile(m_fatJetUncConfig, "Jet.LargeRuncConfig", rEnv, "rel21/Spring2019/R10_GlobalReduction.config"); // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertaintiesRel21Moriond2018LargeR
  configFromFile(m_fatJetUncVars, "Jet.LargeRuncVars", rEnv, "default"); // do all if not specified
  configFromFile(m_TCCJets, "Jet.TCCcollection", rEnv, "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets"); // set to "None" to turn off TCC jets 
  configFromFile(m_TCCJetUncConfig, "Jet.TCCuncConfig", rEnv, "rel21/Summer2019/R10_Scale_TCC_all.config"); // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertaintiesRel21Summer2019TCC
  configFromFile(m_WtagConfig, "Jet.WtaggerConfig", rEnv, "SmoothedContainedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat");
  configFromFile(m_ZtagConfig, "Jet.ZtaggerConfig", rEnv, "SmoothedContainedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat");
  configFromFile(m_WZTaggerCalibArea, "Jet.WZTaggerCalibArea", rEnv, "SmoothedWZTaggers/Rel21/");
  configFromFile(m_ToptagConfig, "Jet.ToptaggerConfig", rEnv, "JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkInclusive_MC16d_20190405_80Eff.dat");
  configFromFile(m_TopTaggerCalibArea, "Jet.TopTaggerCalibArea", rEnv, "JSSWTopTaggerDNN/Rel21/");
  configFromFile(m_jesConfig, "Jet.JESConfig", rEnv, "JES_MC16Recommendation_Consolidated_EMTopo_Apr2019_Rel21.config");
  configFromFile(m_jesConfigAFII, "Jet.JESConfigAFII", rEnv, "JES_MC16Recommendation_AFII_EMTopo_Apr2019_Rel21.config");
  configFromFile(m_jesConfigJMS, "Jet.JESConfigJMS", rEnv, "JES_JMS_MC16Recommendation_Consolidated_MC_only_EMTopo_July2019_Rel21.config");
  configFromFile(m_jesConfigJMSData, "Jet.JESConfigJMSData", rEnv, "JES_JMS_MC16Recommendation_Consolidated_data_only_EMTopo_Sep2019_Rel21.config");
  configFromFile(m_jesConfigFat, "Jet.JESConfigFat", rEnv, "JES_MC16recommendation_FatJet_Trimmed_JMS_comb_17Oct2018.config");
  configFromFile(m_jesConfigFatData, "Jet.JESConfigFatData", rEnv, "JES_MC16recommendation_FatJet_Trimmed_JMS_comb_3April2019.config");
  configFromFile(m_jesCalibSeq, "Jet.CalibSeq", rEnv, "JetArea_Residual_EtaJES_GSC_Insitu");
  configFromFile(m_jesCalibSeqJMS, "Jet.CalibSeqJMS", rEnv, "JetArea_Residual_EtaJES_GSC");
  configFromFile(m_jesCalibSeqFat, "Jet.CalibSeqFat", rEnv, "EtaJES_JMS");
  //
  configFromFile(m_defaultTrackJets, "TrackJet.Coll", rEnv, "AntiKtVR30Rmax4Rmin02TrackJets");
  configFromFile(m_trkJetPt, "TrackJet.Pt", rEnv, 20000.);
  configFromFile(m_trkJetEta, "TrackJet.Eta", rEnv, 2.8);
  //
  configFromFile(m_badJetCut, "BadJet.Cut", rEnv, "LooseBad");
  //
  configFromFile(m_doFwdJVT, "FwdJet.doJVT", rEnv, false); // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PileupJetRecommendations
  configFromFile(m_fJvtWP, "FwdJet.JvtWP", rEnv, "Tight"); 
  configFromFile(m_fJvtPtMax, "FwdJet.JvtPtMax", rEnv, 120e3);
  configFromFile(m_fJvtEtaMin, "FwdJet.JvtEtaMin", rEnv, 2.5);
  configFromFile(m_fJvtConfig, "FwdJet.JvtConfig", rEnv, "May2020/");
  configFromFile(m_JMScalib, "Jet.JMSCalib", rEnv, false);
  //
  configFromFile(m_useBtagging, "Btag.enable", rEnv, true);
  configFromFile(m_BtagTagger, "Btag.Tagger", rEnv, "DL1");
  configFromFile(m_BtagWP, "Btag.WP", rEnv, "FixedCutBEff_77");
  configFromFile(m_BtagMinPt, "Btag.MinPt", rEnv, -1.); // Not calibrated below 20
  configFromFile(m_BtagTimeStamp, "Btag.TimeStamp", rEnv, "201810", true);
  
  //configFromFile(m_bTaggingCalibrationFilePath, "Btag.CalibPath", rEnv, "xAODBTaggingEfficiency/13TeV/2020-21-13TeV-MC16-CDI-2020-03-11_v1.root");
  configFromFile(m_bTaggingCalibrationFilePath, "Btag.CalibPath", rEnv, "xAODBTaggingEfficiency/13TeV/2019-21-13TeV-MC16-CDI-2019-10-07_v1.root");
  configFromFile(m_BtagSystStrategy, "Btag.SystStrategy", rEnv, "Envelope");

  configFromFile(m_useBtagging_trkJet, "BtagTrkJet.enable", rEnv, true);
  configFromFile(m_BtagTagger_trkJet, "BtagTrkJet.Tagger", rEnv, "MV2c10");
  configFromFile(m_BtagWP_trkJet, "BtagTrkJet.WP", rEnv, "FixedCutBEff_77");
  configFromFile(m_BtagMinPt_trkJet, "BtagTrkJet.MinPt", rEnv, -1.); // Not calibrated below 10
  configFromFile(m_BtagTimeStamp_trkJet, "BtagTrkJet.TimeStamp", rEnv, "None", true);
  configFromFile(m_BtagKeyOverride, "Btag.KeyOverride", rEnv, "", true);
  //
  configFromFile(m_orDoBoostedElectron, "OR.DoBoostedElectron", rEnv, true);
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
  configFromFile(m_orDoElEl, "OR.ElEl", rEnv, false);
  configFromFile(m_orDoElMu, "OR.ElMu", rEnv, false);
  configFromFile(m_orDoMuonJet, "OR.MuonJet", rEnv, true);
  configFromFile(m_orDoBjet, "OR.Bjet", rEnv, false);
  configFromFile(m_orDoElBjet, "OR.ElBjet", rEnv, false);
  configFromFile(m_orDoMuBjet, "OR.MuBjet", rEnv, false);
  configFromFile(m_orDoTauBjet, "OR.TauBjet", rEnv, false);
  configFromFile(m_orApplyRelPt, "OR.MuJetApplyRelPt", rEnv, false);
  configFromFile(m_orMuJetPtRatio, "OR.MuJetPtRatio", rEnv, -999.);
  configFromFile(m_orMuJetTrkPtRatio, "OR.MuJetTrkPtRatio", rEnv, -999.);
  configFromFile(m_orRemoveCaloMuons, "OR.RemoveCaloMuons", rEnv, true);
  configFromFile(m_orMuJetInnerDR, "OR.MuJetInnerDR", rEnv, -999.);
  configFromFile(m_orBtagWP, "OR.BtagWP", rEnv, "FixedCutBEff_85");
  configFromFile(m_orInputLabel, "OR.InputLabel", rEnv, "selected"); //"baseline"
  configFromFile(m_orPhotonFavoured, "OR.PhotonFavoured", rEnv, false);
  configFromFile(m_orBJetPtUpperThres, "OR.BJetPtUpperThres", rEnv, -1.); // upper pt threshold of b-jet in OR in unit of MeV, -1 means no pt threshold
  configFromFile(m_orLinkOverlapObjects, "OR.LinkOverlapObjects", rEnv, false);
  //
  configFromFile(m_orDoFatjets, "OR.DoFatJets", rEnv, false);
  configFromFile(m_EleFatJetDR, "OR.EleFatJetDR", rEnv, -999.);
  configFromFile(m_JetFatJetDR, "OR.JetFatJetDR", rEnv, -999.);
  //
  configFromFile(m_useTRUTH3, "Truth.UseTRUTH3", rEnv, true);
  ///
  configFromFile(m_upstreamTriggerMatching, "Trigger.UpstreamMatching", rEnv, false);
  configFromFile(m_trigMatchingPrefix, "Trigger.MatchingPrefix", rEnv, "", true);
  //
  configFromFile(m_doIsoSignal, "SigLep.RequireIso", rEnv, true);
  configFromFile(m_doElIsoSignal, "SigEl.RequireIso", rEnv, m_doIsoSignal);
  configFromFile(m_doMuIsoSignal, "SigMu.RequireIso", rEnv, m_doIsoSignal);
  configFromFile(m_doPhIsoSignal, "SigPh.RequireIso", rEnv, m_doIsoSignal);
  configFromFile(m_useSigLepForIsoCloseByOR, "SigLepPh.UseSigLepForIsoCloseByOR", rEnv, false);
  configFromFile(m_IsoCloseByORpassLabel, "SigLepPh.IsoCloseByORpassLabel", rEnv, "None");

  //
  configFromFile(m_eleTerm, "MET.EleTerm", rEnv, "RefEle");
  configFromFile(m_gammaTerm, "MET.GammaTerm", rEnv, "RefGamma");
  configFromFile(m_tauTerm, "MET.TauTerm", rEnv, "RefTau");
  configFromFile(m_jetTerm, "MET.JetTerm", rEnv, "RefJet");
  configFromFile(m_muonTerm, "MET.MuonTerm", rEnv, "Muons");
  configFromFile(m_inputMETSuffix, "MET.InputSuffix", rEnv, "", true); // May be empty
  configFromFile(m_outMETTerm, "MET.OutputTerm", rEnv, "Final");
  configFromFile(m_metRemoveOverlappingCaloTaggedMuons, "MET.RemoveOverlappingCaloTaggedMuons", rEnv, true);
  configFromFile(m_metDoSetMuonJetEMScale, "Met.DoSetMuonJetEMScale", rEnv, true);
  configFromFile(m_metDoRemoveMuonJets, "MET.DoRemoveMuonJets", rEnv, true);
  configFromFile(m_metUseGhostMuons, "MET.UseGhostMuons", rEnv, false);
  configFromFile(m_metDoMuonEloss, "MET.DoMuonEloss", rEnv, false);
  configFromFile(m_metGreedyPhotons, "MET.DoGreedyPhotons", rEnv, false);
  configFromFile(m_metVeryGreedyPhotons, "MET.DoVeryGreedyPhotons", rEnv, false);

  configFromFile(m_trkMETsyst, "MET.DoTrkSyst", rEnv, true);
  configFromFile(m_caloMETsyst, "MET.DoCaloSyst", rEnv, false);
  configFromFile(m_trkJetsyst, "MET.DoTrkJetSyst", rEnv, false);
  configFromFile(m_metsysConfigPrefix, "METSys.ConfigPrefix", rEnv, "METUtilities/data17_13TeV/prerec_Jan16");
  configFromFile(m_metJetSelection, "MET.JetSelection", rEnv, "Tight"); // Loose, Tight (default), Tighter, Tenacious
  configFromFile(m_softTermParam, "METSig.SoftTermParam", rEnv, met::Random);
  configFromFile(m_treatPUJets, "METSig.TreatPUJets", rEnv, true);
  configFromFile(m_doPhiReso, "METSig.DoPhiReso", rEnv, false);
  //
  configFromFile(m_prwActualMu2017File, "PRW.ActualMu2017File", rEnv, "GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.actualMu.OflLumi-13TeV-010.root");
  configFromFile(m_prwActualMu2018File, "PRW.ActualMu2018File", rEnv, "GoodRunsLists/data18_13TeV/20190219/purw.actualMu.root");
  configFromFile(m_prwDataSF, "PRW.DataSF", rEnv, 1./1.03); // default for mc16, see: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ExtendedPileupReweighting#Tool_Properties
  configFromFile(m_prwDataSF_UP, "PRW.DataSF_UP", rEnv, 1./0.99); // mc16 uncertainty? defaulting to the value in PRWtool
  configFromFile(m_prwDataSF_DW, "PRW.DataSF_DW", rEnv, 1./1.07); // mc16 uncertainty? defaulting to the value in PRWtool
  configFromFile(m_runDepPrescaleWeightPRW, "PRW.UseRunDependentPrescaleWeight", rEnv, false); // If set to true, the prescale weight is the luminosity-average prescale over the lumiblocks in the unprescaled lumicalc file in the PRW tool.
  configFromFile(m_autoconfigPRWPath, "PRW.autoconfigPRWPath", rEnv, "dev/PileupReweighting/share/");
  configFromFile(m_autoconfigPRWFile, "PRW.autoconfigPRWFile", rEnv, "None");
  configFromFile(m_autoconfigPRWCombinedmode, "PRW.autoconfigPRWCombinedmode", rEnv, false);
  configFromFile(m_autoconfigPRWRPVmode, "PRW.autoconfigPRWRPVmode", rEnv, false);
  configFromFile(m_autoconfigPRWHFFilter, "PRW.autoconfigPRWHFFilter", rEnv, "None");
  configFromFile(m_autoconfigPRWRtags, "PRW.autoconfigPRWRtags", rEnv, "mc16a:r9364_r11505_r11285,mc16c:r9781,mc16d:r10201_r11506_r11279,mc16e:r10724_r11507_r11249,mc16ans:r10740_r10832_r10847_r11008_r11036,mc16dns:r10739_r10833_r10848_r11009_r11037,mc16ens:r10790_r11038_r11265");
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

  //** cache trigger chains for electron matching
  GetTriggerTokens(m_electronTriggerSFStringSingle, v_trigs15_cache_singleEle, v_trigs16_cache_singleEle, v_trigs17_cache_singleEle, v_trigs18_cache_singleEle);

  //** cache trigger chains for matching (both electrons and muons)
  GetTriggerTokens(m_electronTriggerSFStringSingle, v_trigs15_cache_singleEle, v_trigs16_cache_singleEle, v_trigs17_cache_singleEle, v_trigs18_cache_singleEle);

  v_trigs15_cache_singleLep = GetTriggerOR(m_trig2015combination_singleLep);
  v_trigs16_cache_singleLep = GetTriggerOR(m_trig2016combination_singleLep);
  v_trigs17_cache_singleLep = GetTriggerOR(m_trig2017combination_singleLep);
  v_trigs18_cache_singleLep = GetTriggerOR(m_trig2018combination_singleLep);
  v_trigs15_cache_diLep = GetTriggerOR(m_trig2015combination_diLep);
  v_trigs16_cache_diLep = GetTriggerOR(m_trig2016combination_diLep);
  v_trigs17_cache_diLep = GetTriggerOR(m_trig2017combination_diLep);
  v_trigs18_cache_diLep = GetTriggerOR(m_trig2018combination_diLep);
  v_trigs15_cache_multiLep = GetTriggerOR(m_trig2015combination_multiLep);
  v_trigs16_cache_multiLep = GetTriggerOR(m_trig2016combination_multiLep);
  v_trigs17_cache_multiLep = GetTriggerOR(m_trig2017combination_multiLep);
  v_trigs18_cache_multiLep = GetTriggerOR(m_trig2018combination_multiLep);

  return StatusCode::SUCCESS;
}


const std::vector<std::string> SUSYObjDef_xAOD::split(const std::string& s, const std::string& delim) const {
  assert(delim.length() == 1);
  std::vector<std::string> retval;
  retval.reserve(std::count(s.begin(), s.end(), delim[0]) + 1);
  // if only 1 
  if (s.find(delim)==std::string::npos) {
     retval.emplace_back(s);
     return retval;
  }
  // if need to split
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

  if( check_isTighter(m_eleIdBaseline, m_eleId, m_el_id_support) ){
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

  // Jets
  if ( m_jetPt < 20e3 ){
    ATH_MSG_WARNING("Your minimum signal jet pt is less than 20 GeV! Jet.Pt : " << m_jetPt << ". This is NOT supported by the jet group!");
  }

  //Btagging //OR-wp looser than signal-wp?
  if( m_BtagWP.compare(0, m_BtagWP.size()-3, m_orBtagWP, 0, m_BtagWP.size()-3) == 0 ){ //same tagger WP (FixedCutBEff_XX or HybBEff_XX)
    if( atoi(m_BtagWP.substr(m_BtagWP.size()-2, m_BtagWP.size()).c_str()) > atoi(m_orBtagWP.substr(m_orBtagWP.size()-2, m_orBtagWP.size()).c_str()) ){
      ATH_MSG_WARNING("Your btagging configuration is inconsistent!  Signal : " << m_BtagWP << " is looser than OR-Baseline : " << m_orBtagWP);
    }
  }
  if (m_BtagMinPt < 20e3 || m_BtagMinPt_trkJet < 10e3) {
     ATH_MSG_WARNING("You btagging MinPt settings are inconsistent! EM(Topo|PFlow)Jets: not calibrated below 20 GeV (Btag.MinPt: " << m_BtagMinPt/1000. << " GeV set), VRTrackJets: not calibrated below 10 GeV (BtagTrkJet.MinPt: " << m_BtagMinPt_trkJet/1000. << " GeV set).");
  }

  //Taus
  ///baseline vs signal pt check
  if(m_tauPrePtCut > 0 and m_tauPrePtCut > m_tauPt) {
    ATH_MSG_WARNING("Your tau pt configuration is inconsistent! Signal pt cut : " << m_tauPt << " < Baseline (pre) pt cut : " << m_tauPrePtCut);
    if(strict) return StatusCode::FAILURE;
  }
  ///WP check
  if( check_isTighter(m_tauIdBaseline, m_tauId, m_tau_id_support) ){
    ATH_MSG_WARNING("Your tau ID configuration is inconsistent!  Signal : " << m_tauId << " looser than Baseline : " << m_tauIdBaseline);
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
      if (m_tauId == "VeryLoose")   theConfig = "SUSYTools/tau_selection_veryloose.conf";
      else if (m_tauId == "Loose")  theConfig = "SUSYTools/tau_selection_loose.conf";
      else if (m_tauId == "Medium") theConfig = "SUSYTools/tau_selection_medium.conf";
      else if (m_tauId == "Tight")  theConfig = "SUSYTools/tau_selection_tight.conf";
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
  }

  if(m_tauConfigPath != "") { //signal taus

    std::string theConfig = m_tauConfigPath;
    if( m_tauConfigPath=="default" ){
      if (m_tauId == "VeryLoose")   theConfig = "SUSYTools/tau_selection_veryloose.conf";
      else if (m_tauId == "Loose")  theConfig = "SUSYTools/tau_selection_loose.conf";
      else if (m_tauId == "Medium") theConfig = "SUSYTools/tau_selection_medium.conf";
      else if (m_tauId == "Tight")  theConfig = "SUSYTools/tau_selection_tight.conf";
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
  // The PDSmear systematics have been initialised as the second component of the JET_JER systematic, here we'll catch the uncertainties which are to use the PDSmear initialised tool. 
  if (!m_jetUncertaintiesTool.empty() && systConfig.name().find("__2") == std::string::npos) {
    CP::SystematicCode ret = m_jetUncertaintiesTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_VERBOSE("Cannot configure JetUncertaintiesTool for systematic var. " << systConfig.name() );
    } else {
      ATH_MSG_VERBOSE("Configured JetUncertaintiesTool for systematic var. " << systConfig.name() );
    }
  }
  if (!m_jetUncertaintiesPDSmearTool.empty() && systConfig.name().find("__2") != std::string::npos ) {
    CP::SystematicCode ret = m_jetUncertaintiesPDSmearTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_VERBOSE("Cannot configure JetUncertaintiesPDSmearTool for systematic var. " << systConfig.name() );
    } else {
      ATH_MSG_VERBOSE("Configured JetUncertaintiesPDSmearTool for systematic var. " << systConfig.name() );
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
  if (!m_TCCjetUncertaintiesTool.empty()) {
    CP::SystematicCode ret = m_TCCjetUncertaintiesTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_VERBOSE("Cannot configure (TCC)JetUncertaintiesTool for systematic var. " << systConfig.name() );
    } else {
      ATH_MSG_VERBOSE("Configured (TCC)JetUncertaintiesTool for systematic var. " << systConfig.name() );
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
  if (!m_jetFwdJvtEfficiencyTool.empty()) {
    CP::SystematicCode ret = m_jetFwdJvtEfficiencyTool->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_VERBOSE("Cannot configure FJVTEfficiency for systematic var. " << systConfig.name() );
    } else {
      ATH_MSG_VERBOSE("Configured FJVTEfficiency for systematic var. " << systConfig.name() );
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
  if (!m_trigGlobalEffCorrTool_diLep.empty()) {
    CP::SystematicCode ret = m_trigGlobalEffCorrTool_diLep->applySystematicVariation(systConfig);
    for(auto &sfop : m_elecTrigEffTools){
      CP::SystematicCode ret1 = sfop->applySystematicVariation(systConfig); 
      if (ret1 != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure m_elecTrigEffTools (dilepton trigger) for systematic var. " << systConfig.name() ); return ret1; }
    }
    for(auto &sfop : m_elecTrigSFTools){
      CP::SystematicCode ret1 = sfop->applySystematicVariation(systConfig);
      if (ret1 != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure m_elecTrigSFTools (dilepton trigger) for systematic var. " << systConfig.name() ); return ret1; }
    }
    for(auto &sfop : m_muonTrigSFTools){
      CP::SystematicCode ret1 = sfop->applySystematicVariation(systConfig);
      if (ret1 != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure m_muonTrigSFTools (dilepton trigger) for systematic var. " << systConfig.name() ); return ret1; }
    }
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure TrigGlobalEfficiencyCorrectionTool (dilepton trigger) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("TrigGlobalEfficiencyCorrectionTool (dilepton trigger) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!m_trigGlobalEffCorrTool_multiLep.empty()) {
    CP::SystematicCode ret = m_trigGlobalEffCorrTool_multiLep->applySystematicVariation(systConfig);
    for(auto &sfop : m_elecTrigEffTools){
      CP::SystematicCode ret1 = sfop->applySystematicVariation(systConfig); 
      if (ret1 != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure m_elecTrigEffTools (multilep trigger) for systematic var. " << systConfig.name() ); return ret1; }
    }
    for(auto &sfop : m_elecTrigSFTools){
      CP::SystematicCode ret1 = sfop->applySystematicVariation(systConfig);
      if (ret1 != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure m_elecTrigSFTools (multilep trigger) for systematic var. " << systConfig.name() ); return ret1; }
    }
    for(auto &sfop : m_muonTrigSFTools){
      CP::SystematicCode ret1 = sfop->applySystematicVariation(systConfig);
      if (ret1 != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure m_muonTrigSFTools (multilep trigger) for systematic var. " << systConfig.name() ); return ret1; }
    }
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure TrigGlobalEfficiencyCorrectionTool (multi-lepton trigger) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("TrigGlobalEfficiencyCorrectionTool (multi-lepton trigger) configured for systematic var. " << systConfig.name() );
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
  if (!m_elecEfficiencySFTool_isoHighPt.empty()) {
    CP::SystematicCode ret = m_elecEfficiencySFTool_isoHighPt->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgElectronEfficiencyCorrectionTool (iso high-pt) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgElectronEfficiencyCorrectionTool (iso high-pt) configured for systematic var. " << systConfig.name() );
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
      ATH_MSG_ERROR("Cannot configure AsgPhotonEfficiencyCorrectionTool (reco) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgPhotonEfficiencyCorrectionTool (reco) configured for systematic var. " << systConfig.name() );
    }
  }
  if (!isData() && !m_photonIsolationSFTool.empty()) {
    CP::SystematicCode ret = m_photonIsolationSFTool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgPhotonEfficiencyCorrectionTool (iso) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgPhotonEfficiencyCorrectionTool configured (iso) for systematic var. " << systConfig.name() );
    }
  }
  if (!isData() && !m_photonTriggerSFTool.empty()) {
    CP::SystematicCode ret = m_photonTriggerSFTool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure AsgPhotonEfficiencyCorrectionTool (trigger) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("AsgPhotonEfficiencyCorrectionTool configured (trigger) for systematic var. " << systConfig.name() );
    }
  }
  if (!m_trigGlobalEffCorrTool_diPhoton.empty()) {
    CP::SystematicCode ret = m_trigGlobalEffCorrTool_diPhoton->applySystematicVariation(systConfig);
    for(auto &sfop : m_photonTrigEffTools){
      CP::SystematicCode ret1 = sfop->applySystematicVariation(systConfig); 
      if (ret1 != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure m_photonTrigEffTools (diphoton trigger) for systematic var. " << systConfig.name() ); return ret1; }
    }
    for(auto &sfop : m_photonTrigSFTools){
      CP::SystematicCode ret1 = sfop->applySystematicVariation(systConfig);
      if (ret1 != CP::SystematicCode::Ok) { ATH_MSG_ERROR("Cannot configure m_photonTrigSFTools (diphoton trigger) for systematic var. " << systConfig.name() ); return ret1; }
    }
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure TrigGlobalEfficiencyCorrectionTool (diphoton trigger) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("TrigGlobalEfficiencyCorrectionTool (diphoton trigger) configured for systematic var. " << systConfig.name() );
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
  if (!m_btagEffTool_trkJet.empty()) {
    CP::SystematicCode ret = m_btagEffTool_trkJet->applySystematicVariation(systConfig);
    if ( ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure xAODBTaggingEfficiency (track jets) for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured xAODBTaggingEfficiency (track jets) for systematic var. " << systConfig.name() );
    }
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
  for(auto &tool : m_tauTrigEffTool) {
    CP::SystematicCode ret = tool->applySystematicVariation(systConfig);
    if (ret != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Cannot configure " << tool->name() << " for systematic var. " << systConfig.name() );
      return ret;
    } else {
      ATH_MSG_VERBOSE("Configured " << tool->name() << " for systematic var. " << systConfig.name() );
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
	if (sys.basename().find("JET_JER") != std::string::npos && m_jetUncertaintiesPDsmearing == true) {
	  // Add the additional PDSmear JET_JER systematics to the systematics registry if we're using the PDSmear. Otherwise they don't need to be added.
	  std::string JER_systematicName = sys.name();
	  JER_systematicName = std::regex_replace(JER_systematicName, std::regex("__1"), "__2");
	  CP::SystematicVariation sys_JER(JER_systematicName);
	  sysInfoList.push_back(getSystInfo(sys_JER));
	}
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

  if (!m_jetJvtEfficiencyTool.empty()) {
    if ( m_jetJvtEfficiencyTool->isAffectedBySystematic( sys ) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Jet;
      sysInfo.affectedWeights.insert(ST::Weights::Jet::JVT);
    }
  }
  if (!m_jetFwdJvtEfficiencyTool.empty()) {
    if ( m_jetFwdJvtEfficiencyTool->isAffectedBySystematic( sys ) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Jet;
      sysInfo.affectedWeights.insert(ST::Weights::Jet::FJVT);
    }
  }


  if (sys.name().find("__2") == std::string::npos) {
    if (!m_jetUncertaintiesTool.empty()) {
      if ( m_jetUncertaintiesTool->isAffectedBySystematic( CP::SystematicVariation(sys.basename(), CP::SystematicVariation::CONTINUOUS) ) ) {
	sysInfo.affectsKinematics = true;
	sysInfo.affectsType = SystObjType::Jet;
      }
    }
  }
  if (sys.name().find("__2") != std::string::npos) {
    if (!m_jetUncertaintiesPDSmearTool.empty()) {
      if ( m_jetUncertaintiesPDSmearTool->isAffectedBySystematic( CP::SystematicVariation(sys.basename(), CP::SystematicVariation::CONTINUOUS) ) ) {
	sysInfo.affectsKinematics = true;
	sysInfo.affectsType = SystObjType::Jet;
      }
    }
  }


   if (!m_fatjetUncertaintiesTool.empty()) {
    if ( m_fatjetUncertaintiesTool->isAffectedBySystematic( CP::SystematicVariation(sys.basename(), CP::SystematicVariation::CONTINUOUS) ) ) {
      sysInfo.affectsKinematics = true;
      sysInfo.affectsType = SystObjType::Jet;
    }
  }
  if (!m_TCCjetUncertaintiesTool.empty()) {
    if ( m_TCCjetUncertaintiesTool->isAffectedBySystematic( CP::SystematicVariation(sys.basename(), CP::SystematicVariation::CONTINUOUS) ) ) {
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
  if (!m_elecEfficiencySFTool_trigEff_singleLep.empty()) {
    if ( m_elecEfficiencySFTool_trigEff_singleLep->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::Trigger);
    }
  }
  if (!m_trigGlobalEffCorrTool_diLep.empty()) {
    if ( m_trigGlobalEffCorrTool_diLep->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Electron;
      sysInfo.affectedWeights.insert(ST::Weights::Electron::Trigger);
    }
  }
  if (!m_trigGlobalEffCorrTool_multiLep.empty()) {
    if ( m_trigGlobalEffCorrTool_multiLep->isAffectedBySystematic(sys) ) {
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
  if (!m_elecEfficiencySFTool_isoHighPt.empty() ) {
    if ( m_elecEfficiencySFTool_isoHighPt->isAffectedBySystematic(sys) ) {
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
  if (!isData() && !m_photonTriggerSFTool.empty()) {
    if (m_photonTriggerSFTool->isAffectedBySystematic(sys)) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Photon;
      sysInfo.affectedWeights.insert(ST::Weights::Photon::Trigger);
    }
  }
  if (!m_trigGlobalEffCorrTool_diPhoton.empty()) {
    if ( m_trigGlobalEffCorrTool_diPhoton->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Photon;
      sysInfo.affectedWeights.insert(ST::Weights::Photon::Trigger);
    }
  }
  if ( !m_btagEffTool.empty() ) {
    if ( m_btagEffTool->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::BTag;
      sysInfo.affectedWeights.insert(ST::Weights::Jet::Btag);
    }
  }
  if ( !m_btagEffTool_trkJet.empty() ) {
    if ( m_btagEffTool_trkJet->isAffectedBySystematic(sys) ) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::BTag;
      sysInfo.affectedWeights.insert(ST::Weights::Jet::Btag_Track);
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
  for(auto &tool : m_tauTrigEffTool) {
    if(tool->isAffectedBySystematic(sys)) {
      sysInfo.affectsWeights = true;
      sysInfo.affectsType = SystObjType::Tau;
      sysInfo.affectedWeights.insert(ST::Weights::Tau::Trigger);
      break;
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
      case met::JETTRK:
	sysInfo.affectsType = SystObjType::MET_Track;
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
  case MET_Track   : affectedType = "MET_Track";  break;
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

  ATH_MSG_VERBOSE( "After overlap removal: Nel=" << Nel << ", Nmu=" << Nmu << ", Njet=" << Njet );
  */

  return StatusCode::SUCCESS;
}


StatusCode SUSYObjDef_xAOD::NearbyLeptonCorrections(xAOD::ElectronContainer *electrons, xAOD::MuonContainer *muons) const {
  // This getCloseByIsoCorrection is computationally less expensive and actually corrects the isoaltion
  // variables from the contribution of the close by leptons
  if (m_isoCloseByTool->getCloseByIsoCorrection(electrons,muons) != CP::CorrectionCode::Ok) {
        return StatusCode::FAILURE;
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

float SUSYObjDef_xAOD::GetPileupWeightPrescaledTrigger(const std::string & trigger_expr) {
  /* This requires the setup of the PRW tool using your own prescaled lumicalc
     files with syntax "HLT_PrescaledTriggerA.lumicalc.root:HLT_PrescaledTriggerA".
     For further informations, please refer to:
     https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ExtendedPileupReweighting#Prescaling%20MC
  */

  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );

  float pu_weight = m_prwTool->getCombinedWeight(*evtInfo,trigger_expr);

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

float SUSYObjDef_xAOD::GetCorrectedActualInteractionsPerCrossing(bool includeDataSF) {
  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
  return m_prwTool->getCorrectedActualInteractionsPerCrossing( *evtInfo, includeDataSF );
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
  if(!evtInfo->isAvailable<unsigned int>("RandomRunNumber"))
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
  if (theRunNumber<290000) return 2015;
  else if (theRunNumber<320000) return 2016;
  else if (theRunNumber<342000) return 2017;
  return 2018;
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
