# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from JetTagTools.BTagTrackToVertexToolConfig import BTagTrackToVertexToolCfg

# import the TrackSelector configurable
from JetTagTools.JetTagToolsConf import Analysis__TrackSelector

# define the class
class IPTrackSelectorCfg( Analysis__TrackSelector ):
    # constructor
    def __init__(self, name = 'IPTrackSelector', useBTagFlagsDefaults = True, options = {}): 
        """Sets up a IPTrackSelector tool and returns it.

        The following options have BTaggingFlags defaults:

        useBLayerHitPrediction              default: True
        usepTDepTrackSel                    default: False
        nHitBLayer                          default: 0

        input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
        output: The actual tool."""

        if useBTagFlagsDefaults:
            trackToVertexTool = BTagTrackToVertexToolCfg('BTagTrackToVertexTool')
            defaults = {'useBLayerHitPrediction' : True,
                     'nHitBLayer'             : 0 ,
                     'usepTDepTrackSel'       : False,
                     'trackToVertexTool'      : trackToVertexTool}

            for option in defaults:
                options.setdefault(option, defaults[option])

        options['name'] = name
        Analysis__TrackSelector.__init__(self, **options)
