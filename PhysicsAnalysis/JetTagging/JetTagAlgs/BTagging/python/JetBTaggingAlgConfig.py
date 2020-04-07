# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTagToolConfig import BTagToolCfg
from BTagging.BTagLightSecVertexingConfig import BTagLightSecVtxToolCfg

# import the JetBTaggingAlg configurable
from BTagging.BTaggingConf import Analysis__JetBTaggingAlg as JetBTaggingAlg

def JetBTaggingAlgCfg(ConfigFlags, JetCollection="", TaggerList=[], SetupScheme="", SVandAssoc={""}, TimeStamp = "", **options):

    acc = ComponentAccumulator()
    jetcol = JetCollection
    ts = TimeStamp

    BTagTrackToJetAssocNameList = []
    for k, v in SVandAssoc.items():
        if v not in BTagTrackToJetAssocNameList: BTagTrackToJetAssocNameList.append(v)

    TrackToJetAssociatorNameList = []
    for assoc in BTagTrackToJetAssocNameList:
        TrackToJetAssociatorNameList.append(jetcol.replace('Track', 'PV0Track') + 'Jets.' + assoc)

    options.setdefault('BTagTool', acc.popToolsAndMerge(BTagToolCfg(ConfigFlags, TaggerList)))

    # setup the secondary vertexing tool
    options['BTagSecVertexing'] = acc.popToolsAndMerge(BTagLightSecVtxToolCfg(ConfigFlags, 'LightSecVx'+ConfigFlags.BTagging.GeneralToolSuffix, jetcol, SVandAssoc =SVandAssoc, TimeStamp = ts, **options))

    btagname = ConfigFlags.BTagging.OutputFiles.Prefix + jetcol
    # Set remaining options
    options['JetCollectionName'] = jetcol.replace('Track', 'PV0Track') + 'Jets'
    options['TrackToJetAssociatorNames'] = TrackToJetAssociatorNameList
    options['JetCalibrationName'] = jetcol.replace('Track', 'PV0Track')
    options['BTagSVCollectionName'] = btagname + 'SecVtx'
    options['BTagJFVtxCollectionName'] = btagname + 'JFVtx'
    options['BTaggingLinkName'] = options['JetCollectionName'] + '.btaggingLink'+ts
    if ts:
        btagname += '_'+ts
    options['BTaggingCollectionName'] = btagname
    options['JetLinkName'] = options['BTaggingCollectionName'] + '.jetLink'
    options['name'] = (btagname + ConfigFlags.BTagging.GeneralToolSuffix).lower()
    print("Manu")
    print(options)

    # -- create main BTagging algorithm
    acc.addEventAlgo(JetBTaggingAlg(**options))

    return acc
