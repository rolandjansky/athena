# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# FLS: copied from the original MultivariateTagManager and implemented the TrigHybridTag correction

# Configuration functions for MultivariateTagManager
# Author: Marie Lanfermann (October 2015)
from BTagging.BTaggingFlags import BTaggingFlags

metaMultivariateTrigHybridTagManager = {'IsATagger': True,
                                        #              'xAODBaseName'       : 'MultivariateTrigHybridTagManager',
                                        'DependsOn': ['AtlasExtrapolator',
                                                      'BTagTrackToVertexTool',
                                                      'IP2DTrigHybridTag',
                                                      'IP3DTrigHybridTag',
                                                      'NewJetFitterVxFinder',
                                                      'SV1Tag'],
                                        'ToolCollection': 'MultivariateTrigHybridTagManager'}


def toolMultivariateTrigHybridTagManager(name, useBTagFlagsDefaults=True, **options):
    """Sets up a MultivariateTrigHybridTagManager tool and returns it.

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
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'inputSV0SourceName': 'SV0',
                    'inputSV1SourceName': 'SV1',
                    'inputIP2DSourceName': 'IP2DTrigHybrid',
                    'inputIP3DSourceName': 'IP3DTrigHybrid',
                    'inputJFSourceName': 'JetFitter',
                    }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MultivariateTagManager
    return Analysis__MultivariateTagManager(**options)
