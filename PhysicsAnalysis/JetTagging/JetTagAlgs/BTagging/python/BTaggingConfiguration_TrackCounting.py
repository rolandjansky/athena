# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for TrackCounting
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaTrackCounting = { 'IsATagger'       : True,
                      'DependsOn'       : ['AtlasExtrapolator',
                                           'BTagTrackToVertexTool',
                                           'InDetVKalVxInJetTool',
                                           'BTagFullLinearizedTrackFactory',
                                           'BTagTrackToVertexIPEstimator',
                                           'TrackCountingBasicTrackGradeFactory',
                                           'SVForIPTool_TrackCounting',
                                           'TrackCountingTrackSelector'],
                      'PassByPointer'   : {'SVForIPTool'              : 'SVForIPTool_TrackCounting',
                                           'trackSelectorTool'        : 'TrackCountingTrackSelector',
                                           'trackGradeFactory'        : 'TrackCountingBasicTrackGradeFactory',
                                           'trackToVertexTool'        : 'BTagTrackToVertexTool',
                                           'TrackToVertexIPEstimator' : 'BTagTrackToVertexIPEstimator'},
                      'PassByName'      : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                                           'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                      'PassTracksAs'    : 'trackAssociation',
                      'ToolCollection'  : 'TrackCounting' }

def toolTrackCounting(name, useBTagFlagsDefaults = True, **options):
    """Sets up a TrackCounting tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    trackGradePartitions                default: [ "Good", "Shared" ]
    RejectBadTracks                     default: False
    SignWithSvx                         default: False
    SecVxFinderNameForV0Removal         default: "InDetVKalVxInJetTool"
    SecVxFinderNameForIPSign            default: "InDetVKalVxInJetTool"
    unbiasIPEstimation                  default: False
    trackAssociation                    default: "Tracks"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'trackGradePartitions'             : [ "Good", "Shared" ],
                     'RejectBadTracks'                  : False,
                     'SignWithSvx'                      : False,
                     'SecVxFinderNameForV0Removal'      : 'InDetVKalVxInJetTool',
                     'SecVxFinderNameForIPSign'         : 'InDetVKalVxInJetTool',
                     'unbiasIPEstimation'               : False,
                     'trackAssociation'                 : "Tracks" }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackCounting
    return Analysis__TrackCounting(**options)

#------------------------------------------------------------------------

metaTrackCountingBasicTrackGradeFactory = { 'ToolCollection' : 'TrackCounting' }

def toolTrackCountingBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a TrackCountingBasicTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    TrackSummaryTool                    default: None
    useSharedHitInfo                    default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'      : BTaggingFlags.OutputLevel,
                     'TrackSummaryTool' : None,
                     'useSharedHitInfo' : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
    return Analysis__BasicTrackGradeFactory(**options)

#------------------------------------------------------------------------

metaSVForIPTool_TrackCounting = { 'ToolCollection' : 'TrackCounting' }

def toolSVForIPTool_TrackCounting(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_TrackCounting tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel' : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
    return Analysis__SVForIPTool(**options)

#------------------------------------------------------------------------

metaTrackCountingTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                                   'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                                   'ToolCollection' : 'TrackCounting' }

def toolTrackCountingTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a TrackCountingTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
    return Analysis__TrackSelector(**options)
