# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
# TEST2.py - derivation framework example demonstrating skimming via means of string 

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TEST2SkimmingToolCfg(flags):
    """Configure the example skimming tool"""
    from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg
    acc = ComponentAccumulator()
    tdt = acc.getPrimaryAndMerge(TrigDecisionToolCfg(flags))
    acc.addPublicTool(CompFactory.DerivationFramework.xAODStringSkimmingTool(name       = "TEST2StringSkimmingTool",
                                                                             expression = "count(Muons.pt > (1 * GeV)) >= 1",
                                                                             TrigDecisionTool=tdt), 
                      primary = True)
    return(acc)                          

def TEST2KernelCfg(flags, name='TEST2Kernel', **kwargs):
    """Configure the derivation framework driving algorithm (kernel)"""
    acc = ComponentAccumulator()
    skimmingTool = acc.getPrimaryAndMerge(TEST2SkimmingToolCfg(flags))
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    acc.addEventAlgo(DerivationKernel(name, SkimmingTools = [skimmingTool]))       
    return acc


def TEST2Cfg(ConfigFlags):

    acc = ComponentAccumulator()
    acc.merge(TEST2KernelCfg(ConfigFlags, name="TEST2Kernel"))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    TEST2SlimmingHelper = SlimmingHelper("TEST2SlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)
    TEST2SlimmingHelper.SmartCollections = ["EventInfo",
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
    TEST2ItemList = TEST2SlimmingHelper.GetItemList()

    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_TEST2", ItemList=TEST2ItemList, AcceptAlgs=["TEST2Kernel"]))

    return acc
