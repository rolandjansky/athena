# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags

# import the JetBTaggerAlg configurable
from BTagging.BTaggingConf import Analysis__JetBTaggerAlg as JetBTaggerAlg

def JetBTaggerAlgCfg(ConfigFlags, JetCollection="", TaggerList=[], SetupScheme="", **options):

    acc=ComponentAccumulator()
    jetcol = JetCollection

    from BTagging.BTagToolConfig import BTagToolCfg
    options.setdefault('BTagTool', acc.popToolsAndMerge(BTagToolCfg(ConfigFlags, jetcol, TaggerList, options)))

    objs = {}
    #options = dict(options)
    options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)
    
    # setup the Analysis__BTagTrackAssociation tool
    from BTagging.BTagTrackAssociationConfig import BTagTrackAssociationCfg
    options.setdefault('BTagTrackAssocTool', acc.popToolsAndMerge(BTagTrackAssociationCfg(ConfigFlags, 'TrackAssociation'+ ConfigFlags.BTagging.GeneralToolSuffix, jetcol, TaggerList )))
    
    # setup the secondary vertexing tool
    from BTagging.BTagSecVertexingConfig import BTagSecVtxToolCfg
    options.setdefault('BTagSecVertexing', acc.popToolsAndMerge(BTagSecVtxToolCfg(ConfigFlags, 'SecVx'+ConfigFlags.BTagging.GeneralToolSuffix, jetcol, outputObjs = objs)))

    # Set remaining options
    new_prefix = options.get('new_prefix', None)
    if new_prefix:
        btagname = new_prefix + jetcol
        del options['new_prefix']
    else:
        btagname = ConfigFlags.BTagging.OutputFiles.Prefix + jetcol
    options.setdefault('name', (btagname + ConfigFlags.BTagging.GeneralToolSuffix).lower())
    options.setdefault('JetCollectionName', jetcol.replace('Track','PV0Track') + "Jets")
    options.setdefault('JetCalibrationName', jetcol.replace('Track','PV0Track'))
    options.setdefault('BTaggingCollectionName', btagname)

    # -- create main BTagging algorithm
    acc.addEventAlgo(JetBTaggerAlg(**options))
    
    return acc
