# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from JetTagTools.BTagFullLinearizedTrackFactoryConfig import BTagFullLinearizedTrackFactoryCfg
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

# import the TrackToVertexIPEstimator configurable
Trk__TrackToVertexIPEstimator=CompFactory.Trk.TrackToVertexIPEstimator

def BTagTrackToVertexIPEstimatorCfg( flags, name = 'TrkToVxIPEstimator', useBTagFlagsDefaults = True, **options ):
    """Sets up a TrackToVertexIPEstimator tool and returns it.

    The following options have BTaggingFlags defaults:

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        btagFullLinearizedTrackFactory = acc.popToolsAndMerge(BTagFullLinearizedTrackFactoryCfg(flags, 'FullLinearizedTrkFactory'))
        atlasExtrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags, 'AtlasExtrapolator'))
        defaults = { 'Extrapolator' : atlasExtrapolator,
                     'LinearizedTrackFactory' : btagFullLinearizedTrackFactory, }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(Trk__TrackToVertexIPEstimator( **options))

    return acc
