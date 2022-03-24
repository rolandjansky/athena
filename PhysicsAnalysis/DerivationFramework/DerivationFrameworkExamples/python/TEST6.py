# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
# TEST6.py - derivation framework example demonstrating pre-selection, such that only
# after passing the skimming do heavier augmentation tools run 

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.CFElements import seqAND

def TEST6SkimmingToolCfg(flags):
    """Configure the example skimming tool"""
    acc = ComponentAccumulator()
    acc.addPublicTool(CompFactory.DerivationFramework.SkimmingToolExample(name                    = "TEST1SkimmingTool",
                                                                          MuonContainerKey        = "Muons",
                                                                          NumberOfMuons           = 1,
                                                                          MuonPtCut               = 1000.0),
                      primary = True)
    return(acc)                          

def TEST6AugmentationToolCfg(flags):
    """Configure the example augmentation tool"""
    acc = ComponentAccumulator()
    acc.addPublicTool(CompFactory.DerivationFramework.AugmentationToolExample(name= "TEST6AugmentationTool"),
                      primary = True)
    return(acc)

def TEST6KernelCfg(flags, name='TEST6Kernel', **kwargs):
    """Configure the derivation framework driving algorithm (kernel)"""
    acc = ComponentAccumulator()
    # Subsequence
    acc.addSequence( seqAND("TEST6Sequence") )
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    skimmingTool = acc.getPrimaryAndMerge(TEST6SkimmingToolCfg(flags))
    skimmingKernel = DerivationKernel(kwargs["PreselectionName"], SkimmingTools = [skimmingTool])
    # Add skimming tool to subsequence
    acc.addEventAlgo( skimmingKernel, sequenceName="TEST6Sequence" ) 
    augmentationTool = acc.getPrimaryAndMerge(TEST6AugmentationToolCfg(flags))
    # Add augmentation tool to same subsequence
    acc.addEventAlgo(DerivationKernel(name, AugmentationTools = [augmentationTool]), sequenceName="TEST6Sequence")
    return(acc)


def TEST6Cfg(ConfigFlags):

    acc = ComponentAccumulator()
    acc.merge(TEST6KernelCfg(ConfigFlags, name="TEST6Kernel", PreselectionName="TEST6PreselectionKernel"))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    TEST6SlimmingHelper = SlimmingHelper("TEST6SlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)
    TEST6SlimmingHelper.SmartCollections = ["EventInfo",
                                            "Electrons",
                                            "Photons",
                                            "Muons",
                                            "PrimaryVertices",
                                            "InDetTrackParticles",
                                            "AntiKt4EMTopoJets",
                                            "AntiKt4EMPFlowJets",
                                            "BTagging_AntiKt4EMPFlow",
                                            "BTagging_AntiKtVR30Rmax4Rmin02Track", 
                                            "MET_Baseline_AntiKt4EMTopo",
                                            "MET_Baseline_AntiKt4EMPFlow",
                                            "TauJets",
                                            "DiTauJets",
                                            "DiTauJetsLowPt",
                                            "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                            "AntiKtVR30Rmax4Rmin02PV0TrackJets"]
    TEST6ItemList = TEST6SlimmingHelper.GetItemList()

    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_TEST6", ItemList=TEST6ItemList, AcceptAlgs=["TEST6Kernel"]))

    return acc
