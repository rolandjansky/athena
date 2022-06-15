# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# TauTruthCommonConfig.py
# Schedules all tools needed for tau truth object selection and writes
# results into SG. These may then be accessed along the train.
# Uses component accumulator
#********************************************************************

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# TODO: should this be in a separate file or ok here?
def TauTruthMatchingWrapperCfg(flags, name = "DFCommonTauTruthMatchingWrapper", **kwargs):
    """Configure the tau truth matching wrapper"""
    acc = ComponentAccumulator()
    TauTruthMatchingWrapper = CompFactory.DerivationFramework.TauTruthMatchingWrapper
    acc.addPublicTool(TauTruthMatchingWrapper(name = name, **kwargs), primary = True)
    return(acc)

# TODO: should this be in a separate file or ok here?
def TruthCollectionMakerTauCfg(flags, name, **kwargs):
    """Configure the tau truth collection maker"""
    acc = ComponentAccumulator()
    TruthCollectionMakerTau = CompFactory.DerivationFramework.TruthCollectionMakerTau
    acc.addPublicTool(TruthCollectionMakerTau(name, **kwargs), primary = True)
    return(acc)

# TODO: should be a common cfg in TauAnalysisTools
def TauTruthMatchingToolCfg(flags, name="DFCommonTauTruthMatchingTool", **kwargs):
    """Configure the tau truth matching tool"""
    acc = ComponentAccumulator()
    TauTruthMatchingTool = CompFactory.TauAnalysisTools.TauTruthMatchingTool
    acc.addPublicTool(TauTruthMatchingTool(name = name, **kwargs), primary = True)
    return(acc)

# TODO: should be a common cfg in TauAnalysisTools
def BuildTruthTausCfg(flags, name, **kwargs):
    """Configure the BuildTruthTaus tool"""
    acc = ComponentAccumulator()
    BuildTruthTaus = CompFactory.TauAnalysisTools.BuildTruthTaus
    acc.addPublicTool(BuildTruthTaus(name = name, **kwargs), primary = True)
    return(acc)

def TauTruthToolsCfg(ConfigFlags):
    """Configure tau truth making and matching"""

    acc = ComponentAccumulator()

    # Ensure that we are running on MC
    if not ConfigFlags.Input.isMC:
        return

    DFCommonTauTruthWrapperTools = []

    # Matching
    # Only do if working with AOD
    if "xAOD::TauJetContainer#TauJets" in ConfigFlags.Input.TypedCollections:    
        DFCommonTauTruthMatchingTool = acc.getPrimaryAndMerge(TauTruthMatchingToolCfg(
            ConfigFlags, 
            name                            = "DFCommonTauTruthMatchingTool",
            WriteTruthTaus                  = True,
            WriteInvisibleFourMomentum      = True,
            WriteVisibleNeutralFourMomentum = True,
            TruthJetContainerName           = "AntiKt4TruthDressedWZJets"))
        DFCommonTauTruthWrapperTool = acc.getPrimaryAndMerge(TauTruthMatchingWrapperCfg(
            ConfigFlags, 
            name                 = "DFCommonTauTruthMatchingWrapper",
            TauTruthMatchingTool = DFCommonTauTruthMatchingTool,
            TauContainerName     = "TauJets")) 
        DFCommonTauTruthWrapperTools.append(DFCommonTauTruthWrapperTool)
    else:
        # No reco taus, so just build the truth tau container
        acc.merge(BuildTruthTausCfg(ConfigFlags, 
                                    name                            = "DFCommonTauTruthBuilder",
                                    WriteTruthTaus                  = True,
                                    WriteInvisibleFourMomentum      = True,
                                    WriteVisibleNeutralFourMomentum = True ))

        acc.merge(TruthCollectionMakerTauCfg(ConfigFlags,
                                             name           = "DFCommonTauTruthCollectionMaker",
                                             BuildTruthTaus = acc.getPublicTool("DFCommonTauTruthBuilder")))
        DFCommonTauTruthWrapperTools.append(acc.getPublicTool("DFCommonTauTruthCollectionMaker"))

    CommonAugmentation = CompFactory.DerivationFramework.CommonAugmentation    
    acc.addEventAlgo(CommonAugmentation( "TauTruthCommonKernel", AugmentationTools = DFCommonTauTruthWrapperTools ))    
    
    return acc    
