# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MultivariateTagManager
# Author: Marie Lanfermann (October 2015)
from BTagging.BTaggingFlags import BTaggingFlags

metaMultivariateTagManager = { 'IsATagger'          : True,
#              'xAODBaseName'       : 'MultivariateTagManager',
              'DependsOn'          : ['AtlasExtrapolator',
                                      'BTagTrackToVertexTool',
                                      'IP2DTag',
                                      'IP3DTag',                                      
                                      'NewJetFitterVxFinder',
                                      'SV1Tag'],
              'ToolCollection'     : 'MultivariateTagManager' }

def toolMultivariateTagManager(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MultivariateTagManager tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    inputSV0SourceName                  default: "SV0"
    inputSV1SourceName                  default: "SV1"
    inputIP2DSourceName                 default: "IP2D"
    inputIP3DSourceName                 default: "IP3D"
    inputJFSourceName                   default: "JetFitter"

    input:             name: The name of the tool (should be unique).
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'inputSV0SourceName'               : 'SV0',
                     'inputSV1SourceName'               : 'SV1',
                     'inputIP2DSourceName'              : 'IP2D',
                     'inputIP3DSourceName'              : 'IP3D',
                     'inputJFSourceName'                : 'JetFitter',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MultivariateTagManager
    return Analysis__MultivariateTagManager(**options)
