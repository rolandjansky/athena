# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
# TEST4.py - derivation framework example demonstrating slimming 
# Note this is the same as TEST1,2,3 but without any tools

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TEST4KernelCfg(flags, name='TEST4Kernel', **kwargs):
    """Configure the derivation framework driving algorithm (kernel)"""
    acc = ComponentAccumulator()
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    acc.addEventAlgo(DerivationKernel(name))       
    return acc


def TEST4Cfg(ConfigFlags):

    acc = ComponentAccumulator()
    acc.merge(TEST4KernelCfg(ConfigFlags, name="TEST4Kernel"))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    TEST4SlimmingHelper = SlimmingHelper("TEST4SlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)
    TEST4SlimmingHelper.SmartCollections = ["EventInfo",
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
    TEST4ItemList = TEST4SlimmingHelper.GetItemList()

    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_TEST4", ItemList=TEST4ItemList, AcceptAlgs=["TEST4Kernel"]))

    return acc
