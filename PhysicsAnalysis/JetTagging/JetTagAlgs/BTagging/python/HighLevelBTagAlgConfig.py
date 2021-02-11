# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

Analysis__HighLevelBTagAlg=CompFactory.Analysis.HighLevelBTagAlg
FlavorTagDiscriminants__DL2Tool=CompFactory.FlavorTagDiscriminants.DL2Tool

def DL2ToolCfg(ConfigFlags, NNFile = '', **options):
    acc = ComponentAccumulator()

    options['nnFile'] = NNFile
    options['name'] = "decorator"

    # This is a hack to accomodate the older b-tagging training with
    # old names for variables. We should be able to remove it when we
    # move over to the 2020 / 2021 retraining.
    if '201903' in NNFile and 'dl1' in NNFile:
        remap = {}
        for aggragate in ['minimum','maximum','average']:
            remap[f'{aggragate}TrackRelativeEta'] = (
                f'JetFitterSecondaryVertex_{aggragate}AllJetTrackRelativeEta')
        options['variableRemapping'] = remap

    dl2 = FlavorTagDiscriminants__DL2Tool(**options)

    acc.setPrivateTools(dl2)

    return acc

def HighLevelBTagAlgCfg(ConfigFlags, BTaggingCollection, TrackCollection, NNFile = "", **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           JetCollection       The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           options:            Python dictionary of options to be passed to the SecVtxTool.
    output: The tool."""

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
