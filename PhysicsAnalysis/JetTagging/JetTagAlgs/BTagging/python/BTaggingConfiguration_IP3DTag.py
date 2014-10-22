# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for IP3DTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaIP3DTag = { 'IsATagger'         : True,
                'DependsOn'         : ['AtlasExtrapolator',
                                       'BTagTrackToVertexTool',
                                       'InDetVKalVxInJetTool',
                                       'BTagFullLinearizedTrackFactory',
                                       'BTagTrackToVertexIPEstimator',
                                       'IP3DNewLikelihoodTool',
                                       'IP3DTrackSelector',
                                       'SVForIPTool_IP3D',
                                       'IP3DBasicTrackGradeFactory',
                                       'IP3DDetailedTrackGradeFactory'],
                'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP3D',
                                       'trackSelectorTool'          : 'IP3DTrackSelector',
                                       'trackGradeFactory'          : 'IP3DDetailedTrackGradeFactory',
#                                       'trackToVertexTool'          : 'BTagTrackToVertexTool',
                                       'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                       'LikelihoodTool'             : 'IP3DNewLikelihoodTool'},
#                'PassByName'        : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
#                                       'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                'PassTracksAs'      : 'trackAssociationName',
                'JetCollectionList' : 'jetCollectionList',
#                'JetWithInfoPlus'   : 'jetWithInfoPlus',
                'ToolCollection'    : 'IP3DTag' }

def toolIP3DTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    impactParameterView                 default: "3D"
    trackGradePartitions                default: [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
    RejectBadTracks                     default: False
    originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
    jetCollectionList                   default: BTaggingFlags.Jets
    unbiasIPEstimation                  default: False (switch to true (better!) when creating new PDFs)
    xAODBaseName                        default: "IP3D"
    UseCHypo                            default: True
    SecVxFinderName                     default: "SV1"

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
                     'RejectBadTracks'                  : False,
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
#                     'jetWithInfoPlus'                  : BTaggingFlags.JetsWithInfoPlus,
#                     'SecVxFinderNameForV0Removal'      : "InDetVKalVxInJetTool",
#                     'SecVxFinderNameForIPSign'         : "InDetVKalVxInJetTool",
                     'unbiasIPEstimation'               : False,
                     #'trackAssociation'                 : "Tracks" }
                     'xAODBaseName'                     : 'IP3D',
                     'UseCHypo'                         : True,
                     'SecVxFinderName'                  : 'SV1',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    return Analysis__IPTag(**options)

#------------------------------------------------------------------

metaIP3DDetailedTrackGradeFactory = { 'ToolCollection' : 'IP3DTag' }

def toolIP3DDetailedTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DDetailedTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useSharedHitInfo                    default: True
    useDetailSharedHitInfo              default: True
    hitBLayerGrade                      default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'useSharedHitInfo'       : True,
                     'useDetailSharedHitInfo' : True,
                     'hitBLayerGrade'         : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
    return Analysis__DetailedTrackGradeFactory(**options)

#------------------------------------------------------------------

metaIP3DBasicTrackGradeFactory = { 'ToolCollection' : 'IP3DTag' }

def toolIP3DBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DBasicTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useSharedHitInfo                    default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'      : BTaggingFlags.OutputLevel,
                     'useSharedHitInfo' : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
    return Analysis__BasicTrackGradeFactory(**options)

#------------------------------------------------------------------

metaSVForIPTool_IP3D = { 'ToolCollection' : 'IP3DTag' }

def toolSVForIPTool_IP3D(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_IP3D tool and returns it.

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

#------------------------------------------------------------------

metaIP3DTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                          'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                          'ToolCollection' : 'IP3DTag' }

def toolIP3DTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTrackSelector tool and returns it.

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

#------------------------------------------------------------------

metaIP3DNewLikelihoodTool = { 'CalibrationFolders' : ['IP3D',],
                              'DependsOn'          : ['BTagCalibrationBrokerTool',],
                              'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                              'ToolCollection'     : 'IP3DTag' }

def toolIP3DNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DNewLikelihoodTool tool and returns it.

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
