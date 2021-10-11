# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

FlavorTagDiscriminants__DL2Tool=CompFactory.FlavorTagDiscriminants.DL2Tool

def DL2ToolCfg(ConfigFlags, NNFile = '', **options):
    acc = ComponentAccumulator()

    options['nnFile'] = NNFile
    options['name'] = "decorator"

    # This is a hack to accomodate the older b-tagging training with
    # old names for variables. We should be able to remove it when we
    # move over to the 2020 / 2021 retraining.
    if '201903' in NNFile and 'dl1' in NNFile:
        remap = {}
        for aggragate in ['minimum','maximum','average']:
            remap[f'{aggragate}TrackRelativeEta'] = (
                f'JetFitterSecondaryVertex_{aggragate}AllJetTrackRelativeEta')
        options['variableRemapping'] = remap

    dl2 = FlavorTagDiscriminants__DL2Tool(**options)

    acc.setPrivateTools(dl2)

    return acc

def HighLevelBTagAlgCfg(ConfigFlags, BTaggingCollection, TrackCollection, NNFile = "", **options):

    acc = ComponentAccumulator()

    nn_name = NNFile.replace("/", "_").replace("_network.json", "")
    dl2 = acc.popToolsAndMerge(DL2ToolCfg(ConfigFlags, NNFile, **options))

    name = '_'.join([nn_name.lower(), BTaggingCollection])

    # some things should not be declared as date dependencies: it will
    # make the trigger sad.
    #
    # In the case of tracking it's mostly static variables that are a
    # problem.
    static_track_vars = {
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
    }
    veto_list = [f'{TrackCollection}.{x}' for x in static_track_vars]
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
    veto_list += [f'{BTaggingCollection}.{x}' for x in undeclared_btag]

    decorAlg = CompFactory.FlavorTagDiscriminants.BTagDecoratorAlg(
        name=name,
        btagContainer=BTaggingCollection,
        trackContainer=TrackCollection,
        decorator=dl2,
        undeclaredReadDecorKeys=sorted(veto_list),
    )

    # -- create the association algorithm
    acc.addEventAlgo(decorAlg)

    return acc
