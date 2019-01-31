/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TopConfiguration/ConfigurationSettings.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>

namespace top {

ConfigurationSettings* ConfigurationSettings::m_instance = 0;

ConfigurationSettings::ConfigurationSettings() : m_configured(false) {
    registerParameter("ElectronCollectionName", "Name of the Electron container");
    registerParameter("MuonCollectionName", "Name of the Muon container");
    registerParameter("PhotonCollectionName", "Name of the Photon container");
    registerParameter("JetCollectionName", "Name of the Jet container");
    registerParameter("LargeJetCollectionName", "Name of the large-R Jet container");
    registerParameter("LargeJetSubstructure", "Setup to use when applying grooming on the large-R jet");
    registerParameter("TrackJetCollectionName", "Name of the track Jet container","None");
    registerParameter("TauCollectionName", "Name of the Tau container");
    registerParameter("JetGhostTrackDecoName", "Name of the jet decoration that holds the ghost-associated track.", "None");

    registerParameter("TruthCollectionName", "Name of the Truth container");
    registerParameter("TruthElectronCollectionName", "Name of the Truth Electron container", "TruthElectrons" );
    registerParameter("TruthMuonCollectionName", "Name of the Truth Muon container", "TruthMuons");
    registerParameter("TruthPhotonCollectionName", "Name of the Truth Photon container", "None");
    registerParameter("TruthMETCollectionName", "Name of the Truth MET container", "MET_Truth");
    registerParameter("TruthJetCollectionName", "Name of the Truth Jet container");
    registerParameter("TruthLargeRJetCollectionName", "Name of the Truth Large R Jet container", "None");

    registerParameter("applyTTVACut", "Decide if to apply lepton z0/d0 cuts", "True");
    
    registerParameter("GRLDir","Location of GRL File","TopAnalysis");
    registerParameter("GRLFile","Name of GRL File", " ");

    registerParameter("TDPPath","Path to TopDataPreparation file (accessible via PathResolver)","dev/AnalysisTop/TopDataPreparation/XSection-MC15-13TeV.data");

    registerParameter("DumpBtagSystsInSystTrees", "Dump systematics-shifted b-tagging SFs in systematic TTrees, True or False (default: False)","False");

    registerParameter("ElectronID", "Type of electron. Likelihood LooseAndBLayerLH, MediumLH, TightLH","TightLH");
    registerParameter("ElectronIDLoose", "Type of electron for background. Likelihood LooseAndBLayerLH, MediumLH, TightLH","MediumLH");
    registerParameter("ElectronPt", "Electron pT cut for object selection (in MeV). Default 25 GeV.", "25000.");
    registerParameter("EgammaSystematicModel","Egamma Systematic model : FULL_v1 , 1NP_v1 (default)","1NP_v1");
    registerParameter("ElectronIsolation","Isolation to use : Gradient, FCLoose, FCTight, FCHighPtCaloOnly, None","Gradient");
    registerParameter("ElectronIsolationLoose","Isolation to use : Gradient, FCLoose, FCTight, FCHighPtCaloOnly, None","None");
    registerParameter("ElectronIsolationSF", "Force electron isolation SF (e.g. None). EXPERIMENTAL!", " ");
    registerParameter("ElectronIsolationSFLoose", "Force electron isolation SF (e.g. None). EXPERIMENTAL!", " ");
    registerParameter("ElectronVetoLArCrack", "True/False. Set to False to disable LAr crack veto (not recommended).", "True");
    registerParameter("UseElectronChargeIDSelection", "True/False. Switch on/off electron charge ID selection (Default False).", "False");

    registerParameter("PhotonPt", "Photon pT cut for object selection (in MeV). Default 25 GeV.", "25000.");
    registerParameter("PhotonEta", "Absolute Photon eta cut for object selection. Default 2.5.", "2.5");
    registerParameter("PhotonID","Type of photon. Definition to use : Tight, Loose and None.","Tight");
    registerParameter("PhotonIDLoose","Type of photon for background. Definition to use : Tight, Loose, None.","Loose");
    registerParameter("PhotonIsolation","Isolation to use : FixedCutTight, FixedCutLoose, None.","FixedCutTight");
    registerParameter("PhotonIsolationLoose","Isolation to use : FixedCutTight, FixedCutLoose, None.","FixedCutLoose");
    registerParameter("PhotonUseRadiativeZ", "True/False. Set to True to enable photon radiative Z up to 100 GeV.", "False");

    registerParameter("MuonPt", "Muon pT cut for object selection (in MeV). Default 25 GeV.", "25000");
    registerParameter("MuonEta", "Absolute Muon eta cut for object selection. Default 2.5.", "2.5" );
    registerParameter("MuonQuality", "Muon quality cut for object selection. Options are VeryLoose, Loose, Medium (default) and Tight", "Medium");
    registerParameter("MuonQualityLoose", "Muon quality cut for object selection. Options are VeryLoose, Loose, Medium (default) and Tight", "Medium");
    registerParameter("MuonIsolation","Isolation to use : Gradient, GradientLoose, Tight, Loose, LooseTrackOnly, FixedCutTight, FixedCutTightTrackOnly, FixedCutLoose, FCTight, FCLoose, FCTightTrackOnly, PromptLepton, None","Gradient");
    registerParameter("MuonIsolationLoose","Isolation to use : Gradient, GradientLoose, Tight, Loose, LooseTrackOnly, FixedCutTight, FixedCutTightTrackOnly, FixedCutLoose, FCTight, FCLoose, FCTightTrackOnly, PromptLepton, None","None");
    registerParameter("MuonIsolationSF", "Force muon isolation SF (e.g. None). EXPERIMENTAL!", " ");
    registerParameter("MuonIsolationSFLoose", "Force muon isolation SF (e.g. None). EXPERIMENTAL!", " ");
    registerParameter("UseAntiMuons", "Use AntiMuons for fake estimate. Default: false", "false");

    registerParameter("JetPt", "Jet pT cut for object selection (in MeV). Default 25 GeV.", "25000.");
    registerParameter("JetEta", "Absolute Jet eta cut for object selection. Default 2.5.", "2.5" );
    registerParameter("FwdJetAndMET", "Forward jet selection and corresponding MET calculation."
                                               "Default (does nothing on forward jets), fJVT (apply fJVT cut if pT<50GeV and |eta|>2.5), Tight (requires pT>30GeV if |eta|>2.5).", "Default");
    registerParameter("JetPtGhostTracks", "Jet pT threshold for ghost track systematic variations calculation (in MeV). Default 17 GeV.", "17000.");
    registerParameter("JetUncertainties_BunchSpacing",
                      "25ns (default) or 50ns - for JetUncertainties",
                      "25ns");
    registerParameter("JetUncertainties_NPModel","AllNuisanceParameters, CategoryReduction (default), GlobalReduction, StrongReduction - for JetUncertainties","CategoryReduction");
    registerParameter("JetUncertainties_QGFracFile","To specify a root file with quark/gluon fractions,"
                      " in order to reduce FlavourComposition and response uncertainties."
                      " Default: None (i.e. no file is used and default flat 50+/-50% fraction is used).","None");
    registerParameter("JetUncertainties_QGHistPatterns","To specify a pattern for the name of the quark/gluon fractions histograms, or a list of DSIDs which will have their specific histogram."
                      " Two syntaxes are possible, either a single string or a list of DSIDs separated by commas:"
                      "   \"MyQGHisto\" (the histograms with \"MyQGHisto\" in their names will be used for all DSIDs),"
                      "   \"410470,410472,345873,345874,345875\" (for the listed DSIDs, histograms with the processed DSID will be used, while the flat 50+/-50% fraction will be used for the other DSIDs)."
                      " Default: None (i.e. no specific pattern is looked for in the name of the provided histograms).","None");
    registerParameter("LargeRSmallRCorrelations",
                      "Do large-small R jet correlation systematics - True or False (default)",
                      "False");
    registerParameter("JetJERSmearingModel","All (inc. data smearing), All_PseudoData (use MC as pseudo-data), Full (inc. data smearing), Full_PseudoData (use MC as pseudo-data) or Simple (MC only - default)","Simple");
    registerParameter("JetCalibSequence","Jet calibaration sequence, GSC (default) or JMS","GSC");
    registerParameter("JVTinMETCalculation", "Perfom a JVT cut on the jets in the MET recalculation? True (default) or False.", "True" );
    
    registerParameter("JSF",  "Used for top mass analysis, default is 1.0", "1.0");
    registerParameter("bJSF", "Used for top mass analysis, default is 1.0", "1.0");

    registerParameter("LargeRJetPt", "Track Jet pT cut for object selection (in MeV). Default 7 GeV.", "25000.");
    registerParameter("LargeRJetEta", "Absolute large-R jet eta cut for object selection. Default 2.0.", "2.0" );
    registerParameter("LargeRJESUncertaintyConfig",
		      "Variables to be varied with JES uncertainties. Options:D2Beta1,Tau21WTA,Tau32WTA,Split12,Split23,Qw ( default Split23,Tau32WTA)",
		      "Split23,Tau32WTA");
    registerParameter("LargeRJESJMSConfig",
		      "Calibration for large-R JES/JMS. CombMass or CaloMass (default CombMass).",
                      "CombMass");
    registerParameter("LargeRToptaggingConfigFile",
                      "Configuration file for top tagging (default or NFC). default=d23,tau32 (recommended) NFC=m,tau32"
                      "(alternative not optimized on large-R jet containing a truth top)",
                      "default");
    
    registerParameter("TrackJetPt", "Track Jet pT cut for object selection (in MeV). Default 10 GeV.", "10000.");
    registerParameter("TrackJetEta", "Absolute Track Jet eta cut for object selection. Default 2.5.", "2.5" );

    registerParameter("RCJetPt",     "Reclustered Jet pT cut for object selection (in MeV). Default 100000 MeV.", "100000.");
    registerParameter("RCJetEta",    "Reclustered Jet eta cut for object selection. Default 2.0.",   "2.0");
    registerParameter("RCJetTrim",   "Reclustered Jet trimming cut for object selection. Default 0.05.", "0.05");
    registerParameter("RCJetRadius", "Reclustered Jet radius for object selection. Default 1.0",   "1.0");
    registerParameter("UseRCJetSubstructure", "Calculate Reclustered Jet Substructure Variables. Default False",   "False");
    registerParameter("UseRCJetAdditionalSubstructure", "Calculate Additional Reclustered Jet Substructure Variables. Default False",   "False");
    
    registerParameter("UseRCJets",   "Use Reclustered Jets. Default False.", "False");

    registerParameter("VarRCJetPt",        "Reclustered Jet (variable-R) pT cut for object selection (in MeV). Default 100000 MeV.", "100000.");
    registerParameter("VarRCJetEta",       "Reclustered Jet (variable-R) eta cut for object selection. Default 2.0.",   "2.0");
    registerParameter("VarRCJetTrim",      "Reclustered Jet (variable-R) trimming cut for object selection. Default 0.05.", "0.05");
    registerParameter("VarRCJetMaxRadius", "Reclustered Jet (variable-R) max. radius cut for object selection. Default 1.0",   "1.0");
    registerParameter("VarRCJetRho",       "Reclustered Jet (variable-R) list of rho values (). Default 2.",   "2");
    registerParameter("VarRCJetMassScale", "Reclustered Jet (variable-R) list of mass scale values (m_w,m_z,m_h,m_t). By default use all.",   "m_w,m_z,m_h,m_t");
    registerParameter("UseVarRCJets",      "Use Reclustered Jets (Variable-R Jets). Default False.", "False");
    registerParameter("UseVarRCJetSubstructure", "Calculate Variable-R Reclustered Jet Substructure Variables. Default False",   "False");
    registerParameter("UseVarRCJetAdditionalSubstructure", "Calculate Additional Variable-R Reclustered Jet Substructure Variables. Default False",   "False");

    registerParameter("TauPt",
		      "Pt cut applied to both tight and loose taus (in MeV)."
		      "Default 25 GeV.",
		      "25000");
    registerParameter("TauJetIDWP",
		      "Tau jet IDWP (None, Loose, Medium, Tight, LooseNotMedium, LooseNotTight, MediumNotTight, NotLoose)."
		      "Default Medium.",
		      "Medium");
    registerParameter("TauJetIDWPLoose",
		      "Loose Tau jet IDWP (None, Loose, Medium, Tight, LooseNotMedium, LooseNotTight, MediumNotTight, NotLoose)."
		      "Default None.",
		      "Medium");
    registerParameter("TauEleBDTWP",
                      "Tau electron BDT WP (None, Loose, Medium, Tight)."
                      "Default Loose.",
                      "Loose");
    registerParameter("TauEleBDTWPLoose",
                      "Loose Tau electron BDT WP (None, Loose, Medium, Tight)."
                      "Default Loose.",
                      "Loose");
    registerParameter("TauEleOLR",
		      "Apply tau-electron overlap removal (True/False)."
		      "Default False",
		      "False");
    registerParameter("TauEleOLRLoose",
		      "Apply loose tau-electron overlap removal (True/False)."
		      "Default False",
		      "False");
    registerParameter("TauJetConfigFile",
		      "Config file to configure tau selection. "
		      "If anything other than 'Default'" 
		      "then all cuts are taken from the "
		      "config file rather than other options.",
		      "Default");
    registerParameter("TauJetConfigFileLoose",
		      "Config file to configure loose tau selection. "
		      "If anything other than 'Default'" 
		      "then all cuts are taken from the "
		      "config file rather than other options.",
		      "Default");
    registerParameter("ApplyTauMVATES",
                      "Apply new Tau energy calibration based on substructure information and regression. Must be True. Deprecated.",
                      "True");
    
    registerParameter("Systematics", "What to run? Nominal (just the nominal), All(do all systematics) " , "Nominal");

    registerParameter("LibraryNames", "Names of any libraries that need loading");
    registerParameter("UseAodMetaData", "Whether to read xAOD meta-data from input files (default: True)", "True");
    registerParameter("WriteTrackingData", "Whether to generate and store analysis-tracking data (default: True)", "True");
    registerParameter("ObjectSelectionName", "Code used to define objects, e.g. ObjectLoaderStandardCuts");
    registerParameter("OutputFormat", "Format, can be user defined, e.g. top::EventSaverFlatNtuple");
    registerParameter("OutputEvents", "AllEvents (saves all events + decison bits), SelectedEvents (saves only the events passing your cuts)");
    registerParameter("OutputFilename", "The file that will contain the output histograms and trees");
    registerParameter("OutputFileSetAutoFlushZero", "setAutoFlush(0) on EventSaverFlatNtuple for ANALYSISTO-44 workaround. (default: False)","False");
    registerParameter("OutputFileNEventAutoFlush", "Set the number of events after which the TTree cache is optimised, ie setAutoFlush(nEvents). (default: 1000)" , "1000");
    registerParameter("OutputFileBasketSizePrimitive", "Set the TTree basket size for primitive objects (int, float, ...). (default: 4096)" , "4096");
    registerParameter("OutputFileBasketSizeVector", "Set the TTree basket size for vector objects. (default: 40960)" , "40960");   

    registerParameter("EventVariableSaveList", "The list of event variables to save (EventSaverxAODNext only).", "runNumber.eventNumber.eventTypeBitmask.averageInteractionsPerCrossing");
    registerParameter("PhotonVariableSaveList", "The list of photon variables to save (EventSaverxAODNext only).", "pt.eta.phi.m.charge.ptvarcone20.topoetcone20.passPreORSelection");
    registerParameter("ElectronVariableSaveList", "The list of electron variables to save (EventSaverxAODNext only).", "pt.eta.phi.m.charge.ptvarcone20.topoetcone20.passPreORSelection");
    registerParameter("MuonVariableSaveList", "The list of muon variables to save (EventSaverxAODNext only).", "pt.eta.phi.m.author.charge.ptvarcone30.topoetcone20.muonType.passPreORSelection");
    registerParameter("TauVariableSaveList", "The list of tau variables to save (EventSaverxAODNext only).", "pt.eta.phi.m.charge.passPreORSelection");
    registerParameter("JetVariableSaveList", "The list of jet variables to save (EventSaverxAODNext only).", "pt.eta.phi.m.passPreORSelection.btaggingLink.HadronConeExclTruthLabelID");
    registerParameter("BTagVariableSaveList", "The list of b-tag variables to save (EventSaverxAODNext only).", "MV2c20_discriminant");

    registerParameter("NEvents", "The number of events that you want to run on (for testing). If 0 then all events are run.", "0");
    registerParameter("FirstEvent", "The number of events that you want to skip (for testing). If 0 then no events are skipped.", "0");
    registerParameter("PerfStats"," I/O Performance printouts. None, Summary or Full" , "None");
    registerParameter("IsAFII", "Define if you are running over a fastsim sample: True or False", " ");

    registerParameter("FakesMMWeights","Calculate matrix-method weights for fake prompt leptons estimate : True (calculate weights), False (does nothing)", "False");
    registerParameter("FakesMMDir","Directory of files containing efficiencies for fake prompt leptons estimate - default is $ROOTCOREBIN/data/TopFakes", "$ROOTCOREBIN/data/TopFakes");
    registerParameter("FakesMMDebug","Enables debug mode for matrix-method weight calculation: True, False (default)", "False");

    registerParameter("DoTight","Dumps the normal non-\"*_Loose\" trees : Data, MC, Both (default), False", "Both");
    registerParameter("DoLoose","Run Loose selection and dumps the Loose trees : Data (default), MC, Both, False", "Data");

    registerParameter("OverlapRemovalLeptonDef","Special: run overlap removal on : Tight (top default) or Loose (not top default) lepton definitions", "Tight");
    registerParameter("ApplyTightSFsInLooseTree","Special: in Loose trees, calculate lepton SFs with tight leptons only, and considering they are tight: True or False (default)", "False");

    registerParameter("ApplyElectronInJetSubtraction","Subtract electrons close to jets for boosted analysis : True or False(top default)", "False");
    registerParameter("TopPartonHistory","ttbar, tb, Wtb, ttz, ttgamma, False (default)", "False");

    registerParameter("TopParticleLevel", "Perform particle level selection? True or False", "False");
    registerParameter("DoParticleLevelOverlapRemoval",
                      "Perform overlap removal at particle level? True (default), False, or any combination (comma separated) of MuonJet, ElectronJet, JetPhoton",
                      "True");

    registerParameter("PDFInfo","Do you want the PDF info? True (in truth tree), Nominal (save to the nominal tree if passes selection) or False (nothing, default)", "False");
    registerParameter("MCGeneratorWeights","Do you want the OTF-computed MC generator weights (if available)? True (in truth tree), Nominal (save to the nominal tree if passes selection) or False (nothing, default)", "False");
    registerParameter("TruthBlockInfo","Do you want to dump the full Truth block info? True or False", "False");

    registerParameter("TruthElectronPt", "Electron pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.", "25000");
    registerParameter("TruthElectronEta", "Absolute electron eta cut for [Particle Level / Truth] object selection. Default 2.5.", "2.5" );

    registerParameter("TruthMuonPt", "Muon pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.", "25000");
    registerParameter("TruthMuonEta", "Absolute Muon eta cut for [Particle Level / Truth] object selection. Default 2.5.", "2.5" );

    registerParameter("TruthPhotonPt",
                      "Photon pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.",
                      "25000");
    registerParameter("TruthPhotonEta",
                      "Absolute Photon eta cut for [Particle Level / Truth] object selection. Default 2.5.",
                      "2.5" );
    registerParameter("TruthPhotonOrigin",
                      "Potential origin of [Particle Level / Truth] photons. Comma separated list of particle origin values as given by MCTruthClassifier (string names).",
                      "WBoson,ZBoson,SinglePhot,Higgs,HiggsMSSM,WZMSSM,PromptPhot,SUSY,UndrPhot,FSRPhot" );
    registerParameter("TruthPhotonIsolation",
                      "Configuration option for isolation applied to [Particle Level / Truth] photons. "
                      "This can be False / None (isolation requirement disabled), "
                      "True (use default isolation), "
                      "or a configuration in the format `VAR CUT`, where VAR is one of the isolation variables and CUT is the cut applied as `VAR / pt < CUT`.",
                      "ptcone30 0.1");


    registerParameter("TruthJetPt", "Jet pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.", "25000");
    registerParameter("TruthJetEta", "Absolute Jet eta cut for [Particle Level / Truth] object selection. Default 2.5.", "2.5" );

    registerParameter("TruthLargeRJetPt", "Large R Jet pT cut for [Particle Level / Truth] object selection (in MeV). Default 25 GeV.", "25000");
    registerParameter("TruthLargeRJetEta", "Absolute Large R Jet eta cut for [Particle Level / Truth] object selection. Default 2.5.", "2.5" );

    registerParameter("LHAPDFSets", "List of PDF sets to calculate weights for, seperated by spaces, use LHAPDF names e.g CT10nlo NNPDF30_nlo_as_0118 MMHT2014nlo68cl", " " );
    registerParameter("LHAPDFEventWeights", "Save per event weights for all PDF sets/members: True (lots of info in truth tree!), Nominal (save to the nominal tree if passes selection) or False (nothing, default).", "False" );
    registerParameter("LHAPDFBaseSet", "Base PDF set used to recalculate XF1,XF2 values if they are zero. Will be added to LHAPDFSets.", " " );

    registerParameter("BTaggingWP",
                      "b-tagging WPs to use in the analysis, separated by commas."
                      " The format should follow the convention of the b-tagging CP group, e.g. FixedCutBEff_60, FlatBEff_77, Continuous, etc."
                      " For fixed-cut WPs, the simpler format 60%, instead of FixedCutBEff_60, is also tolerated."
                      " The specified WPs which are calibrated for all flavours will have scale-factors computed."
                      " By default, no WP is used.",
                      " ");

    registerParameter("BTaggingSystExcludedFromEV",
                      "User-defined list of b-tagging systematics to be dextracted from eigenvector decomposition, separated by semi-colons (none by default)",
                      "none");

    registerParameter("BTaggingCalibrationB",
                      "The calibration to use for the b-tagging SFs (B-jets)."
                      " Default 'default'",
                      "default");

    registerParameter("BTaggingCalibrationC",
                      "The calibration to use for the b-tagging SFs (C- and T-jets)."
                      " Default 'default'",
                      "default");

    registerParameter("BTaggingCalibrationLight",
                      "The calibration to use for the b-tagging SFs (Light-jets)."
                      " Default 'default'",
                      "default");

    registerParameter("PRWConfigFiles",    "List of PU config files, seperated by spaces (nothing by default) - Not compatible with FS/AF options", " ");
    registerParameter("PRWConfigFiles_FS", "List of PU config files only for full sim samples, seperated by spaces (nothing by default)", " ");
    registerParameter("PRWConfigFiles_AF", "List of PU config files only for fast sim samples, seperated by spaces (nothing by default)", " ");
    registerParameter("PRWActualMu_FS", "List of actual mu files for full sim samples, seperated by spaces (nothing by default)", " ");
    registerParameter("PRWActualMu_AF", "List of actual mu files only for fast sim samples, seperated by spaces (nothing by default)", " ");
    registerParameter("PRWLumiCalcFiles", "List of PU lumicalc files, seperated by spaces (nothing by default)", " ");
    registerParameter("PRWUseGRLTool", "Pass the GRL tool to the PU reweighting tool (False by default)", "False");
    registerParameter("PRWMuDependent",
                      "Use mu dependent random run numbers for MC. "
                      "True or False (default True)",
                      "True");
    registerParameter("PRWCustomScaleFactor",
                      "Specify custom scale-factor and up/down variations, for specific studies."
                      "Format is \'nominal:up:down\'."
                      "If nothing is set, the default values will be used (recommended).",
                      " ");
    registerParameter("PRWUnrepresentedDataTolerance", "Specify value between 0 and 1 to represent acceptable fraction of unrepresented data in PRW [default: 0.05]", "0.05");

    registerParameter("MuonTriggerSF", "Muon trigger SFs to calculate", "HLT_mu20_iloose_L1MU15_OR_HLT_mu50");

    registerParameter("KLFitterTransferFunctionsPath","Select the transfer functions to use","mc12a/akt4_LCtopo_PP6");
    registerParameter("KLFitterOutput","Select the KLFitter output (FULL, FITTEDTOPS_ONLY, JETPERM_ONLY)","FULL");
    registerParameter("KLFitterJetSelectionMode","kLeadingThree , kLeadingFour , kLeadingFive , kLeadingSix , kLeadingSeven , kBtagPriorityThreeJets , kBtagPriorityFourJets , kBtagPriorityFiveJets, kBtagPrioritySixJets , kBtagPrioritySevenJets","kBtagPriorityFourJets");
    registerParameter("KLFitterBTaggingMethod","Recommend use kNotag or kVetoNoFit - see KLFitter TWiki","kNotag");
    registerParameter("KLFitterLH", "Select likelihood depending on signal, ttbar, ttbar_angles, ttH, ttZTrilepton, ttbar_AllHadronic, ttbar_BoostedLJets", "ttbar");
    registerParameter("KLFitterTopMassFixed","Fix the mass of the top quark? True or False","True");
    registerParameter("KLFitterSaveAllPermutations","Save All permutations to the output file (False will save only the best)","False");
    registerParameter("KLFitterFailOnLessThanXJets","Fail if kLeadingX or kBtagPriorityXJets is set and the number of jets in the event is less than X (Default is False)","False");

    registerParameter("DynamicKeys", "Additional dynamic key list seperated by ,", "");

    registerParameter("OverlapRemovalProcedure", "Overlap removal procedure to be used. Options include:"
                      " recommended [default], jetmuApplyRelPt, harmonized,"
                      " Boosted, BoostedSlidingDREl, BoostedSlidingDRMu, BoostedSlidingDRElMu, noTauJetOLR",
                      "recommended"
                     );

    registerParameter("OverlapRemovalSlidingInnerDRel", "Overlap removal inner radius to be used for electron SlidingDR ,", "0.2");
    registerParameter("OverlapRemovalSlidingInnerDRmu", "Overlap removal inner radius to be used for muon SlidingDR ,", "0.2");

    registerParameter("LargeJetOverlapRemoval", "Perform overlap removal including large-R jets. True or False (default: False).", "False");

    registerParameter("HLLHC",
                     "Set to run HL-LHC studies,"
                     "True or False (default False)",
                     "False");

    registerParameter("HLLHCFakes",
                      "Set to enable Fakes HL-LHC studies,"
                      "True or False (default False)",
                      "False");

    registerParameter("SaveBootstrapWeights", "Set to true in order to save Poisson bootstrap weights,"
		      "True or False (default False)", "False");
    
    registerParameter("NumberOfBootstrapReplicas", "Define integer number of replicas to be stored with bootstrapping, "
		      "Default 100", "100");

    registerParameter("UseEventLevelJetCleaningTool", "Switch to turn on event-level jet cleaning tool (for testing), True or False (default False)", "False");

    registerParameter("UseGlobalLeptonTriggerSF", "Switch to activate event-level trigger scale factors allowing multiple OR of single-, di-, tri- lepton triggers, True or False (default False)", "False");
    registerParameter("ElectronTriggers",         "Trigger list for GlobalLeptonTriggerSF - Format as 2015@trig1,trig2 2016@trig3,trig4 : Separate periods defined with @ using whitespace, triggers with comma (default: None)", "None");
    registerParameter("ElectronTriggersLoose",    "Trigger list for GlobalLeptonTriggerSF - Format as 2015@trig1,trig2 2016@trig3,trig4 : Separate periods defined with @ using whitespace, triggers with comma (default: None)", "None");
    registerParameter("MuonTriggers",             "Trigger list for GlobalLeptonTriggerSF - Format as 2015@trig1,trig2 2016@trig3,trig4 : Separate periods defined with @ using whitespace, triggers with comma (default: None)", "None");
    registerParameter("MuonTriggersLoose",        "Trigger list for GlobalLeptonTriggerSF - Format as 2015@trig1,trig2 2016@trig3,trig4 : Separate periods defined with @ using whitespace, triggers with comma (default: None)","None");

    registerParameter("KillExperimental", "Disable some specific experimental feature.", " ");
}

ConfigurationSettings* ConfigurationSettings::get() {
    if (!m_instance)
        m_instance = new ConfigurationSettings();

    return m_instance;
}

void ConfigurationSettings::loadFromFile(const std::string& filename) {
    std::ifstream input(filename.c_str());

    if (!input) {
        std::cout << "Configuration file does not exist, " << filename << std::endl;
        std::cout << "Can't continue" << std::endl;
        exit(1);
    }

    struct SelectionData {
        std::string name;
        bool isSub;
        std::vector<std::string> cuts;
    };
    std::string line;
    std::vector<SelectionData> selections;

    //for the key-value pairs
    while (std::getline( input, line )) {
        std::string newstring(line);
        //std::cout << newstring << '\n';

        if (newstring.find("#") != std::string::npos)
            newstring = newstring.substr(0, newstring.find("#"));

	// remove (multiple) spaces hanging around relevant information
        boost::algorithm::trim_all(newstring);
        if (newstring.empty())
            continue;

        // handle start of a (sub)selection (implies end of key-value section)
        if (boost::algorithm::starts_with(newstring, "SELECTION ")) {
            selections.push_back({newstring.substr(10), false, {}});
            continue;
        }
        if (boost::algorithm::starts_with(newstring, "SUB ")) {
            selections.push_back({newstring.substr(4), true, {}});
            continue;
        }

        if (!selections.empty()) {
            // read body of (sub)selection
            auto & sel = selections.back();
            if (boost::algorithm::starts_with(newstring, ". ")) {
                // source another (sub)selection here
                auto subselName = newstring.substr(2);
                auto subselIt = std::find_if(selections.rbegin(), selections.rend(),
                        [subselName](SelectionData const & sel){ return (subselName == sel.name); });
                if (subselIt == selections.rend())
                    throw std::invalid_argument("ConfigurationSettings: unknown selection: " + subselName);
                sel.cuts.insert(sel.cuts.end(), subselIt->cuts.begin(), subselIt->cuts.end());
            }
            else {
                sel.cuts.push_back(newstring);
            }
            continue;
        }

        // parse key-value pair
        std::istringstream liness(newstring);
        std::string key;
        std::string value;

        std::getline(liness, key, ' '); //to the space
        std::getline(liness, value); //to the end of the line

        auto its = strings_.find(key);
        if (its != strings_.end()) {
            its->second.m_data = value;
            its->second.m_set = true;
        }

	//// add dynamic keys
	if("DynamicKeys" == key) {
	  std::vector<std::string> listofkeys;
	  std::string separator = ",";
	  std::string::size_type start = 0, end = 0;
	  while ((end = value.find(separator, start)) != std::string::npos) {
	    std::string token = value.substr(start, end - start);
	    if(token.size()) listofkeys.push_back(token);
	    start = end + 1;
	  }

	  std::string lasttoken = value.substr(start);
	  if(lasttoken.size()) listofkeys.push_back(lasttoken);

	  for(auto par : listofkeys) {
	    registerParameter(par, "Dynamic parameter", "");
	  }
	}
    }

    for (auto & sel : selections) {
        if (sel.isSub)
            continue;
        m_selections.push_back({sel.name, sel.cuts});
    }

    {
       auto const & it = strings_.find("KillExperimental");
       m_killedFeatures.clear();
       if (it != strings_.end() && it->second.m_set) {
          std::string strValue(it->second.m_data);
          boost::trim(strValue);
          boost::split(m_killedFeatures, strValue, boost::is_any_of(" "));
       }
    }

    input.close();
    m_configured = true;
}

