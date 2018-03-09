# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MultivariateFlipTagManager
# Author: Marie Lanfermann (October 2015)
from BTagging.BTaggingFlags import BTaggingFlags

metaMultivariateFlipTagManager = { 'IsATagger'          : True,
#              'xAODBaseName'       : 'MultivariateFlipTagManager',
              'DependsOn'          : ['AtlasExtrapolator',
                                      'BTagTrackToVertexTool',
                                      'IP2DNegTag',
                                      'IP3DNegTag',
                                      'NewJetFitterVxFinderFlip',
                                      'SV0Tag',
                                      'SV1FlipTag'],
                  'ToolCollection'     : 'MultivariateFlipTagManager' }

def toolMultivariateFlipTagManager(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MultivariateFlipTagManager tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    inputSV0SourceName                  default: "SV0"
    inputSV1SourceName                  default: "SV1Flip"
    inputIP2DSourceName                 default: "IP2DNeg"
    inputIP3DSourceName                 default: "IP3DNeg"
    inputJFSourceName                   default: "JetFitterFlip"

    input:             name: The name of the tool (should be unique).
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'inputSV0SourceName'               : 'SV0',
                     'inputSV1SourceName'               : 'SV1Flip',
                     'inputIP2DSourceName'              : 'IP2DNeg',
                     'inputIP3DSourceName'              : 'IP3DNeg',
                     'inputJFSourceName'                : 'JetFitterFlip',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MultivariateTagManager
    return Analysis__MultivariateTagManager(**options)
