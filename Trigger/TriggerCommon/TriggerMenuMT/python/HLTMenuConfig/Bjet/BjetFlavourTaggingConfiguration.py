#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaConfiguration.AllConfigFlags import ConfigFlags

def getFlavourTagging( inputJets, inputVertex, inputTracks ):

    acc = ComponentAccumulator()

    kwargs = {}

    from AthenaConfiguration.ComponentFactory import CompFactory
    Analysis__BTagTrackAugmenterAlg=CompFactory.Analysis.BTagTrackAugmenterAlg
    bTagTrackAugmenter = Analysis__BTagTrackAugmenterAlg( "Analysis__BTagTrackAugmenterAlg" )
    bTagTrackAugmenter.TrackContainer = inputTracks
    bTagTrackAugmenter.PrimaryVertexContainer = inputVertex
    acc.addEventAlgo(bTagTrackAugmenter)

    from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
    from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
    from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
    from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
    

    TrackToJetAssociators = ['BTagTrackToJetAssociator', 'BTagTrackToJetAssociatorBB']
    kwargs['Release'] = '22'
    acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputTracks, 'BTagTrackToJetAssociator', **kwargs))
    kwargs['Release'] = '21'
    acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputTracks, 'BTagTrackToJetAssociatorBB', **kwargs))
    del kwargs['Release'] 

    btagname = "HLT_BTagging"
    btagname_JFVtx = btagname + "_JFVtx"
    btagname_SecVtx = btagname + "_SecVtx"
    btagname_jetLink = btagname + ".jetLink"

    SecVertexingAndAssociators = {'JetFitter':'BTagTrackToJetAssociator','SV1':'BTagTrackToJetAssociator'}
    for k, v in SecVertexingAndAssociators.items():
        if v not in TrackToJetAssociators:
            raise RuntimeError( v + ' is not configured')
        acc.merge(JetSecVtxFindingAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputVertex, k, v))
        JetSecVertexingAlg = JetSecVertexingAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputVertex, k, v)
        SecVertexingAlg = JetSecVertexingAlg.getEventAlgo(inputJets.replace("Jets","").lower() + "_" + k.lower() + "_secvtx")
        if k == "JetFitter":
            SecVertexingAlg.BTagJFVtxCollectionName = recordable( btagname_JFVtx )
        elif k == "SV1":
            SecVertexingAlg.BTagSVCollectionName = recordable( btagname_SecVtx )
        acc.merge(JetSecVertexingAlg)
    
    JetBTaggingAlg = JetBTaggingAlgCfg(ConfigFlags, JetCollection = inputJets.replace("Jets",""), PrimaryVertexCollectionName=inputVertex, TaggerList = ConfigFlags.BTagging.TrigTaggersList, SetupScheme = "Trig", SVandAssoc = SecVertexingAndAssociators, **kwargs)
    BTaggingAlg = JetBTaggingAlg.getEventAlgo((ConfigFlags.BTagging.OutputFiles.Prefix + inputJets.replace("Jets","") + ConfigFlags.BTagging.GeneralToolSuffix).lower()) #Defined in JetBTaggingAlgConfig.py; Ends up to be "btagging_hlt_b(Jets)"
    BTaggingAlg.BTaggingCollectionName = recordable( btagname )
    BTaggingAlg.BTagJFVtxCollectionName = btagname_JFVtx
    BTaggingAlg.BTagSVCollectionName = btagname_SecVtx
    BTaggingAlg.JetLinkName = btagname_jetLink
    acc.merge(JetBTaggingAlg)


    return [acc,btagname]

