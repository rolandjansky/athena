# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# TauTruthCommon.py
# Schedules all tools needed for tau truth object selection and writes
# results into SG. These may then be accessed along the train.
#********************************************************************

from __future__ import print_function

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
# Tau Truth making and matching
def scheduleTauTruthTools(kernel=None):
    # Ensure that we are running on MC!
    from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo
    if not DerivationFrameworkIsMonteCarlo:
        return
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'TauTruthCommonKernel'):
        # Already there!  Carry on...
        return

    DFCommonTauTruthWrapperTools = []

    from AthenaCommon.AppMgr import ToolSvc
    # Matching
    # Only do if working with AOD
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput( "xAOD::TauJetContainer", "TauJets" ):    
        from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauTruthMatchingTool
        DFCommonTauTruthMatchingTool = TauAnalysisTools__TauTruthMatchingTool(name="DFCommonTauTruthMatchingTool")
        DFCommonTauTruthMatchingTool.WriteTruthTaus = True
        DFCommonTauTruthMatchingTool.WriteInvisibleFourMomentum = True
        DFCommonTauTruthMatchingTool.WriteVisibleNeutralFourMomentum = True
        ToolSvc += DFCommonTauTruthMatchingTool

        from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauTruthMatchingWrapper
        DFCommonTauTruthMatchingWrapper = DerivationFramework__TauTruthMatchingWrapper( name = "DFCommonTauTruthMatchingWrapper",
                                                                                        TauTruthMatchingTool = DFCommonTauTruthMatchingTool,
                                                                                        TauContainerName     = "TauJets")
        ToolSvc += DFCommonTauTruthMatchingWrapper
        print (DFCommonTauTruthMatchingWrapper)
        DFCommonTauTruthWrapperTools.append(DFCommonTauTruthMatchingWrapper)
    else:
        # No reco taus, so just build the truth tau container
        from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__BuildTruthTaus
        btt = TauAnalysisTools__BuildTruthTaus( WriteTruthTaus = True,
                                                WriteInvisibleFourMomentum = True,
                                                WriteVisibleNeutralFourMomentum = True )
        ToolSvc += btt
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMakerTau
        DFCommonTruthTauCollectionMaker = DerivationFramework__TruthCollectionMakerTau()
        DFCommonTruthTauCollectionMaker.BuildTruthTaus = btt
        ToolSvc += DFCommonTruthTauCollectionMaker
        print (DFCommonTruthTauCollectionMaker)
        DFCommonTauTruthWrapperTools.append(DFCommonTruthTauCollectionMaker)

    #=======================================
    # CREATE THE DERIVATION KERNEL ALGORITHM
    #=======================================
    from AthenaCommon import CfgMgr
    kernel += CfgMgr.DerivationFramework__CommonAugmentation( "TauTruthCommonKernel",
                                                              AugmentationTools = DFCommonTauTruthWrapperTools )
