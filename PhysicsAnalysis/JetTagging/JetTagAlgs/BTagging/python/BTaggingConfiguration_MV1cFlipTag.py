# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MV1cTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaMV1cFlipTag = { 'IsATagger'          : True,
                    'xAODBaseName'       : 'MV1cFlip',
                    'DependsOn'          : ['AtlasExtrapolator',
                                            'BTagTrackToVertexTool',
                                            'BTagCalibrationBrokerTool',
                                            'IP3DNegTag',
                                            'JetFitterTagCOMBNNIP3DNeg',
                                            'SV1FlipTag'],
                    'CalibrationFolders' : ['MV1c',],
                    'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                    'ToolCollection'     : 'MV1cFlipTag' }

def toolMV1cFlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MV1Tag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerName                          default: "MV1cFlip"
    taggerNameBase                      default: "MV1c"
    inputIP3DWeightName                 default: "IP3DNeg"
    inputSV1WeightName                  default: "SV1Flip"
    inputJetFitterWeightName            default: "JetFitterCombNNIP3DNeg"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'taggerName'                       : 'MV1cFlip',
                     'taggerNameBase'                   : 'MV1c',
                     'inputIP3DWeightName'              : 'IP3DNeg',
                     'inputSV1WeightName'               : 'SV1Flip',
                     'inputJetFitterWeightName'         : 'JetFitterCombNNIP3DNeg',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MV1cTag
    return Analysis__MV1cTag(**options)