  void ConfigurationSettings::registerParameter(const std::string& name, const std::string& message, const std::string& default_val) {
    StringData data;
    data.m_data = default_val;
    data.m_human_explanation = message;
    data.m_set = (default_val.empty() ? false : true);
    strings_[name] = data;
}

const std::string& ConfigurationSettings::value(const std::string& key) const {
    //This class never has loadFromFile called
    if (!m_configured) {
        std::string message = "ConfigurationSettings: Not correctly configured\n";
        message.append("You need to call top::ConfigurationSettings::get()->loadFromFile(filename)\n");
        message.append("Early in your program\n");
        throw std::invalid_argument(message);
    }

    std::map<std::string, StringData>::const_iterator its = strings_.find(key);
    //The string is not in the map
    if (its == strings_.end()) {
        throw std::invalid_argument("ConfigurationSettings: The variable doesn't exist in the code " + key);
    }

    //In the map, but never set to anything
    if (!its->second.m_set) {
        throw std::invalid_argument("ConfigurationSettings: You never set a value for " + key);
    }

    return its->second.m_data;
}

bool ConfigurationSettings::configured() const {
    return m_configured;
}

const std::map<std::string, StringData>& ConfigurationSettings::stringData() const {
    return strings_;
}

const std::vector<SelectionConfigurationData> ConfigurationSettings::selections() const {
    return m_selections;
}

bool ConfigurationSettings::retrieve(std::string const & key, bool & value) const {
    using boost::trim;
    using boost::equals;
    using boost::iequals;
    auto stringValue = ConfigurationSettings::get()->value(key);
    trim(stringValue);
    if (equals(stringValue, "")) {
        return false;
    }
    if (iequals(stringValue, "false") or iequals(stringValue, "0") or iequals(stringValue, "n") or iequals(stringValue, "no") or iequals(stringValue, "off")) {
        value = false;
        return true;
    }
    if (iequals(stringValue, "true") or iequals(stringValue, "1") or iequals(stringValue, "y") or iequals(stringValue, "yes") or iequals(stringValue, "on")) {
        value = true;
        return true;
    }
    throw std::invalid_argument(std::string("expected boolean value for configuration setting ") + key);
}

bool ConfigurationSettings::feature(std::string const & name) const {
   /* We search a list of strings, not a particularly efficient implementation.
    * If need be, we could abuse the aux registry and use integers instead of
    * strings. Anyhow, in most cases, the list should be empty. */
   return (m_killedFeatures.empty() || std::find(m_killedFeatures.begin(), m_killedFeatures.end(), name) == m_killedFeatures.end());
}

}

std::ostream& operator<<(std::ostream& os, const top::SelectionConfigurationData& data) {
    os << " - " << data.m_name << "\n";
    for (const auto& cutname : data.m_cutnames)
        os << "    " << cutname << "\n";

    return os;
}

std::ostream& operator<<(std::ostream& os, const top::ConfigurationSettings& settings) {
    for (std::map<std::string, top::StringData >::const_iterator its = settings.stringData().begin(); its != settings.stringData().end(); ++its) {
        std::stringstream s;
        s << "\"" << its->first << "\"";

        std::stringstream s2;
        s2 << "\"" << its->second.m_data << "\"";
        os << std::setw(40) << std::left << s.str() << " : " << std::setw(35) << s2.str() << " - " << std::right << its->second.m_human_explanation << "\n";
    }

    //for (const auto& selection : settings.selections())
    //    os << selection << "\n";

    return os;
}
