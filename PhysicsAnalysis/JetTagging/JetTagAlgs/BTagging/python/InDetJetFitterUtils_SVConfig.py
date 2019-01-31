# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from JetTagTools.BTagFullLinearizedTrackFactoryConfig import BTagFullLinearizedTrackFactoryCfg
from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

# importi the FullLinearizedTrackFactory configurable
from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetJetFitterUtils


# define the class
class InDetJetFitterUtils_SVCfg( InDet__InDetJetFitterUtils ):
     # constructor
    def __init__(self, name = 'InDetJFUtils', useBTagFlagsDefaults = True, options = {}):
        """Sets up a InDetJetFitterUtils tool and returns it.

        The following options have BTaggingFlags defaults:

        input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
        output: The actual tool."""
        if useBTagFlagsDefaults:
            jetFitterFullLinearizedTrackFactory = BTagFullLinearizedTrackFactoryCfg('JFFullLinearizedTrackFactory')
            jetFitterExtrapolator = AtlasExtrapolatorCfg('JFExtrapolator')
            defaults = { 'LinearizedTrackFactory': jetFitterFullLinearizedTrackFactory,
                     'Extrapolator' : jetFitterExtrapolator }
            for option in defaults:
                options.setdefault(option, defaults[option])
        options['name'] = name
        InDet__InDetJetFitterUtils.__init__(self, **options)
