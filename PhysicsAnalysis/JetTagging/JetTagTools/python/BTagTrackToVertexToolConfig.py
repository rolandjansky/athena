# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

# import the TrackToVertexIPEstimator configurable
from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex

# define the class
class BTagTrackToVertexToolCfg( Reco__TrackToVertex ):
     # constructor
    def __init__(self, name = 'BTagTrackToVertexTool', useBTagFlagsDefaults = True, options = {}):
        """Sets up a BTagTrackToVertexTool tool and returns it.

        input:             name: The name of the tool (should be unique).
                           useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.

                  **options: Python dictionary with options for the tool.
        output: The actual tool."""

        atlasExtrapolator = AtlasExtrapolatorCfg('AtlasExtrapolator')
        options.setdefault('Extrapolator', atlasExtrapolator)
        options['name'] = name
 
        Reco__TrackToVertex.__init__(self, **options)
