# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MV2mFlipTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaMV2mFlipTag = { 'IsATagger'          : True,
                    'xAODBaseName'       : 'MV2mFlip',
                    'DependsOn'          : ['AtlasExtrapolator',
                                            'BTagTrackToVertexTool',
                                            'BTagCalibrationBrokerTool',
                                            'IP2DNegTag',
                                            'IP3DNegTag',
                                            'JetFitterTagCOMBNNIP3DNeg',
                                            'SV0Tag',
                                            'SV1FlipTag'],
                    'CalibrationFolders' : ['MV2m',],
                    'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                    'ToolCollection'     : 'MV2mFlipTag' }

def toolMV2mFlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MV2mTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerName                          default: "MV2mFlip"
    taggerNameBase                      default: "MV2m"
    forceMV2CalibrationAlias            default: BTaggingFlags.ForceMV2CalibrationAlias
    MV2CalibAlias                       default: BTaggingFlags.MV2CalibAlias
    inputSV0SourceName                  default: "SV0"
    inputSV1SourceName                  default: "SV1Flip"
    inputIP2DSourceName                 default: "IP2DNeg"
    inputIP3DSourceName                 default: "IP3DNeg"
    inputJFSourceName                   default: "JetFitterFlip"
    inputJFProbSourceName               default: "JetFitterCombNNIP3DNeg"
    trainingConfig                      default: BTaggingFlags.MV2mTrainingConfig

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'taggerName'                       : 'MV2mFlip',
                     'taggerNameBase'                   : 'MV2m',
                     'forceMV2CalibrationAlias'         : BTaggingFlags.ForceMV2CalibrationAlias,
                     'MV2CalibAlias'                    : BTaggingFlags.MV2CalibAlias,
                     'inputSV0SourceName'               : 'SV0',
                     'inputSV1SourceName'               : 'SV1Flip',
                     'inputIP2DSourceName'              : 'IP2DNeg',
                     'inputIP3DSourceName'              : 'IP3DNeg',
                     'inputJFSourceName'                : 'JetFitterFlip',
                     'inputJFProbSourceName'            : 'JetFitterCombNNIP3DNeg',
                     'trainingConfig'                   : BTaggingFlags.MV2mTrainingConfig,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MV2Tag
    return Analysis__MV2Tag(**options)
