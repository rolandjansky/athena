# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.BTagToolConfig import BTagToolCfg
from BTagging.BTagLightSecVertexingConfig import BTagLightSecVtxToolCfg

# import the JetBTaggingAlg configurable
Analysis__JetBTaggingAlg = CompFactory.Analysis.JetBTaggingAlg


def JetBTaggingAlgCfg(ConfigFlags, BTaggingCollection="", JetCollection="", PrimaryVertexCollectionName="", TaggerList=[], SetupScheme="", SVandAssoc={""}, TimeStamp = "", **options):

    acc = ComponentAccumulator()
    jetcol = JetCollection
    ts = TimeStamp

    BTagTrackToJetAssocNameList = []
    for k, v in SVandAssoc.items():
        if v not in BTagTrackToJetAssocNameList: BTagTrackToJetAssocNameList.append(v)

    TrackToJetAssociatorNameList = []
    for assoc in BTagTrackToJetAssocNameList:
        TrackToJetAssociatorNameList.append(jetcol.replace('Track', 'PV0Track') + 'Jets.' + assoc)

    options.setdefault('BTagTool', acc.popToolsAndMerge(BTagToolCfg(ConfigFlags, TaggerList, PrimaryVertexCollectionName, SetupScheme)))

    # setup the secondary vertexing tool
    options['BTagSecVertexing'] = acc.popToolsAndMerge(BTagLightSecVtxToolCfg(ConfigFlags, 'LightSecVx'+ConfigFlags.BTagging.GeneralToolSuffix, jetcol, PrimaryVertexCollectionName, SVandAssoc =SVandAssoc, TimeStamp = ts, **options))

    # Set remaining options
    options['JetCollectionName'] = jetcol.replace('Track', 'PV0Track') + 'Jets'
    options['TrackToJetAssociatorNames'] = TrackToJetAssociatorNameList
    options['JetCalibrationName'] = jetcol.replace('Track', 'PV0Track')
    options['BTagSVCollectionName'] = BTaggingCollection + 'SecVtx'
    options['BTagJFVtxCollectionName'] = BTaggingCollection + 'JFVtx'

    options['BTaggingLinkName'] = options['JetCollectionName'] + '.btaggingLink'+ts
    if ts:
        BTaggingCollection += '_'+ts
    options['BTaggingCollectionName'] = BTaggingCollection
    options['JetLinkName'] = options['BTaggingCollectionName'] + '.jetLink'
    options['name'] = (options['BTaggingCollectionName'] + ConfigFlags.BTagging.GeneralToolSuffix).lower()

    # -- create main BTagging algorithm
    acc.addEventAlgo(Analysis__JetBTaggingAlg(**options))

    return acc
