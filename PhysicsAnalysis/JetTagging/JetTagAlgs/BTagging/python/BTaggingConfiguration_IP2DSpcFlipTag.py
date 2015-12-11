# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for IP2DSpcFlipTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaIP2DSpcFlipTag = { 'IsATagger'         : True,
                       'DependsOn'         : ['AtlasExtrapolator',
                                              'BTagTrackToVertexTool',
                                              'InDetVKalVxInJetTool',
                                              'BTagFullLinearizedTrackFactory',
                                              'BTagTrackToVertexIPEstimator',
                                              'IP2DSpcFlipNewLikelihoodTool',
                                              'IP2DSpcFlipTrackSelector',
                                              'SVForIPTool_IP2DSpcFlip',
                                              'IP2DSpcFlipBasicTrackGradeFactory',
                                              'IP2DSpcFlipDetailedTrackGradeFactory'],
                       'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP2DSpcFlip',
                                              'trackSelectorTool'          : 'IP2DSpcFlipTrackSelector',
                                              'trackGradeFactory'          : 'IP2DSpcFlipDetailedTrackGradeFactory',
                                              'trackToVertexTool'          : 'BTagTrackToVertexTool',
                                              'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                              'LikelihoodTool'             : 'IP2DSpcFlipNewLikelihoodTool'},
                       'PassByName'        : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                                              'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                       'PassTracksAs'      : 'trackAssociation',
                       'JetCollectionList' : 'jetCollectionList',
                       'JetWithInfoPlus'   : 'jetWithInfoPlus',
                       'ToolCollection'    : 'IP2DSpcFlipTag' }

def toolIP2DSpcFlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DSpcFlipTag tool and returns it.

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
                     'trackAssociation'                 : "Tracks" }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    return Analysis__IPTag(**options)

#-------------------------------------------------------------------------

metaIP2DSpcFlipDetailedTrackGradeFactory = { 'ToolCollection' : 'IP2DSpcFlipTag' }

def toolIP2DSpcFlipDetailedTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DSpcFlipDetailedTrackGradeFactory tool and returns it.

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

#-------------------------------------------------------------------------

metaIP2DSpcFlipBasicTrackGradeFactory = { 'ToolCollection' : 'IP2DSpcFlipTag' }

def toolIP2DSpcFlipBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DSpcFlipBasicTrackGradeFactory tool and returns it.

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

#-------------------------------------------------------------------------

metaSVForIPTool_IP2DSpcFlip = { 'ToolCollection' : 'IP2DSpcFlipTag' }

def toolSVForIPTool_IP2DSpcFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_IP2DSpcFlip tool and returns it.

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

#-------------------------------------------------------------------------

metaIP2DSpcFlipTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                                 'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                                 'ToolCollection' : 'IP2DSpcFlipTag' }

def toolIP2DSpcFlipTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DSpcFlipTrackSelector tool and returns it.

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

#-------------------------------------------------------------------------

metaIP2DSpcFlipNewLikelihoodTool = { 'CalibrationFolders' : ['IP2D',],
                                     'DependsOn'          : ['BTagCalibrationBrokerTool',],
                                     'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                                     'ToolCollection'     : 'IP2DSpcFlipTag' }

def toolIP2DSpcFlipNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DSpcFlipNewLikelihoodTool tool and returns it.

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
