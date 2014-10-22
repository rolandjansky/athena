# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for JetProbTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaJetProbTag = { 'IsATagger'          : True,
                   'DependsOn'          : ['AtlasExtrapolator',
                                           'BTagTrackToVertexTool',
                                           'InDetVKalVxInJetTool',
                                           'BTagFullLinearizedTrackFactory',
                                           'BTagTrackToVertexIPEstimator',
                                           'JetProbBasicTrackGradeFactory',
                                           'SVForIPTool_JetProb',
                                           'JetProbTrackSelector',
                                           'BTagCalibrationBrokerTool'],
                   'PassByPointer'      : {'SVForIPTool'              : 'SVForIPTool_JetProb',
                                           'trackSelectorTool'        : 'JetProbTrackSelector',
                                           'trackGradeFactory'        : 'JetProbBasicTrackGradeFactory',
                                           'trackToVertexTool'        : 'BTagTrackToVertexTool',
                                           'TrackToVertexIPEstimator' : 'BTagTrackToVertexIPEstimator',
                                           'calibrationTool'          : 'BTagCalibrationBrokerTool'},
                   'PassByName'         : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                                           'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                   'PassTracksAs'       : 'trackAssociation',
                   'CalibrationFolders' : ['JetProb',],
                   'JetCollectionList'  : 'jetCollectionList',
                   'JetWithInfoPlus'    : 'jetWithInfoPlus',
                   'ToolCollection'     : 'JetProbTag' }

def toolJetProbTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetProbTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    impactParameterView                 default: "2D"
    trackGradePartitions                default: [ "Good", "Shared" ]
    UseNegIP                            default: False
    RejectBadTracks                     default: False
    SecVxFinderNameForV0Removal         default: "InDetVKalVxInJetTool"
    SecVxFinderNameForIPSign            default: "InDetVKalVxInJetTool"
    originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
    jetCollectionList                   default: BTaggingFlags.Jets
    jetWithInfoPlus                     default: BTaggingFlags.JetsWithInfoPlus
    unbiasIPEstimation                  default: False (switch to true (better!) when creating new PDFs)
    InputType                           default: globalflags.DataSource()
    trackAssociation                    default: "Tracks"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        from AthenaCommon.GlobalFlags import globalflags
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'impactParameterView'              : '2D',
                     'trackGradePartitions'             : [ "Good", "Shared" ],
                     'UseNegIP'                         : False,
                     'RejectBadTracks'                  : False,
                     'SecVxFinderNameForV0Removal'      : 'InDetVKalVxInJetTool',
                     'SecVxFinderNameForIPSign'         : 'InDetVKalVxInJetTool',
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'jetWithInfoPlus'                  : BTaggingFlags.JetsWithInfoPlus,
                     'unbiasIPEstimation'               : False,
                     'InputType'                        : globalflags.DataSource(),
                     'trackAssociation'                 : "Tracks" }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetProbTag
    return Analysis__JetProbTag(**options)

#---------------------------------------------------------------------

metaJetProbBasicTrackGradeFactory = { 'ToolCollection' : 'JetProbTag' }

def toolJetProbBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetProbBasicTrackGradeFactory tool and returns it.

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

#---------------------------------------------------------------------

metaSVForIPTool_JetProb = { 'ToolCollection' : 'JetProbTag' }

def toolSVForIPTool_JetProb(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_JetProb tool and returns it.

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

#---------------------------------------------------------------------

metaJetProbTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                             'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool' },
                             'ToolCollection' : 'JetProbTag' }

def toolJetProbTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetProbTrackSelector tool and returns it.

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
