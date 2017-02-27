# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MV2Tag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaMV2Tag = { 'IsATagger'          : True,
               'DependsOn'          : ['AtlasExtrapolator',
                                       'BTagTrackToVertexTool',
                                       'BTagCalibrationBrokerTool',
                                       'IP3DTag',
                                       'JetFitterTagCOMBNN',
                                       'SV1Tag'],
               'CalibrationFolders' : ['MV2',],
               'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
               'ToolCollection'     : 'MV2Tag' }

def toolMV2Tag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MV2Tag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerName                          default: "MV2Tag"
    taggerNameBase                      default: "MV2Tag"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'taggerName'                       : 'MV2Tag',
                     'taggerNameBase'                   : 'MV2Tag' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MV2Tag
    return Analysis__MV2Tag(**options)
