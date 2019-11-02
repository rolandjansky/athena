# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
    # these are supposed to mimic the TTbarPlusBFilter, TTbarPlusBBFilter, and TTbarPlusCFilter Filters in https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC15JobOptions/trunk/common/Filters
    from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TTbarPlusHeavyFlavorFilterTool

    TOPQttbarBfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("TOPQTTbarPlusBFilterTool")
    TOPQttbarBfiltertool.SelectB = True
    TOPQttbarBfiltertool.BpTMinCut = 5000
    TOPQttbarBfiltertool.BMultiplicityCut = 1 # >=
    ToolSvc += TOPQttbarBfiltertool

    TOPQttbarBBfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("TOPQTTbarPlusBBFilterTool")
    TOPQttbarBBfiltertool.SelectB = True
    TOPQttbarBBfiltertool.BpTMinCut = 15000
    TOPQttbarBBfiltertool.BMultiplicityCut = 2 # >=
    ToolSvc += TOPQttbarBBfiltertool

    TOPQttbarCfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("TOPQTTbarPlusCFilterTool")
    TOPQttbarCfiltertool.SelectC = True
    TOPQttbarCfiltertool.CpTMinCut = 15000
    TOPQttbarCfiltertool.CMultiplicityCut = 1 # >=
    # these two are the default values
    # B-hadrons have precedence; if one B is found, it won't pass the CFilter
    TOPQttbarCfiltertool.BpTMinCut = 5000
    TOPQttbarCfiltertool.BMultiplicityCut = 1 # >=
    ToolSvc += TOPQttbarCfiltertool

    from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TopHeavyFlavorFilterAugmentation
    TOPQTopHFFilterAugmentation = DerivationFramework__TopHeavyFlavorFilterAugmentation(name = "TOPQTopHFFilterAugmentation")
    TOPQTopHFFilterAugmentation.BFilterTool = TOPQttbarBfiltertool
    TOPQTopHFFilterAugmentation.BBFilterTool = TOPQttbarBBfiltertool
    TOPQTopHFFilterAugmentation.CFilterTool = TOPQttbarCfiltertool
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
