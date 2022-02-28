# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.BTagToolConfig import BTagToolCfg
from BTagging.BTagLightSecVertexingConfig import BTagLightSecVtxToolCfg


def JetBTaggingAlgCfg(ConfigFlags, BTaggingCollection, JetCollection, PrimaryVertexCollectionName, TaggerList, SecVertexers, Tracks, Muons, SetupScheme="", TimeStamp = "", **options):
    acc = ComponentAccumulator()

    options.setdefault('BTagTool', acc.popToolsAndMerge(BTagToolCfg(ConfigFlags, TaggerList, PrimaryVertexCollectionName, SetupScheme)))

    # setup the secondary vertexing tool
    options['BTagSecVertexing'] = acc.popToolsAndMerge(BTagLightSecVtxToolCfg(ConfigFlags, 'LightSecVx'+ConfigFlags.BTagging.GeneralToolSuffix, JetCollection, PrimaryVertexCollectionName, SecVertexers = SecVertexers, TimeStamp = TimeStamp, **options))

    # Set remaining options
    options['JetCollectionName'] = JetCollection.replace('Track', 'PV0Track') + 'Jets'
    options['IncomingTracks'] = Tracks
    options['OutgoingTracks'] = "BTagTrackToJetAssociator"
    options['IncomingMuons'] = Muons
    options['OutgoingMuons'] = "Muons"
    options['JetCalibrationName'] = JetCollection.replace('Track', 'PV0Track')
    options['BTagSVCollectionName'] = BTaggingCollection + 'SecVtx'
    options['BTagJFVtxCollectionName'] = BTaggingCollection + 'JFVtx'
    
    if ConfigFlags.BTagging.RunFlipTaggers is True:
        options['BTagSVFlipCollectionName'] = BTaggingCollection + 'SecVtxFlip'
        options['BTagJFVtxFlipCollectionName'] = BTaggingCollection + 'JFVtxFlip'

    options['BTaggingLinkName'] = options['JetCollectionName'] + '.btaggingLink'+TimeStamp
    if TimeStamp: BTaggingCollection += '_'+TimeStamp
    options['BTaggingCollectionName'] = BTaggingCollection
    options['JetLinkName'] = options['BTaggingCollectionName'] + '.jetLink'
    options['name'] = (options['BTaggingCollectionName'] + "_" + options['JetCollectionName'] + ConfigFlags.BTagging.GeneralToolSuffix).lower()

    # -- create main BTagging algorithm
    acc.addEventAlgo(CompFactory.Analysis.JetBTaggingAlg(**options))

    return acc
