# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for DL1Tag
# Author: Marie Lanfermann (September 2015)
from BTagging.BTaggingFlags import BTaggingFlags

metaDL1Tag = { 'IsATagger'          : True,
                  'xAODBaseName'       : 'DL1',
                  'DependsOn'          : ['AtlasExtrapolator',
                                          'BTagTrackToVertexTool',
                                          'BTagCalibrationBrokerTool',
                                          'IP2DTag',
                                          'IP3DTag',
                                          'NewJetFitterVxFinder',
                                          'SV0Tag',
                                          'SV1Tag'],
                  'CalibrationFolders' : ['DL1',],
                  'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                  'ToolCollection'     : 'DL1Tag' }

def toolDL1Tag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a DL1Tag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerName                          default: "DL1"
    taggerNameBase                      default: "DL1"
    forceDL1CalibrationAlias            default: BTaggingFlags.ForceDL1CalibrationAlias
    DL1CalibAlias                       default: BTaggingFlags.DL1CalibAlias
    inputSV0SourceName                  default: "SV0"
    inputSV1SourceName                  default: "SV1"
    inputIP2DSourceName                 default: "IP2D"
    inputIP3DSourceName                 default: "IP3D"
    inputJFSourceName                   default: "JetFitter"
    trainingConfig                      default: BTaggingFlags.DL1cTrainingConfig

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'inputSV0SourceName'               : 'SV0',
                     'inputSV1SourceName'               : 'SV1',
                     'inputIP2DSourceName'              : 'IP2D',
                     'inputIP3DSourceName'              : 'IP3D',
                     'inputJFSourceName'                : 'JetFitter',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__DL1Tag
    return Analysis__DL1Tag(**options)
