# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# TauTruthCommon.py
# Schedules all tools needed for tau truth object selection and writes
# results into SG. These may then be accessed along the train.
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
# Tau Truth making and matching
if DerivationFrameworkIsMonteCarlo:

    from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauTruthMatchingWrapper
    from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauTruthMatchingTool

    DFCommonTauTruthWrapperTools = []

    # Tau Truth making and matching
    # Set up the MCTruthClassifier
    from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
    DFCommonTauTruthClassifier = MCTruthClassifier(name = "DFCommonTauTruthClassifier",
                                        ParticleCaloExtensionTool="")
    ToolSvc += DFCommonTauTruthClassifier

    # Build the truth taus
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMakerTau
    DFCommonTruthTauTool = DerivationFramework__TruthCollectionMakerTau(name             = "DFCommonTruthTauTool",
                                                                 NewCollectionName       = "TruthTaus",
                                                                 MCTruthClassifier       = DFCommonTauTruthClassifier)
    ToolSvc += DFCommonTruthTauTool
    DFCommonTauTruthWrapperTools.append(DFCommonTruthTauTool)

    # Matching
    # Only do if working with AOD
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput( "xAOD::TauJetContainer", "TauJets" ):    
        DFCommonTauTruthMatchingTool = TauAnalysisTools__TauTruthMatchingTool(name="DFCommonTauTruthMatchingTool")
        ToolSvc += DFCommonTauTruthMatchingTool
        DFCommonTauTruthMatchingWrapper = DerivationFramework__TauTruthMatchingWrapper( name = "DFCommonTauTruthMatchingWrapper",
                                                                                        TauTruthMatchingTool = DFCommonTauTruthMatchingTool,
                                                                                        TauContainerName     = "TauJets")
        ToolSvc += DFCommonTauTruthMatchingWrapper
        print DFCommonTauTruthMatchingWrapper
        DFCommonTauTruthWrapperTools.append(DFCommonTauTruthMatchingWrapper)

    #=======================================
    # CREATE THE DERIVATION KERNEL ALGORITHM
    #=======================================

    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
    DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("TauTruthCommonKernel",
                                                                             AugmentationTools = DFCommonTauTruthWrapperTools)
