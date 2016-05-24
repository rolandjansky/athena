# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MV2cl100FlipTag
# Author: Wouter van den Wollenberg (2013-2014) Jonathan Shlomi (2016)
from BTagging.BTaggingFlags import BTaggingFlags

metaMV2cl100FlipTag = { 'IsATagger'          : True,
                       'xAODBaseName'       : 'MV2cl100Flip',
                       'DependsOn'          : ['AtlasExtrapolator',
                                               'BTagTrackToVertexTool',
                                               'BTagCalibrationBrokerTool',
                                               'IP2DNegTag',
                                               'IP3DNegTag',
#                                               'JetFitterTagCOMBNNIP3DNeg',
                                               'NewJetFitterVxFinderFlip',
                                               'SV0Tag',
                                               'SV1FlipTag'],
                       'CalibrationFolders' : ['MV2cl100',],
                       'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                       'ToolCollection'     : 'MV2cl100FlipTag' }

def toolMV2cl100FlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MV2cl100FlipTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerName                          default: "MV2cl100Flip"
    taggerNameBase                      default: "MV2cl100"
    forceMV2CalibrationAlias            default: BTaggingFlags.ForceMV2CalibrationAlias
    MV2CalibAlias                       default: BTaggingFlags.MV2CalibAlias
    inputSV0SourceName                  default: "SV0"
    inputSV1SourceName                  default: "SV1Flip"
    inputIP2DSourceName                 default: "IP2DNeg"
    inputIP3DSourceName                 default: "IP3DNeg"
    inputJFSourceName                   default: "JetFitterFlip"
    inputJFProbSourceName               default: "JetFitterCombNNIP3DNeg"
    trainingConfig                      default: BTaggingFlags.MV2cTrainingConfig

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'taggerName'                       : 'MV2cl100Flip',
                     'taggerNameBase'                   : 'MV2cl100',
                     'forceMV2CalibrationAlias'         : BTaggingFlags.ForceMV2CalibrationAlias,
                     'MV2CalibAlias'                    : BTaggingFlags.MV2CalibAlias,
                     'inputSV0SourceName'               : 'SV0',
                     'inputSV1SourceName'               : 'SV1Flip',
                     'inputIP2DSourceName'              : 'IP2DNeg',
                     'inputIP3DSourceName'              : 'IP3DNeg',
                     'inputJFSourceName'                : 'JetFitterFlip',
                     'inputJFProbSourceName'            : 'JetFitterCombNNIP3DNeg',
                     'trainingConfig'                   : BTaggingFlags.MV2cTrainingConfig,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MV2Tag
    return Analysis__MV2Tag(**options)
