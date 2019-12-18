# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

# import the TrackToVertexIPEstimator configurable
Reco__TrackToVertex=CompFactory.Reco__TrackToVertex

def BTagTrackToVertexToolCfg(flags, name = 'BTagTrackToVertexTool', useBTagFlagsDefaults = True, **options ):
    """Sets up a BTagTrackToVertexTool tool and returns it.

    input:             name: The name of the tool (should be unique).
                       useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.

              **options: Python dictionary with options for the tool.
    output: The actual tool."""

    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        accExtrapolator = AtlasExtrapolatorCfg(flags, 'AtlasExtrapolator')
        atlasExtrapolator = accExtrapolator.popPrivateTools()
        acc.merge(accExtrapolator)
        options.setdefault('Extrapolator', atlasExtrapolator)
    options['name'] = name
    acc.setPrivateTools(Reco__TrackToVertex( **options)) 

    return acc
