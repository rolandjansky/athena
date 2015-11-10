# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for DL1FlipTag
# Author: Marie Lanfermann (September 2015)
from BTagging.BTaggingFlags import BTaggingFlags

metaDL1FlipTag = { 'IsATagger'          : True,
                      'xAODBaseName'       : 'DL1Flip',
                      'DependsOn'          : ['AtlasExtrapolator',
                                              'BTagTrackToVertexTool'],
                      'CalibrationFolders' : ['DL1',],
                      'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                      'ToolCollection'     : 'DL1FlipTag' }

def toolDL1FlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a DL1FlipTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__DL1Tag
    return Analysis__DL1Tag(**options)
