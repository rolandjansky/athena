# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python
# TEST5.py - derivation framework example demonstrating the use of decorators
# Two decorations are done - one from an example tool (AugemntationToolExample)
# and one from a CP tool (muon selection tool). Decorations are added as ExtraVariables

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TEST5CPToolCfg(flags):
    """Configure the example muon CP tool"""
    acc = ComponentAccumulator()
    mst = CompFactory.CP.MuonSelectionTool('TEST5MuonSelectionTool')
    acc.addPublicTool(mst, primary=True)
    acc.addPublicTool(CompFactory.DerivationFramework.AsgSelectionToolWrapper(name = "TEST5MuonToolWrapper",
                                                                              AsgSelectionTool = mst,
                                                                              CutType = "IDHits",
                                                                              StoreGateEntryName = "TEST5GoodMuons",
                                                                              ContainerName = "Muons"),
                      primary = True)
    return(acc)


def TEST5AugmentationToolCfg(flags):
    """Configure the example augmentation tool"""
    acc = ComponentAccumulator()
    acc.addPublicTool(CompFactory.DerivationFramework.AugmentationToolExample(name       = "TEST5AugmentationTool"),
                      primary = True)
    return(acc)                          


def TEST5KernelCfg(flags, name='TEST5Kernel', **kwargs):
    """Configure the derivation framework driving algorithm (kernel)"""
    acc = ComponentAccumulator()
    augmentationTool = acc.getPrimaryAndMerge(TEST5AugmentationToolCfg(flags))
    cpTool = acc.getPrimaryAndMerge(TEST5CPToolCfg(flags))
    DerivationKernel = CompFactory.DerivationFramework.DerivationKernel
    acc.addEventAlgo(DerivationKernel(name, AugmentationTools = [augmentationTool,cpTool]))       
    return acc


def TEST5Cfg(ConfigFlags):

    acc = ComponentAccumulator()
    acc.merge(TEST5KernelCfg(ConfigFlags, name="TEST5Kernel",StreamName = "OutputStreamDAOD_TEST5"))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
    TEST5SlimmingHelper = SlimmingHelper("TEST5SlimmingHelper", NamesAndTypes = ConfigFlags.Input.TypedCollections)
    TEST5SlimmingHelper.SmartCollections = ["EventInfo","InDetTrackParticles","PrimaryVertices","Muons"]
    TEST5SlimmingHelper.ExtraVariables += ["InDetTrackParticles.DFDecoratorExample"]
    TEST5SlimmingHelper.ExtraVariables += ["Muons.TEST5GoodMuons"]
    TEST5ItemList = TEST5SlimmingHelper.GetItemList()
    acc.merge(OutputStreamCfg(ConfigFlags, "DAOD_TEST5", ItemList=TEST5ItemList, AcceptAlgs=["TEST5Kernel"]))
    return acc
