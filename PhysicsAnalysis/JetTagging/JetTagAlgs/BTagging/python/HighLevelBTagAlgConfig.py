# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

Analysis__HighLevelBTagAlg=CompFactory.Analysis.HighLevelBTagAlg
FlavorTagDiscriminants__DL2Tool=CompFactory.FlavorTagDiscriminants.DL2Tool

def DL2ToolCfg(ConfigFlags, NNFile = '', **options):
    acc = ComponentAccumulator()

    options['nnFile'] = NNFile
    options['name'] = "decorator"
    dl2 = FlavorTagDiscriminants__DL2Tool(**options)

    acc.setPrivateTools(dl2)

    return acc

def HighLevelBTagAlgCfg(ConfigFlags, BTaggingCollection, TrackCollection, NNFile = "", sequenceName=None, **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           JetCollection       The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           options:            Python dictionary of options to be passed to the SecVtxTool.
    output: The tool."""

    if sequenceName:
        acc = ComponentAccumulator(sequenceName)
    else:
        acc = ComponentAccumulator()

    Name = NNFile.replace("/", "_").replace("_network.json", "")
    dl2 = acc.popToolsAndMerge(DL2ToolCfg(ConfigFlags, NNFile, **options))

    options = {}
    options['BTaggingCollectionName'] = BTaggingCollection
    options['TrackContainer'] = TrackCollection
    options['JetDecorator'] = dl2
    options['name'] = Name.lower()

    # -- create the association algorithm
    acc.addEventAlgo(Analysis__HighLevelBTagAlg(**options))

    return acc
