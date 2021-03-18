# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from JetTagTools.BTagTrackToVertexIPEstimatorConfig import BTagTrackToVertexIPEstimatorCfg
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

Analysis__BTagTrackAugmenterAlg=CompFactory.Analysis.BTagTrackAugmenterAlg

def BTagTrackAugmenterAlgCfg(ConfigFlags, TrackCollection = 'InDetTrackParticles', PrimaryVertexCollectionName = 'PrimaryVertices', **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           options:            Python dictionary of options to be passed to the alg.
    output: The CA."""

    acc = ComponentAccumulator()
    # Minimal configuration
    # @TODO why is options re-initialised to an empty dict ?
    options = {}
    options['name'] = ('BTagTrackAugmenter').lower()
    options['TrackContainer'] = TrackCollection
    options['PrimaryVertexContainer'] = PrimaryVertexCollectionName
    if 'TrackToVertexIPEstimator' not in  options :
        options.setdefault('TrackToVertexIPEstimator',acc.popToolsAndMerge(BTagTrackToVertexIPEstimatorCfg(ConfigFlags, 'TrkToVxIPEstimator') ))
    if 'Extrapolator' not in options :
        exrtrapolator_acc = AtlasExtrapolatorCfg(ConfigFlags)
        options.setdefault('Extrapolator', acc.popToolsAndMerge(exrtrapolator_acc))

    # -- create the track augmenter algorithm
    acc.addEventAlgo(Analysis__BTagTrackAugmenterAlg(**options))

    return acc
