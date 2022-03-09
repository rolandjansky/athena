# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from JetTagTools.BTagTrackToVertexIPEstimatorConfig import BTagTrackToVertexIPEstimatorCfg
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg


def BTagTrackAugmenterAlgCfg(ConfigFlags, TrackCollection = 'InDetTrackParticles', PrimaryVertexCollectionName = 'PrimaryVertices', **options):

    acc = ComponentAccumulator()
    # Minimal configuration
    # @TODO why is options re-initialised to an empty dict ?
    options = {}
    options['name'] = ('BTagTrackAugmenter').lower() + PrimaryVertexCollectionName + TrackCollection
    options['TrackContainer'] = TrackCollection
    options['PrimaryVertexContainer'] = PrimaryVertexCollectionName
    if 'TrackToVertexIPEstimator' not in  options :
        options.setdefault('TrackToVertexIPEstimator',acc.popToolsAndMerge(BTagTrackToVertexIPEstimatorCfg(ConfigFlags, 'TrkToVxIPEstimator') ))
    if 'Extrapolator' not in options :
        options.setdefault('Extrapolator', acc.popToolsAndMerge(AtlasExtrapolatorCfg(ConfigFlags)))

    # -- create the track augmenter algorithm
    acc.addEventAlgo(CompFactory.Analysis.BTagTrackAugmenterAlg(**options))

    return acc
