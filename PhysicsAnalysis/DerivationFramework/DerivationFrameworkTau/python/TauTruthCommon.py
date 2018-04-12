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
def scheduleTauTruthTools(kernel=None):
    # Ensure that we are running on MC!
    if not DerivationFrameworkIsMonteCarlo:
        return
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'TauTruthCommonKernel'):
        # Already there!  Carry on...
        return

    from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauTruthMatchingWrapper
    from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauTruthMatchingTool

    DFCommonTauTruthWrapperTools = []

    # Tau Truth making and matching
    # Set up the MCTruthClassifier if we need to
    # Would normally use the MCTC from DFCommon, but this goes to a private tool handle
    from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
    DFCommonTauTruthClassifier = MCTruthClassifier(name = "DFCommonTauTruthClassifier",
                                        ParticleCaloExtensionTool="")

    from AthenaCommon.AppMgr import ToolSvc
    # Matching
    # Only do if working with AOD
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput( "xAOD::TauJetContainer", "TauJets" ):    
        # Make sure the MCTC is in the toolSvc in this case
        ToolSvc += DFCommonTauTruthClassifier

        DFCommonTauTruthMatchingTool = TauAnalysisTools__TauTruthMatchingTool(name="DFCommonTauTruthMatchingTool")
        DFCommonTauTruthMatchingTool.WriteTruthTaus = True
        DFCommonTauTruthMatchingTool.MCTruthClassifierTool = DFCommonTauTruthClassifier
        ToolSvc += DFCommonTauTruthMatchingTool
        DFCommonTauTruthMatchingWrapper = DerivationFramework__TauTruthMatchingWrapper( name = "DFCommonTauTruthMatchingWrapper",
                                                                                        TauTruthMatchingTool = DFCommonTauTruthMatchingTool,
                                                                                        TauContainerName     = "TauJets")
        ToolSvc += DFCommonTauTruthMatchingWrapper
        print DFCommonTauTruthMatchingWrapper
        DFCommonTauTruthWrapperTools.append(DFCommonTauTruthMatchingWrapper)
    else:
        # No reco taus, so just build the truth tau container
        from tauRecTools.tauRecToolsConf import tauRecTools__BuildTruthTaus
        btt = tauRecTools__BuildTruthTaus( WriteTruthTaus=True ,
                                           WriteInvisibleFourMomentum=True,
                                           MCTruthClassifierTool=DFCommonTauTruthClassifier )
        # This guy wants a private tool handle, so *don't* put the MCTC into the toolSvc
        ToolSvc += btt
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMakerTau
        DFCommonTruthTauCollectionMaker = DerivationFramework__TruthCollectionMakerTau()
        DFCommonTruthTauCollectionMaker.BuildTruthTaus = btt
        ToolSvc += DFCommonTruthTauCollectionMaker
        DFCommonTauTruthWrapperTools.append(DFCommonTruthTauCollectionMaker)

    #=======================================
    # CREATE THE DERIVATION KERNEL ALGORITHM
    #=======================================
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
    kernel += CfgMgr.DerivationFramework__CommonAugmentation("TauTruthCommonKernel",
                                                                             AugmentationTools = DFCommonTauTruthWrapperTools)
