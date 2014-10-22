# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MVbTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaMVbTag = { 'IsATagger'          : True,
               'DependsOn'          : ['AtlasExtrapolator',
                                       'BTagTrackToVertexTool',
                                       'BTagCalibrationBrokerTool',
                                       'IP2DTag',
                                       'IP3DTag',
                                       'JetFitterTagCOMBNN',
                                       'SV0Tag',
                                       'SV1Tag'],
               'CalibrationFolders' : ['MVb',],
               'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
               'ToolCollection'     : 'MVbTag' }

def toolMVbTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MVbTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerName                          default: "MVb"
    taggerNameBase                      default: "MVb"
    xAODBaseName                        default: "MVb"
    inputSV0SourceName                  default: "SV0"
    inputSV1SourceName                  default: "SV1"
    inputIP2DSourceName                 default: "IP2D"
    inputIP3DSourceName                 default: "IP3D"
    inputJFSourceName                   default: "JetFitter"
    inputJFProbSourceName               default: "JetFitterCombNN"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'taggerName'                       : 'MVb',
                     'taggerNameBase'                   : 'MVb',
                     'xAODBaseName'                     : 'MVb',
                     'inputSV0SourceName'               : 'SV0',
                     'inputSV1SourceName'               : 'SV1',
                     'inputIP2DSourceName'              : 'IP2D',
                     'inputIP3DSourceName'              : 'IP3D',
                     'inputJFSourceName'                : 'JetFitter',
                     'inputJFProbSourceName'            : 'JetFitterCombNN',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MVbTag
    return Analysis__MVbTag(**options)
