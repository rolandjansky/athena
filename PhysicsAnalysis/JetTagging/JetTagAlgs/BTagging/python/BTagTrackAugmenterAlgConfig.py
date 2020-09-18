# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

Analysis__BTagTrackAugmenterAlg=CompFactory.Analysis.BTagTrackAugmenterAlg

def BTagTrackAugmenterAlgCfg(ConfigFlags, TrackCollection = 'InDetTrackParticles', PrimaryVertexCollectionName = 'PrimaryVertices', **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           options:            Python dictionary of options to be passed to the alg.
    output: The CA."""

    acc = ComponentAccumulator()
    # Minimal configuration
    options = {}
    options['name'] = ('BTagTrackAugmenter').lower()
    options['TrackContainer'] = TrackCollection
    options['PrimaryVertexContainer'] = PrimaryVertexCollectionName

    # -- create the track augmenter algorithm
    acc.addEventAlgo(Analysis__BTagTrackAugmenterAlg(**options))

    return acc
