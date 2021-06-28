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

def getFlavourTagging( inputJets, inputVertex, inputTracks , inputMuons = ""):

    acc = ComponentAccumulator()
    
    #Output container names as defined in TriggerEDMRun3
    BTagName = recordable("HLT_BTagging")

    #Particle to Jet Association
    acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputTracks, "TracksForBTagging"))

    if inputMuons:
        acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputMuons, "MuonsForBTagging"))
        Muons = "MuonsForBTagging"
    else:
        Muons = ""

    #Secondary Vertexing
    SecVertexers = [ 'JetFitter', 'SV1' ]
    for sv in SecVertexers:
        acc.merge(JetSecVtxFindingAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputVertex, sv, "TracksForBTagging"))
        acc.merge(JetSecVertexingAlgCfg(ConfigFlags, BTagName, inputJets.replace("Jets",""), inputTracks, inputVertex, sv))
    
    #Run Run2 taggers, i.e. IP2D, IP3D, SV1, JetFitter, MV2c10
    acc.merge(JetBTaggingAlgCfg(ConfigFlags, BTaggingCollection=BTagName, JetCollection=inputJets.replace("Jets",""), PrimaryVertexCollectionName=inputVertex, TaggerList=ConfigFlags.BTagging.Run2TrigTaggers, SetupScheme="Trig", SecVertexers = SecVertexers, Tracks = "TracksForBTagging", Muons = Muons))

    #Track Augmenter
    acc.merge(BTagTrackAugmenterAlgCfg(ConfigFlags, TrackCollection=inputTracks, PrimaryVertexCollectionName=inputVertex))
    
    #Jet Augmenter
    acc.merge(BTagHighLevelAugmenterAlgCfg(
        ConfigFlags,
        JetCollection=inputJets.replace("Jets",""),
        BTagCollection=BTagName,
        Associator="BTagTrackToJetAssociator",
        TrackCollection=inputTracks,
    ))

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