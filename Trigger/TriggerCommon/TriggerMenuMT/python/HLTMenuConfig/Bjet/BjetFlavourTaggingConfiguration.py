#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from TrigEDMConfig.TriggerEDMRun3 import recordable

from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
from BTagging.BTagHighLevelAugmenterAlgConfig import BTagHighLevelAugmenterAlgCfg
from BTagging.HighLevelBTagAlgConfig import HighLevelBTagAlgCfg

def getFlavourTagging( inputJets, inputVertex, inputTracks ):

    acc = ComponentAccumulator()
    
    #Output container names as defined in TriggerEDMRun3
    BTagName = recordable("HLT_BTagging")

    #Particle to Jet Association
    TrackToJetAssociators = ['BTagTrackToJetAssociator']
    kwargs = {}
    kwargs['Release'] = '22'
    acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputTracks, TrackToJetAssociators[0], **kwargs))

    #Secondary Vertexing
    SecVertexingAndAssociators = {'JetFitter':'BTagTrackToJetAssociator','SV1':'BTagTrackToJetAssociator'}
    for k, v in SecVertexingAndAssociators.items():
        if v not in TrackToJetAssociators:
            raise RuntimeError( v + ' is not configured')
        acc.merge(JetSecVtxFindingAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputVertex, k, v))
        acc.merge(JetSecVertexingAlgCfg(ConfigFlags, BTagName, inputJets.replace("Jets",""), inputVertex, k, v))
    
    #Run Run2 taggers, i.e. IP2D, IP3D, SV1, JetFitter, MV2c10
    acc.merge(JetBTaggingAlgCfg(ConfigFlags, BTaggingCollection=BTagName, JetCollection=inputJets.replace("Jets",""), PrimaryVertexCollectionName=inputVertex, TaggerList=ConfigFlags.BTagging.Run2TrigTaggers, SetupScheme="Trig", SVandAssoc=SecVertexingAndAssociators))

    #Track Augmenter
    acc.merge(BTagTrackAugmenterAlgCfg(ConfigFlags, TrackCollection=inputTracks, PrimaryVertexCollectionName=inputVertex))
    
    #Jet Augmenter
    acc.merge(BTagHighLevelAugmenterAlgCfg(ConfigFlags, JetCollection=inputJets.replace("Jets",""), BTagCollection=BTagName, Associator=TrackToJetAssociators[0]))

    #Run new Run3 taggers, i.e. DL1, RNNIP, DL1r
    postTagDL2JetToTrainingMap={
        'AntiKt4EMPFlow': [
        #'BTagging/201903/smt/antikt4empflow/network.json',
        'BTagging/201903/rnnip/antikt4empflow/network.json',
        'BTagging/201903/dl1r/antikt4empflow/network.json',
        'BTagging/201903/dl1/antikt4empflow/network.json',
        #'BTagging/201903/dl1rmu/antikt4empflow/network.json',
        ]
    }
    for jsonFile in postTagDL2JetToTrainingMap['AntiKt4EMPFlow']:
        acc.merge(HighLevelBTagAlgCfg(ConfigFlags, BTaggingCollection=BTagName, TrackCollection=inputTracks, NNFile=jsonFile) )


    return [acc,BTagName]

