# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MVbFlipTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaMVbFlipTag = { 'IsATagger'          : False,
                   'xAODBaseName'       : 'MVbFlip',
                   'DependsOn'          : ['AtlasExtrapolator',
                                           'BTagTrackToVertexTool',
                                           'BTagCalibrationBrokerTool',
                                           'IP2DNegTag',
                                           'IP3DNegTag',
                                           'JetFitterTagCOMBNNIP3DNeg',
                                           'SV0Tag',
                                           'SV1FlipTag'],
                   'CalibrationFolders' : ['MVb',],
                   'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                   'ToolCollection'     : 'MVbFlipTag' }

def toolMVbFlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MVbTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerName                          default: "MVbFlip"
    taggerNameBase                      default: "MVb"
    inputSV0SourceName                  default: "SV0"
    inputSV1SourceName                  default: "SV1Neg"
    inputIP2DSourceName                 default: "IP2DNeg"
    inputIP3DSourceName                 default: "IP3DNeg"
    inputJFSourceName                   default: "JetFitterFlip"
    inputJFProbSourceName               default: "JetFitterCombNNIP3DNeg"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'taggerName'                       : 'MVbFlip',
                     'taggerNameBase'                   : 'MVb',
                     'inputSV0SourceName'               : 'SV0',
                     'inputSV1SourceName'               : 'SV1Flip',
                     'inputIP2DSourceName'              : 'IP2DNeg',
                     'inputIP3DSourceName'              : 'IP3DNeg',
                     'inputJFSourceName'                : 'JetFitterFlip',
                     'inputJFProbSourceName'            : 'JetFitterCombNNIP3DNeg',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MVbTag
    return Analysis__MVbTag(**options)
