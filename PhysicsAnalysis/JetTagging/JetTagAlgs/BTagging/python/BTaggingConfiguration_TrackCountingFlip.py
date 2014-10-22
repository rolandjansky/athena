# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for TrackCountingFlip
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaTrackCountingFlip = { 'IsATagger'       : True,
                          'DependsOn'       : ['AtlasExtrapolator',
                                               'BTagTrackToVertexTool',
                                               'InDetVKalVxInJetTool',
                                               'BTagFullLinearizedTrackFactory',
                                               'BTagTrackToVertexIPEstimator',
                                               'TrackCountingFlipTrackSelector',
                                               'SVForIPTool_TrackCountingFlip',
                                               'TrackCountingFlipBasicTrackGradeFactory'],
                          'PassByPointer'   : {'SVForIPTool'              : 'SVForIPTool_TrackCountingFlip',
                                               'trackSelectorTool'        : 'TrackCountingFlipTrackSelector',
                                               'trackGradeFactory'        : 'TrackCountingFlipBasicTrackGradeFactory',
                                               'trackToVertexTool'        : 'BTagTrackToVertexTool',
                                               'TrackToVertexIPEstimator' : 'BTagTrackToVertexIPEstimator'},
                          'PassByName'      : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                                               'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                          'PassTracksAs'    : 'trackAssociation',
                          'ToolCollection'  : 'TrackCountingFlip' }

def toolTrackCountingFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a TrackCountingFlip tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    trackGradePartitions                default: [ "Good", "Shared" ]
    RejectBadTracks                     default: False
    SignWithSvx                         default: False
    SecVxFinderNameForV0Removal         default: "InDetVKalVxInJetTool"
    SecVxFinderNameForIPSign            default: "InDetVKalVxInJetTool"
    unbiasIPEstimation                  default: False
    flipIPSign                          default: True
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
                     'flipIPSign'                       : True,
                     'trackAssociation'                 : "Tracks" }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackCounting
    return Analysis__TrackCounting(**options)

#----------------------------------------------------------------------------

metaTrackCountingFlipBasicTrackGradeFactory = { 'ToolCollection' : 'TrackCountingFlip' }

def toolTrackCountingFlipBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a TrackCountingFlipBasicTrackGradeFactory tool and returns it.

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

#----------------------------------------------------------------------------

metaSVForIPTool_TrackCountingFlip = { 'ToolCollection' : 'TrackCountingFlip' }

def toolSVForIPTool_TrackCountingFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_TrackCountingFlip tool and returns it.

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

#----------------------------------------------------------------------------

metaTrackCountingFlipTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                                       'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                                       'ToolCollection' : 'TrackCountingFlip' }

def toolTrackCountingFlipTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a TrackCountingFlipTrackSelector tool and returns it.

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
