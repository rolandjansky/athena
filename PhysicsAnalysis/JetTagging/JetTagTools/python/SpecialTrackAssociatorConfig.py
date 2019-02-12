# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# import the TrackToVertexIPEstimator configurable
from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TightTrackVertexAssociationTool

# define the class
class SpecialTrackAssociatorCfg( CP__TightTrackVertexAssociationTool ):
     # constructor
    def __init__(self, name = 'SpecialTrackAssociator', useBTagFlagsDefaults = True, options = {}):
        """Sets up a SpecialTrackAssociator tool and returns it.

        The following options have BTaggingFlags defaults:

        input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
        output: The actual tool."""
        if useBTagFlagsDefaults:
            defaults = { 'dzSinTheta_cut'         : 3,
                     'doPV'                   : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
        options['name'] = name
 
        CP__TightTrackVertexAssociationTool.__init__(self, **options)
