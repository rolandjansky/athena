# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

# importi the FullLinearizedTrackFactory configurable
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory

# define the class
class BTagFullLinearizedTrackFactoryCfg( Trk__FullLinearizedTrackFactory ):
     # constructor
    def __init__(self, name = 'FullLinearizedTrackFactory', useBTagFlagsDefaults = True, options = {}):
        """Sets up a BTagFullLinearizedTrackFactory tool and returns it.

        input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
        output: The actual tool, which can then be added to ToolSvc via ToolSvc += output."""

        if useBTagFlagsDefaults:
            atlasExtrapolator = AtlasExtrapolatorCfg('AtlasExtrapolator')
            defaults = { 'Extrapolator'            : atlasExtrapolator}
            for option in defaults:
                options.setdefault(option, defaults[option])

        options['name'] = name
        Trk__FullLinearizedTrackFactory.__init__(self,**options)




