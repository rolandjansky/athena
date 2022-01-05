#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags

from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
from BTagging.BTagHighLevelAugmenterAlgConfig import BTagHighLevelAugmenterAlgCfg
from BTagging.HighLevelBTagAlgConfig import HighLevelBTagAlgCfg
from JetTagCalibration.JetTagCalibConfig import JetTagCalibCfg

def getFlavourTagging( inputJets, inputVertex, inputTracks, BTagName,
                       inputMuons = ""):

    acc = ComponentAccumulator()

    inputJetsPrefix = inputJets.replace("bJets","b")  # because Cfg functions internally re-append the 'Jets' string

    #Particle to Jet Association
    acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJetsPrefix, inputTracks, "TracksForBTagging"))

    if inputMuons:
        acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJetsPrefix, inputMuons, "MuonsForBTagging"))
        Muons = "MuonsForBTagging"
    else:
        Muons = ""

    #Secondary Vertexing
    SecVertexers = [ 'JetFitter', 'SV1' ]
    for sv in SecVertexers:
        acc.merge(JetSecVtxFindingAlgCfg(ConfigFlags, inputJetsPrefix, inputVertex, sv, "TracksForBTagging"))
        acc.merge(JetSecVertexingAlgCfg(ConfigFlags, BTagName, inputJetsPrefix, inputTracks, inputVertex, sv))

    #Run Run2 taggers, i.e. IP2D, IP3D, SV1, JetFitter, MV2c10
    acc.merge(JetBTaggingAlgCfg(ConfigFlags, BTaggingCollection=BTagName, JetCollection=inputJetsPrefix, PrimaryVertexCollectionName=inputVertex, TaggerList=ConfigFlags.BTagging.Run2TrigTaggers, SetupScheme="Trig", SecVertexers = SecVertexers, Tracks = "TracksForBTagging", Muons = Muons))

    #Track Augmenter
    acc.merge(BTagTrackAugmenterAlgCfg(ConfigFlags, TrackCollection=inputTracks, PrimaryVertexCollectionName=inputVertex))

    #Jet Augmenter
    acc.merge(BTagHighLevelAugmenterAlgCfg(
        ConfigFlags,
        JetCollection=inputJets,
        BTagCollection=BTagName,
        Associator="BTagTrackToJetAssociator",
        TrackCollection=inputTracks,
    ))

    # Jet Calibration
    acc.merge(JetTagCalibCfg(ConfigFlags, scheme="Trig",
                             TaggerList=ConfigFlags.BTagging.Run2TrigTaggers,
                             NewChannel = [f"{inputJetsPrefix}->{inputJetsPrefix},AntiKt4EMTopo"])) # "HLT_bJets" is the name of the b-jet JetContainer

    #Run new Run3 taggers, i.e. DL1, RNNIP, DL1r
    tagger_list = [
        # r21 offline b-tagging NNs
        'BTagging/201903/rnnip/antikt4empflow/network.json',
        'BTagging/201903/dl1r/antikt4empflow/network.json',
        'BTagging/201903/dl1/antikt4empflow/network.json',

        # loose track selection, trained on r21
        'BTagging/20210517/dipsLoose/antikt4empflow/network.json',
        # IP3D track selection, trained on r21
        'BTagging/20210517/dips/antikt4empflow/network.json',
        # DL1d, uses "loose" dips above
        'BTagging/20210519r22/dl1d/antikt4empflow/network.json',
        # DL1d, uses IP3D dips above
        'BTagging/20210528r22/dl1d/antikt4empflow/network.json',

        # The following were the best offline R22 taggers according to
        #
        # https://ftag-docs.docs.cern.ch/algorithms/available_taggers/
        #
        # R22 retraining for DIPS, provides dipsLoose20210729
        'BTagging/20210729/dipsLoose/antikt4empflow/network.json',
        # R22 retraining with the above DIPS, provides DL1dv00
        'BTagging/20210824r22/dl1dLoose/antikt4empflow/network.json',
    ]
    for jsonFile in tagger_list:
        acc.merge(HighLevelBTagAlgCfg(ConfigFlags, BTaggingCollection=BTagName, TrackCollection=inputTracks, NNFile=jsonFile) )


    return acc
