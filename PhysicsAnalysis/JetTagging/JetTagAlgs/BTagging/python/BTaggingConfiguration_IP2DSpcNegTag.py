# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for IP2DSpcNegTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaIP2DSpcNegTag = { 'IsATagger'         : True,
                      'DependsOn'         : ['AtlasExtrapolator',
                                             'BTagTrackToVertexTool',
                                             'InDetVKalVxInJetTool',
                                             'BTagFullLinearizedTrackFactory',
                                             'BTagTrackToVertexIPEstimator',
                                             'IP2DSpcNegNewLikelihoodTool',
                                             'IP2DSpcNegTrackSelector',
                                             'SVForIPTool_IP2DSpcNeg',
                                             'IP2DSpcNegBasicTrackGradeFactory',
                                             'IP2DSpcNegDetailedTrackGradeFactory'],
                      'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP2DSpcNeg',
                                             'trackSelectorTool'          : 'IP2DSpcNegTrackSelector',
                                             'trackGradeFactory'          : 'IP2DSpcNegDetailedTrackGradeFactory',
                                             'trackToVertexTool'          : 'BTagTrackToVertexTool',
                                             'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                             'LikelihoodTool'             : 'IP2DSpcNegNewLikelihoodTool'},
                      'PassByName'        : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                                             'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                      'PassTracksAs'      : 'trackAssociation',
                      'JetCollectionList' : 'jetCollectionList',
                      'JetWithInfoPlus'   : 'jetWithInfoPlus',
                      'ToolCollection'    : 'IP2DSpcNegTag' }

def toolIP2DSpcNegTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DSpcNegTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    impactParameterView                 default: "2D"
    trackGradePartitions                default: [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
    RejectBadTracks                     default: False
    originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
    jetCollectionList                   default: BTaggingFlags.Jets
    jetWithInfoPlus                     default: BTaggingFlags.JetsWithInfoPlus
    SecVxFinderNameForV0Removal         default: "InDetVKalVxInJetTool"
    SecVxFinderNameForIPSign            default: "InDetVKalVxInJetTool"
    unbiasIPEstimation                  default: False (switch to true (better!) when creating new PDFs)
    writeInfoPlus                       default: False
    SignWithSvx                         default: True
    flipIPSign                          default: True
    usePosIP                            default: True
    useNegIP                            default: False
    trackAssociation                    default: "Tracks"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'impactParameterView'              : '2D',
                     'trackGradePartitions'             : [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                     'RejectBadTracks'                  : True,
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'jetWithInfoPlus'                  : BTaggingFlags.JetsWithInfoPlus,
                     'SecVxFinderNameForV0Removal'      : "InDetVKalVxInJetTool",
                     'SecVxFinderNameForIPSign'         : "InDetVKalVxInJetTool",
                     'unbiasIPEstimation'               : False,
                     'writeInfoPlus'                    : False,
                     'SignWithSvx'                      : True,
                     'flipIPSign'                       : True,
                     'usePosIP'                         : True,
                     'useNegIP'                         : False,
                     'trackAssociation'                 : "Tracks" }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    return Analysis__IPTag(**options)

#------------------------------------------------------------------------

metaIP2DSpcNegDetailedTrackGradeFactory = { 'ToolCollection' : 'IP2DSpcNegTag' }

def toolIP2DSpcNegDetailedTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DSpcNegDetailedTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    TrackSummaryTool                    default: None
    useSharedHitInfo                    default: True
    useDetailSharedHitInfo              default: True
    hitBLayerGrade                      default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'TrackSummaryTool'       : None,
                     'useSharedHitInfo'       : True,
                     'useDetailSharedHitInfo' : True,
                     'hitBLayerGrade'         : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
    return Analysis__DetailedTrackGradeFactory(**options)

#------------------------------------------------------------------------

metaIP2DSpcNegBasicTrackGradeFactory = { 'ToolCollection' : 'IP2DSpcNegTag' }

def toolIP2DSpcNegBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DSpcNegBasicTrackGradeFactory tool and returns it.

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

metaSVForIPTool_IP2DSpcNeg = { 'ToolCollection' : 'IP2DSpcNegTag' }

def toolSVForIPTool_IP2DSpcNeg(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_IP2DSpcNeg tool and returns it.

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

metaIP2DSpcNegTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                                'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                                'ToolCollection' : 'IP2DSpcNegTag' }

def toolIP2DSpcNegTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DSpcNegTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useBLayerHitPrediction              default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'useBLayerHitPrediction' : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
    return Analysis__TrackSelector(**options)

#------------------------------------------------------------------------

metaIP2DSpcNegNewLikelihoodTool = { 'CalibrationFolders' : ['IP2D',],
                                    'DependsOn'          : ['BTagCalibrationBrokerTool',],
                                    'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                                    'ToolCollection'     : 'IP2DSpcNegTag' }

def toolIP2DSpcNegNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DSpcNegNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "IP2D"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'taggerName'                          : 'IP2D' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
