# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for SoftElectronTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaSoftElectronTag = { 'IsATagger'               : True,
                        'NeedsElectronAssociator' : True,
                        'DependsOn'               : ['AtlasExtrapolator',
                                                     'BTagTrackToVertexTool',
                                                     'BTagFullLinearizedTrackFactory',
                                                     'BTagTrackToVertexIPEstimator',
                                                     'BTagCalibrationBrokerTool',
                                                     'SoftElectronTagTrackSelector',
                                                     'SoftElectronTagNewLikelihoodTool'],
                        'PassByPointer'           : {'trackSelectorTool'        : 'SoftElectronTagTrackSelector',
                                                     'LikelihoodTool'           : 'SoftElectronTagNewLikelihoodTool',
                                                     'TrackToVertexIPEstimator' : 'BTagTrackToVertexIPEstimator',
                                                     'calibrationTool'          : 'BTagCalibrationBrokerTool',
                                                     'trackToVertexTool'        : 'BTagTrackToVertexTool'},
                        'CalibrationFolders'      : ['SoftEl',],
                        'JetCollectionList'       : 'jetCollectionList',
                        'ToolCollection'          : 'SoftElectronTag' }

def toolSoftElectronTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SoftElectronTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    originalElCollectionName            default: "ElectronAODCollection"
    writeInfoPlus                       default: True
    jetCollectionList                   default: BTaggingFlags.Jets

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'originalElCollectionName'         : 'ElectronAODCollection',
                     'writeInfoPlus'                    : True,
                     'jetCollectionList'                : BTaggingFlags.Jets }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SoftElectronTag
    return Analysis__SoftElectronTag(**options)

#--------------------------------------------------------------------------

metaSoftElectronTagTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                                     'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                                     'ToolCollection' : 'SoftElectronTag' }

def toolSoftElectronTagTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SoftElectronTagTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
    return Analysis__TrackSelector(**options)

#--------------------------------------------------------------------------

metaSoftElectronTagNewLikelihoodTool = { 'CalibrationFolders' : ['SoftEl',],
                                         'DependsOn'          : ['BTagCalibrationBrokerTool',],
                                         'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                                         'ToolCollection'     : 'SoftElectronTag' }

def toolSoftElectronTagNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SoftElectronTagNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "SoftEl"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'taggerName'                          : 'SoftEl' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
