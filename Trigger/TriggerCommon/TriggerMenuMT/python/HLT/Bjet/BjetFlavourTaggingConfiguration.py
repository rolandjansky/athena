#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.AllConfigFlags import ConfigFlags

from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg

from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
from BTagging.BTagHighLevelAugmenterAlgConfig import BTagHighLevelAugmenterAlgCfg
from BTagging.HighLevelBTagAlgConfig import HighLevelBTagAlgCfg
from BTagging.HighLevelBTagAlgConfig import getStaticTrackVars
from JetTagCalibration.JetTagCalibConfig import JetTagCalibCfg

def getFlavourTagging( inputJets, inputVertex, inputTracks, BTagName,
                       inputMuons = ""):

    acc = ComponentAccumulator()

    inputJetsPrefix = inputJets.replace("bJets","b")  # because Cfg functions internally re-append the 'Jets' string

    #Particle to Jet Association
    acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJets, inputTracks, "TracksForBTagging"))

    if inputMuons:
        acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJets, inputMuons, "MuonsForBTagging"))
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

        # The following were the best online R22 taggers according to
        #
        # https://ftag-docs.docs.cern.ch/algorithms/available_taggers/
        #
        # R22 retraining for DIPS, provides dips20211116 with a loose track selection
        'BTagging/20211216trig/dips/AntiKt4EMPFlow/network.json',
        # R22 retraining with the above DIPS, provides DL1d20211216
        'BTagging/20211216trig/dl1d/AntiKt4EMPFlow/network.json',
    ]
    for jsonFile in tagger_list:
        acc.merge(HighLevelBTagAlgCfg(ConfigFlags, BTaggingCollection=BTagName, TrackCollection=inputTracks, NNFile=jsonFile) )


    return acc

def getFastFlavourTagging( flags, inputJets, inputVertex, inputTracks):
    """
    This function tags jets directly: there is no  b-tagging object
    """

    ca = ComponentAccumulator()

    # first add the track augmentation to define peragee coordinates
    jet_name = inputJets
    trackIpPrefix='simpleIp_'
    ca.merge(
        OnlineBeamspotIpAugmenterCfg(
            flags,
            tracks=inputTracks,
            vertices=inputVertex,
            trackIpPrefix=trackIpPrefix,
        )
    )

    # now we assicoate the tracks to the jet
    tracksOnJetDecoratorName = "TracksForMinimalJetTag"
    ca.merge(JetParticleAssociationAlgCfg(
        flags,
        JetCollection=jet_name,
        InputParticleCollection=inputTracks,
        OutputParticleDecoration=tracksOnJetDecoratorName,
    ))

    # Now we have to add an algorithm that tags the jets with dips
    # The input and output remapping is handled via a map in DL2.
    #
    # The file above adds dipsLoose20210517_p*, we'll call them
    # dips_p* on the jet.
    nnFile = 'BTagging/20210517/dipsLoose/antikt4empflow/network.json'
    variableRemapping = {
        'BTagTrackToJetAssociator': tracksOnJetDecoratorName,
        **{f'dipsLoose20210517_p{x}': f'fastDips_p{x}' for x in 'cub'},
        'btagIp_': trackIpPrefix,
    }
    # not all the keys that the NN requests are declaired. This will
    # cause an algorithm stall if we don't explicetly tell it that it
    # can ignore some of them.
    missingKeys = getStaticTrackVars(inputTracks)
    # The 'jetLink' is a special special hack: it won't be used
    # because we're tagging the jet directly here, but DL2 still
    # reports it as a dependency.
    missingKeys += [f'{jet_name}.jetLink']
    nnAlgoKey = nnFile.replace('/','_').split('.')[0]
    ca.addEventAlgo(
        CompFactory.FlavorTagDiscriminants.JetTagDecoratorAlg(
            name='_'.join([
                'simpleJetTagAlg',
                jet_name,
                inputTracks,
                nnAlgoKey,
            ]),
            container=jet_name,
            constituentContainer=inputTracks,
            undeclaredReadDecorKeys=missingKeys,
            decorator=CompFactory.FlavorTagDiscriminants.DL2Tool(
                name='_'.join([
                    'simpleDipsToJet',
                    nnAlgoKey,
                ]),
                nnFile=nnFile,
                variableRemapping=variableRemapping,
                # note that the tracks are associated to the jet as
                # and IParticle container.
                trackLinkType='IPARTICLE',
            ),
        )
    )
    return ca


def OnlineBeamspotIpAugmenterCfg(cfgFlags, tracks, vertices='',
                                 trackIpPrefix='simpleIp_'):
    ca = ComponentAccumulator()

    pfx = 'online'
    i = 'EventInfo'
    x = f'{i}.{pfx}BeamPosX'
    y = f'{i}.{pfx}BeamPosY'
    z = f'{i}.{pfx}BeamPosZ'
    sig_x = f'{i}.{pfx}BeamPosSigmaX'
    sig_y = f'{i}.{pfx}BeamPosSigmaY'
    sig_z = f'{i}.{pfx}BeamPosSigmaZ'
    cov_xy = f'{i}.{pfx}BeamPosSigmaXY'
    tilt_XZ = f'{i}.{pfx}BeamTiltXZ'
    tilt_YZ = f'{i}.{pfx}BeamTiltYZ'
    status = f'{i}.{pfx}BeamStatus'

    ca.merge(BeamSpotCondAlgCfg(cfgFlags))
    ca.addEventAlgo(CompFactory.xAODMaker.EventInfoBeamSpotDecoratorAlg(
        beamPosXKey=x,
        beamPosYKey=y,
        beamPosZKey=z,
        beamPosSigmaXKey=sig_x,
        beamPosSigmaYKey=sig_y,
        beamPosSigmaZKey=sig_z,
        beamPosSigmaXYKey=cov_xy,
        beamTiltXZKey=tilt_XZ,
        beamTiltYZKey=tilt_YZ,
        beamStatusKey=status,
    ))

    ca.addEventAlgo(
        CompFactory.FlavorTagDiscriminants.PoorMansIpAugmenterAlg(
            name='_'.join([
                'SimpleTrackAugmenter',
                tracks,
                vertices,
                trackIpPrefix,
            ]).replace('__','_').rstrip('_'),
            trackContainer=tracks,
            primaryVertexContainer=vertices,
            prefix=trackIpPrefix,
            beamspotSigmaX=sig_x,
            beamspotSigmaY=sig_y,
            beamspotSigmaZ=sig_z,
            beamspotCovarianceXY=cov_xy
        )
    )
    return ca
