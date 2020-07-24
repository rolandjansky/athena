# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# import the NewLikelihoodTool configurable
Analysis__NewLikelihoodTool=CompFactory.Analysis.NewLikelihoodTool

def NewLikelihoodToolCfg( flags, name = 'NewLikelihoodTool', taggername = 'IP2D', scheme = '', useBTagFlagsDefaults = True, **options):
    """Sets up a NewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    taggerName                          default: "IP2D"

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        if taggername != 'SMT':
            defaults = {'taggerName'                          : taggername }
        else:
            defaults = {'taggerName'                          : taggername,
                        'smoothNTimes'                        : 0,
                        'normalizedProb'                      : True,
                        'interpolate'                         : True }
            if(flags.BTagging.RunModus == 'reference'):
                defaults['smoothNTimes'] = 1
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    if scheme == 'Trig':
        options['HistosKey'] = 'JetTagTrigCalibHistosKey'
    acc.setPrivateTools(Analysis__NewLikelihoodTool(**options))

    return acc

