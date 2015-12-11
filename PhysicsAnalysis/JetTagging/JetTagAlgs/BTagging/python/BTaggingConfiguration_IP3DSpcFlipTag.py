# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for IP3DSpcFlipTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaIP3DSpcFlipTag = { 'IsATagger'         : True,
                       'DependsOn'         : ['AtlasExtrapolator',
                                              'BTagTrackToVertexTool',
                                              'InDetVKalVxInJetTool',
                                              'BTagFullLinearizedTrackFactory',
                                              'BTagTrackToVertexIPEstimator',
                                              'IP3DSpcFlipNewLikelihoodTool',
                                              'IP3DSpcFlipTrackSelector',
                                              'SVForIPTool_IP3DSpcFlip',
                                              'IP3DSpcFlipBasicTrackGradeFactory',
                                              'IP3DSpcFlipDetailedTrackGradeFactory'],
                       'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP3DSpcFlip',
                                              'trackSelectorTool'          : 'IP3DSpcFlipTrackSelector',
                                              'trackGradeFactory'          : 'IP3DSpcFlipDetailedTrackGradeFactory',
                                              'trackToVertexTool'          : 'BTagTrackToVertexTool',
                                              'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                              'LikelihoodTool'             : 'IP3DSpcFlipNewLikelihoodTool'},
                       'PassByName'        : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                                              'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                       'PassTracksAs'      : 'trackAssociation',
                       'JetCollectionList' : 'jetCollectionList',
                       'JetWithInfoPlus'   : 'jetWithInfoPlus',
                       'ToolCollection'    : 'IP3DSpcFlipTag' }

def toolIP3DSpcFlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DSpcFlipTag tool and returns it.

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
                     'trackAssociation'                 : "Tracks" }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    return Analysis__IPTag(**options)

#-------------------------------------------------------------------------

metaIP3DSpcFlipDetailedTrackGradeFactory = { 'ToolCollection' : 'IP3DSpcFlipTag' }

def toolIP3DSpcFlipDetailedTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DSpcFlipDetailedTrackGradeFactory tool and returns it.

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

metaIP3DSpcFlipBasicTrackGradeFactory = { 'ToolCollection' : 'IP3DSpcFlipTag' }

def toolIP3DSpcFlipBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DSpcFlipBasicTrackGradeFactory tool and returns it.

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

metaSVForIPTool_IP3DSpcFlip = { 'ToolCollection' : 'IP3DSpcFlipTag' }

def toolSVForIPTool_IP3DSpcFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_IP3DSpcFlip tool and returns it.

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

metaIP3DSpcFlipTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                                 'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                                 'ToolCollection' : 'IP3DSpcFlipTag' }

def toolIP3DSpcFlipTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DSpcFlipTrackSelector tool and returns it.

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

metaIP3DSpcFlipNewLikelihoodTool = { 'CalibrationFolders' : ['IP3D',],
                                     'DependsOn'          : ['BTagCalibrationBrokerTool',],
                                     'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                                     'ToolCollection'     : 'IP3DSpcFlipTag' }

def toolIP3DSpcFlipNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DSpcFlipNewLikelihoodTool tool and returns it.

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
