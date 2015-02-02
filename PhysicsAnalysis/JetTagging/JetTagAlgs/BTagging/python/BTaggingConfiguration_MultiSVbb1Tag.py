# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MultiSVbb1 (adapted from the Royer's original code)
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaMultiSVbb1Tag = { 'IsATagger'         : True,
                      'xAODBaseName'      : 'MSV',
                      'DependsOn'         : ['AtlasExtrapolator',
                                             'BTagTrackToVertexTool',
                                             'BTagCalibrationBrokerTool',
                                             'InDetVKalMultiVxInJetTool',],
                      'CalibrationFolders' : ['MultiSVbb1',],
                      'PassByPointer'     : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                      'JetCollectionList' : 'jetCollectionList',
                      'DefaultTracks'     : 'BTagTrackToJetAssociatorBB',
                      'ToolCollection'    : 'MultiSVbb1Tag' }

def toolMultiSVbb1Tag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MultiSVTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerNameBase                      default: "MultiSVbb1"
    SecVxFinderName                     default: "MSV"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'taggerNameBase'                   : 'MultiSVbb1',
                     'SecVxFinderName'                  : 'MSV' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MultiSVTag
    return Analysis__MultiSVTag(**options)

