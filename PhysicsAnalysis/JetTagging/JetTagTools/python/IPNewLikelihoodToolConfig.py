# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# import the NewLikelihoodTool configurable
from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool

# define the class
class IPNewLikelihoodToolCfg( Analysis__NewLikelihoodTool ):
    # constructor
    def __init__(self, name = 'NewLikelihoodTool', taggername = 'IP2D', useBTagFlagsDefaults = True, options = {}):
        """Sets up a NewLikelihoodTool tool and returns it.

        The following options have BTaggingFlags defaults:

        taggerName                          default: "IP2D"

        input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
        output: The actual tool."""


        options['name'] = name

        if useBTagFlagsDefaults:
            defaults = {'taggerName'                          : taggername }
            for option in defaults:
                options.setdefault(option, defaults[option])

        Analysis__NewLikelihoodTool.__init__(self, **options)
