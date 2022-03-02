# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def DL2ToolCfg(ConfigFlags, NNFile = '', FlipConfig='STANDARD' , **options):
    acc = ComponentAccumulator()

    options['nnFile'] = NNFile
    options['name'] = "decorator"

    # default is "STANDARD" in case of a setup of the standard b-taggers. "NEGATIVE_IP_ONLY" [and "FLIP_SIGN"] if want to set up the flip taggers
    # naming convention, see here: https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/JetTagging/FlavorTagDiscriminants/Root/FlipTagEnums.cxx

    options['flipTagConfig'] = FlipConfig

    # This is a hack to accomodate the older b-tagging training with
    # old names for variables. We should be able to remove it when we
    # move over to the 2020 / 2021 retraining.
    if '201903' in NNFile and 'dl1' in NNFile:
        remap = {}
        for aggragate in ['minimum','maximum','average']:
            remap[f'{aggragate}TrackRelativeEta'] = (
                f'JetFitterSecondaryVertex_{aggragate}AllJetTrackRelativeEta')
        options['variableRemapping'] = remap

    dl2 = CompFactory.FlavorTagDiscriminants.DL2Tool(**options)

    acc.setPrivateTools(dl2)

    return acc

def GNNToolCfg(ConfigFlags, NNFile = '', **options):
    acc = ComponentAccumulator()

    options['nnFile'] = NNFile
    options['name'] = "decorator"

    gnntool = CompFactory.FlavorTagDiscriminants.GNNTool(**options)

    acc.setPrivateTools(gnntool)

    return acc

def getStaticTrackVars(TrackCollection):
    # some things should not be declared as date dependencies: it will
    # make the trigger sad.
    #
    # In the case of tracking it's mostly static variables that are a
    # problem.
    static_track_vars = [
        'numberOfInnermostPixelLayerHits',
        'numberOfInnermostPixelLayerSharedHits',
        'numberOfInnermostPixelLayerSplitHits',
        'numberOfNextToInnermostPixelLayerHits',
        'numberOfPixelDeadSensors',
        'numberOfPixelHits',
        'numberOfPixelHoles',
        'numberOfPixelSharedHits',
        'numberOfPixelSplitHits',
        'numberOfSCTDeadSensors',
        'numberOfSCTHits',
        'numberOfSCTHoles',
        'numberOfSCTSharedHits',
        'chiSquared',
        'numberDoF',
        'qOverP',
    ]
    return [f'{TrackCollection}.{x}' for x in static_track_vars]

def getUndeclaredBtagVars(BTaggingCollection):
    #
    # In the case of b-tagging we should really declare these
    # variables using WriteDecorHandle, but this is very much a work
    # in progress.
    #
    # We should revisit this once in a while, last time this was
    # checked was:
    #
    #  - 20210602
    #
    undeclared_btag = [
        'JetFitter_N2Tpair',
        'JetFitter_energyFraction',
        'JetFitter_mass',
        'JetFitter_nSingleTracks',
        'JetFitter_nTracksAtVtx',
        'JetFitter_nVTX',
        'JetFitter_significance3d',
        'SV1_L3d',
        'SV1_Lxy',
        'SV1_N2Tpair',
        'SV1_NGTinSvx',
        'SV1_deltaR',
        'SV1_efracsvx',
        'SV1_masssvx',
        'SV1_significance3d',
        'BTagTrackToJetAssociator',
    ]
    return [f'{BTaggingCollection}.{x}' for x in undeclared_btag]

def HighLevelBTagAlgCfg(ConfigFlags, BTaggingCollection, TrackCollection, NNFile = "", FlipConfig="STANDARD" , **options):

    acc = ComponentAccumulator()

    NNFile_extension = NNFile.split(".")[-1]
    if NNFile_extension == "json":
        nn_name = NNFile.replace("/", "_").replace("_network.json", "")
        decorator = acc.popToolsAndMerge(DL2ToolCfg(ConfigFlags, NNFile,FlipConfig=FlipConfig ,**options))
    elif NNFile_extension == "onnx":
        nn_name = NNFile.replace("/", "_").replace(".onnx", "")
        decorator = acc.popToolsAndMerge(GNNToolCfg(ConfigFlags, NNFile, **options))
    else:
        raise ValueError("HighLevelBTagAlgCfg: Wrong NNFile extension. Please check the NNFile argument")

    name = '_'.join([nn_name.lower(), BTaggingCollection])

    # Ensure different names for standard and flip taggers
    if FlipConfig != "STANDARD":
        FlipConfig_name = FlipConfig
        name = name + FlipConfig_name

    veto_list = getStaticTrackVars(TrackCollection)
    veto_list += getUndeclaredBtagVars(BTaggingCollection)

    decorAlg = CompFactory.FlavorTagDiscriminants.BTagDecoratorAlg(
        name=name,
        container=BTaggingCollection,
        constituentContainer=TrackCollection,
        decorator=decorator,
        undeclaredReadDecorKeys=veto_list,
    )

    # -- create the association algorithm
    acc.addEventAlgo(decorAlg)

    return acc
