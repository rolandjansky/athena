# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from JetTagTools.BTagFullLinearizedTrackFactoryConfig import BTagFullLinearizedTrackFactoryCfg
from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

# import the TrackToVertexIPEstimator configurable
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator

# define the class
class BTagTrackToVertexIPEstimatorCfg( Trk__TrackToVertexIPEstimator ):
     # constructor
    def __init__(self, name = 'TrkToVxIPEstimator', useBTagFlagsDefaults = True, options = {}):
        """Sets up a TrackToVertexIPEstimator tool and returns it.

        The following options have BTaggingFlags defaults:

        input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
        output: The actual tool."""

        if useBTagFlagsDefaults:
            btagFullLinearizedTrackFactory = BTagFullLinearizedTrackFactoryCfg('FullLinearizedTrackFactory')
            atlasExtrapolator = AtlasExtrapolatorCfg('AtlasExtrapolator')
            defaults = { 'Extrapolator' : atlasExtrapolator,
                     'LinearizedTrackFactory' : btagFullLinearizedTrackFactory, }
            for option in defaults:
                options.setdefault(option, defaults[option])
        options['name'] = name

        Trk__TrackToVertexIPEstimator.__init__(self, **options)
