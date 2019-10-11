# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags

# import the JetBTaggerAlg configurable
from BTagging.BTaggingConf import Analysis__JetBTaggerAlg as JetBTaggerAlg

def JetBTaggerAlgCfg(ConfigFlags, JetCollection="", TaggerList=[], SetupScheme="", **options):

    acc=ComponentAccumulator()
    jetcol = JetCollection

    objs = {}
    #options = dict(options)
    options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)
    
    # setup the Analysis__BTagTrackAssociation tool
    from BTagging.BTagTrackAssociationConfig import BTagTrackAssociationCfg
    options.setdefault('BTagTrackAssocTool', acc.popToolsAndMerge(BTagTrackAssociationCfg(ConfigFlags, 'TrackAssociation'+ ConfigFlags.BTagging.GeneralToolSuffix, jetcol, TaggerList )))
    
    # setup the secondary vertexing tool
    from BTagging.BTagSecVertexingConfig import BTagSecVtxToolCfg
    options.setdefault('BTagSecVertexing', acc.popToolsAndMerge(BTagSecVtxToolCfg(ConfigFlags, 'SecVx'+ConfigFlags.BTagging.GeneralToolSuffix, jetcol, outputObjs = objs, **options)))

    from BTagging.BTagToolConfig import BTagToolCfg
    options.setdefault('BTagTool', acc.popToolsAndMerge(BTagToolCfg(ConfigFlags, jetcol, TaggerList)))

    btagname = ConfigFlags.BTagging.OutputFiles.Prefix + jetcol
    timestamp = options.get('TimeStamp', None)
    if timestamp:
        btagname += timestamp
        del options['TimeStamp']

    # Set remaining options
    options.setdefault('name', (btagname + ConfigFlags.BTagging.GeneralToolSuffix).lower())
    options.setdefault('JetCollectionName', jetcol.replace('Track','PV0Track') + "Jets")
    options.setdefault('JetCalibrationName', jetcol.replace('Track','PV0Track'))
    options.setdefault('BTaggingCollectionName', btagname)

    # -- create main BTagging algorithm
    acc.addEventAlgo(JetBTaggerAlg(**options))
    
    return acc
