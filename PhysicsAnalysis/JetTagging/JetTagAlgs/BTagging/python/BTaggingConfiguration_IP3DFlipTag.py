# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for IP3DFlipTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaIP3DFlipTag = { 'IsATagger'         : True,
                    'DependsOn'         : ['AtlasExtrapolator',
                                           'BTagTrackToVertexTool',
                                           'InDetVKalVxInJetTool',
                                           'BTagFullLinearizedTrackFactory',
                                           'BTagTrackToVertexIPEstimator',
                                           'IP3DFlipNewLikelihoodTool',
                                           'IP3DFlipTrackSelector',
                                           'SVForIPTool_IP3DFlip',
                                           'IP3DFlipBasicTrackGradeFactory',
                                           'IP3DFlipDetailedTrackGradeFactory'],
                    'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP3DFlip',
                                           'trackSelectorTool'          : 'IP3DFlipTrackSelector',
                                           'trackGradeFactory'          : 'IP3DFlipDetailedTrackGradeFactory',
                                           'trackToVertexTool'          : 'BTagTrackToVertexTool',
                                           'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                           'LikelihoodTool'             : 'IP3DFlipNewLikelihoodTool'},
                    'PassByName'        : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                                           'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                    'PassTracksAs'      : 'trackAssociation',
                    'JetCollectionList' : 'jetCollectionList',
                    'JetWithInfoPlus'   : 'jetWithInfoPlus',
                    'ToolCollection'    : 'IP3DFlipTag' }

def toolIP3DFlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DFlipTag tool and returns it.

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
    flipIPSign                          default: True
    writeInfoPlus                       default: False
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
                     'flipIPSign'                       : True,
                     'writeInfoPlus'                    : False,
                     'trackAssociation'                 : "Tracks" }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    return Analysis__IPTag(**options)

#----------------------------------------------------------------------

metaIP3DFlipDetailedTrackGradeFactory = { 'ToolCollection' : 'IP3DFlipTag' }

def toolIP3DFlipDetailedTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DFlipDetailedTrackGradeFactory tool and returns it.

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

#----------------------------------------------------------------------

metaIP3DFlipBasicTrackGradeFactory = { 'ToolCollection' : 'IP3DFlipTag' }

def toolIP3DFlipBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DFlipBasicTrackGradeFactory tool and returns it.

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

#----------------------------------------------------------------------

metaSVForIPTool_IP3DFlip = { 'ToolCollection' : 'IP3DFlipTag' }

def toolSVForIPTool_IP3DFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_IP3DFlip tool and returns it.

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

#----------------------------------------------------------------------

metaIP3DFlipTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                              'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                              'ToolCollection' : 'IP3DFlipTag' }

def toolIP3DFlipTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DFlipTrackSelector tool and returns it.

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

#----------------------------------------------------------------------

metaIP3DFlipNewLikelihoodTool = { 'CalibrationFolders' : ['IP3D',],
                                  'DependsOn'          : ['BTagCalibrationBrokerTool',],
                                  'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                                  'ToolCollection'     : 'IP3DFlipTag' }

def toolIP3DFlipNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DFlipNewLikelihoodTool tool and returns it.

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
