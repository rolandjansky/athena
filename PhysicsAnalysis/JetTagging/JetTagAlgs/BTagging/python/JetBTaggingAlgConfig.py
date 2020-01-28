# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTagToolConfig import BTagToolCfg
from BTagging.BTagLightSecVertexingConfig import BTagLightSecVtxToolCfg

# import the JetBTaggingAlg configurable
from BTagging.BTaggingConf import Analysis__JetBTaggingAlg as JetBTaggingAlg

def JetBTaggingAlgCfg(ConfigFlags, JetCollection="", TaggerList=[], SetupScheme="", SVandAssoc={""}, **options):

    acc = ComponentAccumulator()
    jetcol = JetCollection

    BTagTrackToJetAssocNameList = []
    for k, v in SVandAssoc.items():
        if v not in BTagTrackToJetAssocNameList: BTagTrackToJetAssocNameList.append(v)

    TrackToJetAssociatorNameList = []
    for assoc in BTagTrackToJetAssocNameList:
        TrackToJetAssociatorNameList.append(jetcol.replace('Track', 'PV0Track') + 'Jets.' + assoc)

    options.setdefault('BTagTool', acc.popToolsAndMerge(BTagToolCfg(ConfigFlags, TaggerList)))

    timestamp = options.get('TimeStamp', None)
    if not timestamp:
        timestamp = ['']
    else:
        del options['TimeStamp']

    for ts in timestamp:
        # setup the secondary vertexing tool
        options['BTagSecVertexing'] = acc.popToolsAndMerge(BTagLightSecVtxToolCfg(ConfigFlags, 'LightSecVx'+ConfigFlags.BTagging.GeneralToolSuffix, jetcol, SVandAssoc =SVandAssoc, TimeStamp = ts, **options))

        btagname = ConfigFlags.BTagging.OutputFiles.Prefix + jetcol
        if ts:
            btagname += '_'+ts
        # Set remaining options
        options['name'] = (btagname + ConfigFlags.BTagging.GeneralToolSuffix).lower()
        options['JetCollectionName'] = jetcol.replace('Track', 'PV0Track') + 'Jets'
        options['TrackToJetAssociatorNames'] = TrackToJetAssociatorNameList
        options['BTagSVCollectionName'] = btagname + 'SecVtx'
        options['BTagJFVtxCollectionName'] = btagname + 'JFVtx'
        options['JetCalibrationName'] = jetcol.replace('Track', 'PV0Track')
        options['BTaggingCollectionName'] = btagname
        options['BTaggingLink'] = '.btaggingLink'+ts

        # -- create main BTagging algorithm
        acc.addEventAlgo(JetBTaggingAlg(**options))

    return acc
