# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for IP3DSpcNegTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaIP3DSpcNegTag = { 'IsATagger'         : True,
                      'DependsOn'         : ['AtlasExtrapolator',
                                             'BTagTrackToVertexTool',
                                             'InDetVKalVxInJetTool',
                                             'BTagFullLinearizedTrackFactory',
                                             'BTagTrackToVertexIPEstimator',
                                             'IP3DSpcNegNewLikelihoodTool',
                                             'IP3DSpcNegTrackSelector',
                                             'SVForIPTool_IP3DSpcNeg',
                                             'IP3DSpcNegBasicTrackGradeFactory',
                                             'IP3DSpcNegDetailedTrackGradeFactory'],
                      'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP3DSpcNeg',
                                             'trackSelectorTool'          : 'IP3DSpcNegTrackSelector',
                                             'trackGradeFactory'          : 'IP3DSpcNegDetailedTrackGradeFactory',
                                             'trackToVertexTool'          : 'BTagTrackToVertexTool',
                                             'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                             'LikelihoodTool'             : 'IP3DSpcNegNewLikelihoodTool'},
                      'PassByName'        : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                                             'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                      'PassTracksAs'      : 'trackAssociation',
                      'JetCollectionList' : 'jetCollectionList',
                      'JetWithInfoPlus'   : 'jetWithInfoPlus',
                      'ToolCollection'    : 'IP3DSpcNegTag' }

def toolIP3DSpcNegTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DSpcNegTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    impactParameterView                 default: "3D"
    trackGradePartitions                default: [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
    RejectBadTracks                     default: False
    originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
    jetCollectionList                   default: BTaggingFlags.Jets
    jetWithInfoPlus                     default: BTaggingFlags.JetsWithInfoPlus
    SecVxFinderNameForV0Removal         default: "InDetVKalVxInJetTool"
    SecVxFinderNameForIPSign            default: "InDetVKalVxInJetTool"
    unbiasIPEstimation                  default: False (switch to true (better!) when creating new PDFs)
    SignWithSvx                         default: True
    writeInfoPlus                       default: False
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
                     'impactParameterView'              : '3D',
                     'trackGradePartitions'             : [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ],
                     'RejectBadTracks'                  : True,
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'jetWithInfoPlus'                  : BTaggingFlags.JetsWithInfoPlus,
                     'SecVxFinderNameForV0Removal'      : "InDetVKalVxInJetTool",
                     'SecVxFinderNameForIPSign'         : "InDetVKalVxInJetTool",
                     'unbiasIPEstimation'               : False,
                     'SignWithSvx'                      : True,
                     'writeInfoPlus'                    : False,
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

metaIP3DSpcNegDetailedTrackGradeFactory = { 'ToolCollection' : 'IP3DSpcNegTag' }

def toolIP3DSpcNegDetailedTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DSpcNegDetailedTrackGradeFactory tool and returns it.

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

metaIP3DSpcNegBasicTrackGradeFactory = { 'ToolCollection' : 'IP3DSpcNegTag' }

def toolIP3DSpcNegBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DSpcNegBasicTrackGradeFactory tool and returns it.

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

metaSVForIPTool_IP3DSpcNeg = { 'ToolCollection' : 'IP3DSpcNegTag' }

def toolSVForIPTool_IP3DSpcNeg(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_IP3DSpcNeg tool and returns it.

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

metaIP3DSpcNegTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                                'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                                'ToolCollection' : 'IP3DSpcNegTag' }

def toolIP3DSpcNegTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DSpcNegTrackSelector tool and returns it.

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

metaIP3DSpcNegNewLikelihoodTool = { 'CalibrationFolders' : ['IP3D',],
                                    'DependsOn'          : ['BTagCalibrationBrokerTool',],
                                    'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                                    'ToolCollection'     : 'IP3DSpcNegTag' }

def toolIP3DSpcNegNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DSpcNegNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "IP3D"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'taggerName'                          : 'IP3D' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
