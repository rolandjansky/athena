# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#====================================================================
# Common file used for TOPQ truth tools / augmentation
# Call with:
#  from DerivationFrameworkTop.TOPQCommonTruthTools import *
#  TOPQXSequence += TOPQCommonTruthKernel
#====================================================================

#================================
# IMPORTS
#================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *

def setup(ToolSvc):

  augmentationTools=[]

  #==============================================================================
  # Set up the MCTruthClassifier
  #============================================================================== 
  from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
  TOPQClassifier = MCTruthClassifier( name                      = "TOPQClassifier",
                                       ParticleCaloExtensionTool = "" ) 
  ToolSvc += TOPQClassifier
  print "TOPQClassifier: ", TOPQClassifier

  #===============================================================================
  # Add Decoration Tool to Dress the Main Truth Collection with the Classification
  #===============================================================================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/TruthClassificationDecorator.cxx 
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
  TOPQClassificationDecorator = DerivationFramework__TruthClassificationDecorator(
                                 name              = "TOPQClassificationDecorator",
                                 ParticlesKey      = "TruthParticles",
                                 MCTruthClassifier = TOPQClassifier)  
  ToolSvc += TOPQClassificationDecorator
  augmentationTools.append(TOPQClassificationDecorator)
  print "TOPQClassificationDecorator: ", TOPQClassificationDecorator
   
  #==============================================================================
  # Schedule the tool for adding new truth collection
  #==============================================================================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/TruthCollectionMaker.cxx
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/TruthCollectionMakerTau.cxx
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMakerTau

  #===========
  # TruthMuons
  #===========
  TOPQMuonTool = DerivationFramework__TruthCollectionMaker(
                   name                    = "TOPQMuonTool",
                   NewCollectionName       = "TruthMuons",
                   ParticleSelectionString = "(abs(TruthParticles.pdgId) == 13) && (TruthParticles.status == 1) && (TruthParticles.barcode < 200000) " )
  ToolSvc += TOPQMuonTool
  print "TOPQMuonTool: ", TOPQMuonTool 
  augmentationTools.append(TOPQMuonTool)

  #===============
  # TruthElectrons
  #===============
  TOPQElectronTool = DerivationFramework__TruthCollectionMaker(
                       name                    = "TOPQElectronTool",
                       NewCollectionName       = "TruthElectrons",
                       ParticleSelectionString = "(abs(TruthParticles.pdgId) == 11) && (TruthParticles.status == 1) && (TruthParticles.barcode < 200000)" )
  ToolSvc += TOPQElectronTool
  augmentationTools.append(TOPQElectronTool)
  print "TOPQElectronTool: ",TOPQElectronTool 

  #=============
  # TruthPhotons
  #=============   
  TOPQPhotonTool = DerivationFramework__TruthCollectionMaker(
                     name                    = "TOPQPhotonTool",
                     NewCollectionName       = "TruthPhotons",
                     ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1) && (TruthParticles.barcode < 200000)" )
  ToolSvc += TOPQPhotonTool
  augmentationTools.append(TOPQPhotonTool)
  print "TOPQPhotonTool: ", TOPQPhotonTool

  #===============
  # TruthNeutrinos
  #===============
  TOPQneutrinoexpression = "(abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.status == 1) && (TruthParticles.barcode < 200000)"
  TOPQNeutrinoTool = DerivationFramework__TruthCollectionMaker(
                       name                    = "TOPQNeutrinoTool",
                       NewCollectionName       = "TruthNeutrinos",
                       ParticleSelectionString = TOPQneutrinoexpression )
  ToolSvc += TOPQNeutrinoTool
  augmentationTools.append(TOPQNeutrinoTool)
  print "TOPQNeutrinoTool: ", TOPQNeutrinoTool

  #==========
  # TruthTaus
  #==========
  TOPQTauTool = DerivationFramework__TruthCollectionMakerTau(
                  name                    = "TOPQTauTool",
                  NewCollectionName       = "TruthTaus",
                  MCTruthClassifier       = TOPQClassifier,
                  RunClassifier           = True)
  ToolSvc += TOPQTauTool 
  augmentationTools.append(TOPQTauTool)
  print "TOPQTauTool: ", TOPQTauTool

  #==============================================================================
  # TRUTH DRESSING
  #==============================================================================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/TruthDressingTool.cxx
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthDressingTool
  TOPQMuonDressingTool = DerivationFramework__TruthDressingTool(
                           name                  = "TOPQMuonDressingTool",
                           dressParticlesKey     = "TruthMuons",
                           usePhotonsFromHadrons = False,
                           dressingConeSize      = 0.1,
                           particleIDsToDress    = [13],
                           useAntiKt = True )
  ToolSvc += TOPQMuonDressingTool
  augmentationTools.append(TOPQMuonDressingTool)
  print "TOPQMuonDressingTool: ", TOPQMuonDressingTool

  TOPQElectronDressingTool = DerivationFramework__TruthDressingTool(
                               name                  = "TOPQElectronDressingTool",
                               dressParticlesKey     = "TruthElectrons",
                               usePhotonsFromHadrons = False,
                               dressingConeSize      = 0.1,
                               particleIDsToDress    = [11],
                               useAntiKt             = True )
  ToolSvc += TOPQElectronDressingTool
  augmentationTools.append(TOPQElectronDressingTool)
  print "TOPQElectronDressingTool: ", TOPQElectronDressingTool

  #==============================================================================
  # TRUTH ISOLATION
  #==============================================================================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/TruthIsolationTool.cxx
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthIsolationTool
  TOPQElectronIsolationTool1 = DerivationFramework__TruthIsolationTool(
                                 name                   = "TOPQElectronIsolationTool1",
                                 isoParticlesKey        = "TruthElectrons",
                                 allParticlesKey        = "TruthParticles",
                                 particleIDsToCalculate = [11],
                                 IsolationConeSizes     = [0.2],
                                 IsolationVarNamePrefix = 'etcone',
                                 ChargedParticlesOnly   = False )
  ToolSvc += TOPQElectronIsolationTool1
  augmentationTools.append(TOPQElectronIsolationTool1)
  print "TOPQElectronIsolationTool1: ", TOPQElectronIsolationTool1
   
  TOPQElectronIsolationTool2 = DerivationFramework__TruthIsolationTool(
                                 name                   = "TOPQElectronIsolationTool2",
                                 isoParticlesKey        = "TruthElectrons",
                                 allParticlesKey        = "TruthParticles",
                                 particleIDsToCalculate = [11],
                                 IsolationConeSizes     = [0.3],
                                 IsolationVarNamePrefix = 'ptcone',
                                 ChargedParticlesOnly   = True )
  ToolSvc += TOPQElectronIsolationTool2
  augmentationTools.append(TOPQElectronIsolationTool2)
  print "TOPQElectronIsolationTool2: ", TOPQElectronIsolationTool2
   
  TOPQMuonIsolationTool1 = DerivationFramework__TruthIsolationTool(
                             name                   = "TOPQMuonIsolationTool1",
                             isoParticlesKey        = "TruthMuons",
                             allParticlesKey        = "TruthParticles",
                             particleIDsToCalculate = [13],
                             IsolationConeSizes     = [0.2],
                             IsolationVarNamePrefix = 'etcone',
                             ChargedParticlesOnly   = False )
  ToolSvc += TOPQMuonIsolationTool1
  augmentationTools.append(TOPQMuonIsolationTool1)
  print "TOPQMuonIsolationTool1: ", TOPQMuonIsolationTool1
   
  TOPQMuonIsolationTool2 = DerivationFramework__TruthIsolationTool(
                            name                   = "TOPQMuonIsolationTool2",
                            isoParticlesKey        = "TruthMuons",
                            allParticlesKey        = "TruthParticles",
                            particleIDsToCalculate = [13],
                            IsolationConeSizes     = [0.3],
                            IsolationVarNamePrefix = 'ptcone',
                            ChargedParticlesOnly   = True )
  ToolSvc += TOPQMuonIsolationTool2
  augmentationTools.append(TOPQMuonIsolationTool2)
  print "TOPQMuonIsolationTool2: ", TOPQMuonIsolationTool2

  TOPQPhotonIsolationTool1 = DerivationFramework__TruthIsolationTool(
                               name                   = "TOPQPhotonIsolationTool1",
                               isoParticlesKey        = "TruthPhotons",
                               allParticlesKey        = "TruthParticles",
                               particleIDsToCalculate = [22],
                               IsolationConeSizes     = [0.2],
                               IsolationVarNamePrefix = 'etcone',
                               ChargedParticlesOnly   = False )
  ToolSvc += TOPQPhotonIsolationTool1
  augmentationTools.append(TOPQPhotonIsolationTool1)
  print "TOPQPhotonIsolationTool1: ", TOPQPhotonIsolationTool1
   
  TOPQPhotonIsolationTool2 = DerivationFramework__TruthIsolationTool(
                               name                   = "TOPQPhotonIsolationTool2",
                               isoParticlesKey        = "TruthPhotons",
                               allParticlesKey        = "TruthParticles",
                               particleIDsToCalculate = [22],
                               IsolationConeSizes     = [0.3],
                               IsolationVarNamePrefix = 'ptcone',
                               ChargedParticlesOnly   = True )
  ToolSvc += TOPQPhotonIsolationTool2
  augmentationTools.append(TOPQPhotonIsolationTool2)
  print "TOPQPhotonIsolationTool2: ", TOPQPhotonIsolationTool2

  #==============================================================================
  # BACKGROUND ELECTRON DECORATION TYPE/ORIGIN
  #==============================================================================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkEGamma/trunk/src/BkgElectronClassification.cxx
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification 
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (
                                    name = "BkgElectronClassificationTool",
                                    MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  augmentationTools.append(BkgElectronClassificationTool)
  print "BkgElectronClassificationTool: ", BkgElectronClassificationTool


  #==============================================================================
  # BOOSTED TOP PAIR DECORATION
  #==============================================================================
  # /PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/BoostedHadTopAndTopPairFilterTool.cxx
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/BoostedHadTopAndTopPairAugmentation.cxx
  #from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__BoostedHadTopAndTopPairFilterTool
  #TOPQboostedtopfiltertool = DerivationFramework__BoostedHadTopAndTopPairFilterTool(
  #                             name = "TOPQBoostedHadTopAndTopPairFilterTool",
  #                             tHadPtCut  = 2000000.0, #cut on hadronic tops in MeV
  #                             tPairPtCut = 3500000.0) #cut on ttbar system in MeV
  #ToolSvc += TOPQboostedtopfiltertool

  #from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__BoostedHadTopAndTopPairFilterAugmentation
  #TOPQBoostedHadTopAndTopPairFilterAugmentation = DerivationFramework__BoostedHadTopAndTopPairFilterAugmentation(name = "TOPQBoostedHadTopAndTopPairFilterAugmentation")
  #TOPQBoostedHadTopAndTopPairFilterAugmentation.FilterTool = TOPQboostedtopfiltertool
  #ToolSvc += TOPQBoostedHadTopAndTopPairFilterAugmentation
  #augmentationTools.append(TOPQBoostedHadTopAndTopPairFilterAugmentation)
  #print "TOPQBoostedHadTopAndTopPairFilterAugmentationTool: ", TOPQBoostedHadTopAndTopPairFilterAugmentation


  #==============================================================================
  # HEAVY FLAVOR DECORATION
  #==============================================================================
  # /PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TTbarPlusHeavyFlavorFilterTool.cxx
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TopHeavyFlavorFilterAugmentation.cxx
  from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TTbarPlusHeavyFlavorFilterTool
  TOPQtthffiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("TOPQTTbarPlusHeavyFlavorFilterTool")
  ToolSvc += TOPQtthffiltertool
   
  from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TopHeavyFlavorFilterAugmentation
  TOPQTopHFFilterAugmentation = DerivationFramework__TopHeavyFlavorFilterAugmentation(name = "TOPQTopHFFilterAugmentation")
  TOPQTopHFFilterAugmentation.FilterTool = TOPQtthffiltertool
  ToolSvc += TOPQTopHFFilterAugmentation
  augmentationTools.append(TOPQTopHFFilterAugmentation)
  print "TOPQTopHFFilterAugmentationTool: ", TOPQTopHFFilterAugmentation

  #==============================================================================
  # HEAVY FLAVOR DECORATIONS (ttbar)
  #==============================================================================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/HadronOriginClassifier.cxx
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/HadronOriginDecorator.cxx
  # list of ttbar samples by mc_channel_number
  TOPQDSIDList=[
    410000,
    410001,
    410002,
    410003,
    410004,
    410007,
    410008,
    410009,
    301528,
    301529,
    301530,
    301531,
    301532,
    303722,
    303723,
    303724,
    303725,
    303726,
    407009,
    407010,
    407011,
    407012,
    410120,
    410121,
    426090,
    426091,
    426092,
    426093,
    426094,
    426095,
    426096,
    426097,
    429007,
  ]

  import PyUtils.AthFile as af
  from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
  f = af.fopen(athenaCommonFlags.PoolAODInput()[0])
  if len(f.mc_channel_number) > 0:
    if(int(f.mc_channel_number[0]) in TOPQDSIDList):
      from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HadronOriginClassifier
      TOPQhadronorigintool = DerivationFramework__HadronOriginClassifier("TOPQHadronOriginClassifier",DSID=int(f.mc_channel_number[0]))
      ToolSvc += TOPQhadronorigintool
      print "TOPQhadronorigintool: ", TOPQhadronorigintool
      from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HadronOriginDecorator
      TOPQhadronorigindecorator = DerivationFramework__HadronOriginDecorator(name = "TOPQHadronOriginDecorator")
      TOPQhadronorigindecorator.ToolName = TOPQhadronorigintool
      ToolSvc += TOPQhadronorigindecorator
      print "TOPQhadronorigindecorator: ", TOPQhadronorigindecorator
      augmentationTools.append(TOPQhadronorigindecorator)

  #==============================================================================
  # TAU TRUTH MATCHING
  #==============================================================================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkTau/trunk/src/TauTruthMatchingWrapper.cxx
  # PhysicsAnalysis/TauID/TauAnalysisTools/trunk/Root/TauTruthMatchingTool.cxx
  from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauTruthMatchingWrapper
  from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauTruthMatchingTool
   
  from RecExConfig.ObjKeyStore import objKeyStore
  if objKeyStore.isInInput( "xAOD::TauJetContainer", "TauJets" ):   
    TOPQTauTruthMatchingTool = TauAnalysisTools__TauTruthMatchingTool(name="TOPQTauTruthMatchingTool")
    ToolSvc += TOPQTauTruthMatchingTool
    print "TOPQTauTruthMatchingTool: ", TOPQTauTruthMatchingTool
    TOPQTauTruthMatchingWrapper = DerivationFramework__TauTruthMatchingWrapper( name                 = "TOPQTauTruthMatchingWrapper",
                                                                                TauTruthMatchingTool = TOPQTauTruthMatchingTool,
                                                                                TauContainerName     = "TauJets")
    ToolSvc += TOPQTauTruthMatchingWrapper
    augmentationTools.append(TOPQTauTruthMatchingWrapper)
    print "TOPQTauTruthMatchingWrapperTool: ", TOPQTauTruthMatchingWrapper

  #=============
  # RETURN TOOLS
  #=============   
  return augmentationTools
#end setup(ToolSvc)

#==============================================================================
# SETUP TRUTH KERNEL
#==============================================================================
augmentationTools = setup(ToolSvc)
TOPQCommonTruthKernel = CfgMgr.DerivationFramework__CommonAugmentation("TOPQCommonTruthKernel", AugmentationTools = augmentationTools)
