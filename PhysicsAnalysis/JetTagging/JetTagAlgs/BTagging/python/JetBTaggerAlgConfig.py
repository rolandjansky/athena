# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags

# import the JetBTaggerAlg configurable
from BTagging.BTaggingConf import Analysis__JetBTaggerAlg as JetBTaggerAlg

def JetBTaggerAlgCfg(ConfigFlags, JetCollection="", TaggerList=[], SetupScheme="", Verbose = True, options={}, StripJetsSuffix = True):

    acc=ComponentAccumulator()
    jetcol = JetCollection

    from BTagging.BTaggingConfiguration import getConfiguration
    ConfInstance = getConfiguration()
    from BTagging.BTagToolConfig import BTagToolCfg
    #accBTagTool = BTagToolCfg(ConfigFlags, jetcol, TaggerList,  Verbose = Verbose, options=options)
    #options.setdefault('BTagTool', accBTagTool.popPrivateTools())
    #acc.merge(accBTagTool)
    options.setdefault('BTagTool', acc.popToolsAndMerge(BTagToolCfg(ConfigFlags, jetcol, TaggerList,  Verbose = Verbose, options=options)))

    objs = {}
    options = dict(options)
    options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)
    
    # setup the Analysis__BTagTrackAssociation tool
    from BTagging.BTagTrackAssociationConfig import BTagTrackAssociationCfg
    accBTagTrackAssociation = BTagTrackAssociationCfg(ConfigFlags, 'TrackAssociation', jetcol, TaggerList, Verbose = Verbose)
    thisBTagTrackAssociation = accBTagTrackAssociation.popPrivateTools()
    options.setdefault('BTagTrackAssocTool', thisBTagTrackAssociation)
    acc.merge(accBTagTrackAssociation)
    
    # setup the secondary vertexing tool
    from BTagging.BTagSecVertexingConfig import BTagSecVtxToolCfg
    accSecVtxTool = BTagSecVtxToolCfg(ConfigFlags, 'SecVx'+ConfInstance.GeneralToolSuffix(), jetcol, Verbose, outputObjs = objs)
    thisSecVtxTool = accSecVtxTool.popPrivateTools()
    options.setdefault('BTagSecVertexing', thisSecVtxTool)
    acc.merge(accSecVtxTool)

    # Set remaining options
    btagname = ConfInstance.getOutputFilesPrefix() + jetcol
    options.setdefault('name', 'btagging_antikt4emtopo')
    options.setdefault('JetCollectionName', jetcol.replace('Track','PV0Track') + "Jets")
    options.setdefault('JetCalibrationName', jetcol.replace('Track','PV0Track'))
    options.setdefault('BTaggingCollectionName', btagname)

    # -- create main BTagging algorithm
    acc.addEventAlgo(JetBTaggerAlg(**options))
    
    return acc
