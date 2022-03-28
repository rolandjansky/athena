# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
# TEST1.py - derivation framework example demonstrating skimming 

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TEST1SkimmingToolCfg(flags):
    """Configure the example skimming tool"""
    acc = ComponentAccumulator()
    acc.addPublicTool(CompFactory.DerivationFramework.SkimmingToolExample(name                    = "TEST1SkimmingTool", 
                                                                          MuonContainerKey        = "Muons",
                                                                          NumberOfMuons           = 1,
                                                                          MuonPtCut               = 1000.0),
                      primary = True)
    return(acc)                          

def TEST1KernelCfg(flags, name='TEST1Kernel', **kwargs):
    """Configure the derivation framework driving algorithm (kernel)"""
    acc = ComponentAccumulator()
    skimmingTool = acc.getPrimaryAndMerge(TEST1SkimmingToolCfg(flags))
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    acc.addEventAlgo(DerivationKernel(name, SkimmingTools = [skimmingTool]))       
    return acc


def TEST1Cfg(ConfigFlags):

    acc = ComponentAccumulator()
    acc.merge(TEST1KernelCfg(ConfigFlags, name="TEST1Kernel"))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    TEST1SlimmingHelper = SlimmingHelper("TEST1SlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)
    TEST1SlimmingHelper.SmartCollections = ["EventInfo",
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
    TEST1ItemList = TEST1SlimmingHelper.GetItemList()

    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_TEST1", ItemList=TEST1ItemList, AcceptAlgs=["TEST1Kernel"]))

    return acc
