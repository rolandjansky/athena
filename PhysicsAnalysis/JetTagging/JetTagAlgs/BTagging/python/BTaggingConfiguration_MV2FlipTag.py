# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MV2FlipTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaMV2FlipTag = { 'IsATagger'          : True,
                   'DependsOn'          : ['AtlasExtrapolator',
                                           'BTagTrackToVertexTool',
                                           'BTagCalibrationBrokerTool',
                                           'IP3DNegTag',
                                           'JetFitterTagCOMBNNIP3DNeg',
                                           'SV1FlipTag'],
                   'CalibrationFolders' : ['MV2',],
                   'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                   'ToolCollection'     : 'MV2FlipTag' }

def toolMV2FlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MV2FlipTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    inputIP3DWeightName                 default: "IP3DNeg"
    inputSV1WeightName                  default: "SV1Flip"
    inputJetFitterWeightName            default: "JetFitterCOMBNNIP3DNeg"
    taggerName                          default: "MV2FlipTag"
    taggerNameBase                      default: "MV2Tag"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'inputIP3DWeightName'              : 'IP3DNeg',
                     'inputSV1WeightName'               : 'SV1Flip',
                     'inputJetFitterWeightName'         : 'JetFitterCOMBNNIP3DNeg',
                     'taggerName'                       : 'MV2FlipTag',
                     'taggerNameBase'                   : 'MV2Tag' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MV1Tag
    return Analysis__MV1Tag(**options)
