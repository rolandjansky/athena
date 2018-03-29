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
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/BoostedHadTopAndTopPairFilterTool.cxx
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/BoostedHadTopAndTopPairAugmentation.cxx
  from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__BoostedHadTopAndTopPairFilterTool
  TOPQboostedtopfiltertool = DerivationFramework__BoostedHadTopAndTopPairFilterTool(
                               name = "TOPQBoostedHadTopAndTopPairFilterTool")
                               #tHadPtCut  = 500000.0, #cut on hadronic tops in MeV
                               #tPairPtCut = 350000.0) #cut on ttbar system in MeV
  ToolSvc += TOPQboostedtopfiltertool

  from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__BoostedHadTopAndTopPairFilterAugmentation
  TOPQBoostedHadTopAndTopPairFilterAugmentation = DerivationFramework__BoostedHadTopAndTopPairFilterAugmentation(name = "TOPQBoostedHadTopAndTopPairFilterAugmentation")
  TOPQBoostedHadTopAndTopPairFilterAugmentation.FilterTool_Low  = TOPQboostedtopfiltertool
  TOPQBoostedHadTopAndTopPairFilterAugmentation.FilterTool_High = TOPQboostedtopfiltertool
  ToolSvc += TOPQBoostedHadTopAndTopPairFilterAugmentation
  augmentationTools.append(TOPQBoostedHadTopAndTopPairFilterAugmentation)
  print "TOPQBoostedHadTopAndTopPairFilterAugmentationTool: ", TOPQBoostedHadTopAndTopPairFilterAugmentation


  #==============================================================================
  # HEAVY FLAVOR DECORATION
  #==============================================================================
  # PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TTbarPlusHeavyFlavorFilterTool.cxx
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

  from DerivationFrameworkMCTruth.HFHadronsCommon import *
  TOPQDSIDList=list(DSIDList)


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
  # Does the tau truth matching, and also gives us a tau truth collection
  from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
  scheduleTauTruthTools()

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

#==============================================================================
# Add SumOfWeights metadata for LHE3 multiweights: TOPQDERIV-21
#==============================================================================
import DerivationFrameworkCore.LHE3WeightMetadata

#     #==============================================================================
#     # HIGGS TEMPLATE XSECTION augmentation
#     #==============================================================================
#     # https://its.cern.ch/jira/browse/TOPQDERIV-6
#     # first find config file
#     from PathResolver import PathResolver as pr
#     infile = pr.FindCalibFile('DerivationFrameworkHiggs/HiggsMCsamples.cfg')
#     # can't yet use the PathResolver right now - doing it with environment variables
#     #Prefix = os.getenv("AtlasArea")
#     #infile = Prefix+'/PhysicsAnalysis/DerivationFramework/DerivationFrameworkHiggs/share/HiggsMCsamples.cfg'
#     print 'Found path to HiggsMCsamples.cfg configuration file (using PathResolver): '+infile
#     ## use the config file with ROOT TEnv class
#     import ROOT
#     higgs_env = ROOT.TEnv()
#     higgs_env.ReadFile(infile, ROOT.kEnvLocal)
#     ## Convert the TEnv into a python dict
#     tenv_dict = {i.GetName(): set(i.GetValue().split()) for i in higgs_env.GetTable()}
#     ## Now combine the DSIDs from each category into a single set
#     final_set = {i for dsid_list in tenv_dict.values() for i in dsid_list}
#     ## finaly import the Higgs stuff only if the DSID is in the list
#     import PyUtils.AthFile as af
#     from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#     f = af.fopen(athenaCommonFlags.PoolAODInput()[0])
#     if len(f.mc_channel_number) > 0:
#       if str(f.mc_channel_number[0]) in final_set:
#         import DerivationFrameworkHiggs.TruthCategories
