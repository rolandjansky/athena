# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# FLS: copied the original IP3DTag.py and rename every occurrence of IP3D -> IP3DTrighHybrid

# Configuration functions for IP3DTrigHybridTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
from IOVDbSvc.CondDB import conddb
btagrun1 = False
if conddb.dbdata == 'COMP200':
    btagrun1 = True
elif conddb.isMC:
    btagrun1 = (geoFlags.Run() == "RUN1" or (geoFlags.Run()
                                             == "UNDEFINED" and geoFlags.isIBL() == False))

metaIP3DTrigHybridTag = {'IsATagger': True,
                         'xAODBaseName': 'IP3DTrigHybrid',
                         'DependsOn': ['AtlasExtrapolator',
                                       'BTagTrackToVertexTool',
                                       'InDetVKalVxInJetTool',
                                       'BTagFullLinearizedTrackFactory',
                                       'BTagTrackToVertexIPEstimator',
                                       'IP3DTrigHybridNewLikelihoodTool',
                                       'IP3DTrigHybridTrackSelector',
                                       'TrigHybridInDetTrackSelector',
                                       'TrigHybridSpecialTrackAssociator',
                                       'SVForIPTool_IP3DTrigHybrid',
                                       'IP3DTrigHybridBasicTrackGradeFactory',
                                       'IP3DTrigHybridDetailedTrackGradeFactory'],
                         'PassByPointer': {'SVForIPTool': 'SVForIPTool_IP3DTrigHybrid',
                                           'trackSelectorTool': 'IP3DTrigHybridTrackSelector',
                                           'InDetTrackSelectionTool': 'TrigHybridInDetTrackSelector',
                                           'TrackVertexAssociationTool': 'TrigHybridSpecialTrackAssociator',
                                           'trackGradeFactory': 'IP3DTrigHybridDetailedTrackGradeFactory',
                                           #                                       'trackToVertexTool'          : 'BTagTrackToVertexTool',
                                           'TrackToVertexIPEstimator': 'BTagTrackToVertexIPEstimator',
                                           'LikelihoodTool': 'IP3DTrigHybridNewLikelihoodTool'},
                         #                'PassByName'        : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                         #                                       'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                         'PassTracksAs': 'trackAssociationName',
                         'JetCollectionList': 'jetCollectionList',
                         'ToolCollection': 'IP3DTrigHybridTag'}


def toolIP3DTrigHybridTag(name, useBTagFlagsDefaults=True, **options):
    """Sets up a IP3DTrigHybridTag tool and returns it.

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
    UseCHypo                            default: True
    SecVxFinderName                     default: "SV1"
    jetPtMinRef                         default: BTaggingFlags.JetPtMinRef

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        grades = ["0HitIn0HitNInExp2", "0HitIn0HitNInExpIn", "0HitIn0HitNInExpNIn", "0HitIn0HitNIn",
                  "0HitInExp", "0HitIn",
                  "0HitNInExp", "0HitNIn",
                  "InANDNInShared", "PixShared", "SctShared",
                  "InANDNInSplit", "PixSplit",
                  "Good"]
        if btagrun1:
            grades = ["Good", "BlaShared",
                      "PixShared", "SctShared", "0HitBLayer"]

        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'Runmodus': BTaggingFlags.Runmodus,
                    'referenceType': BTaggingFlags.ReferenceType,
                    'jetPtMinRef': BTaggingFlags.JetPtMinRef,
                    'impactParameterView': '3D',
                    'trackGradePartitions': grades,
                    'RejectBadTracks': True,
                    'originalTPCollectionName': BTaggingFlags.TrackParticleCollectionName,
                    'jetCollectionList': BTaggingFlags.Jets,
                    #                     'SecVxFinderNameForV0Removal'      : "InDetVKalVxInJetTool",
                    #                     'SecVxFinderNameForIPSign'         : "InDetVKalVxInJetTool",
                    'unbiasIPEstimation': False,
                    #'trackAssociation'                 : "Tracks" }
                    'UseCHypo': True,
                    'SecVxFinderName': 'SV1',
                    'storeTrackParameters': True,
                    'storeTrackParticles': True,
                    'storeIpValues': True
                    }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    return Analysis__IPTag(**options)


#------------------------------------------------------------------

metaIP3DTrigHybridDetailedTrackGradeFactory = {
    'ToolCollection': 'IP3DTrigHybridTag'}


def toolIP3DTrigHybridDetailedTrackGradeFactory(name, useBTagFlagsDefaults=True, **options):
    """Sets up a IP3DTrigHybridDetailedTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useSharedHitInfo                    default: True
    useDetailSharedHitInfo              default: True
    hitBLayerGrade                      default: True
    useRun2TrackGrading                 default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'useSharedHitInfo': True,
                    'useDetailSharedHitInfo': True,
                    'useRun2TrackGrading': (btagrun1 == False),
                    'useInnerLayers0HitInfo': (btagrun1 == False),
                    'useDetailSplitHitInfo': (btagrun1 == False),
                    'hitBLayerGrade': True}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
    return Analysis__DetailedTrackGradeFactory(**options)

#------------------------------------------------------------------


metaIP3DTrigHybridBasicTrackGradeFactory = {
    'ToolCollection': 'IP3DTrigHybridTag'}


def toolIP3DTrigHybridBasicTrackGradeFactory(name, useBTagFlagsDefaults=True, **options):
    """Sets up a IP3DTrigHybridBasicTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useSharedHitInfo                    default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'useSharedHitInfo': True}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
    return Analysis__BasicTrackGradeFactory(**options)

#------------------------------------------------------------------


metaSVForIPTool_IP3DTrigHybrid = {'ToolCollection': 'IP3DTrigHybridTag'}


def toolSVForIPTool_IP3DTrigHybrid(name, useBTagFlagsDefaults=True, **options):
    """Sets up a SVForIPTool_IP3DTrigHybrid tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool
    return Analysis__SVForIPTool(**options)

#------------------------------------------------------------------


metaIP3DTrigHybridTrackSelector = {'DependsOn': ['BTagTrackToVertexTool', ],
                                   'PassByPointer': {'trackToVertexTool': 'BTagTrackToVertexTool'},
                                   'ToolCollection': 'IP3DTrigHybridTag'}


def toolIP3DTrigHybridTrackSelector(name, useBTagFlagsDefaults=True, **options):
    """Sets up a IP3DTrigHybridTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useBLayerHitPrediction              default: True
    usepTDepTrackSel                    default: False
    nHitBLayer                          default: 0

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'useBLayerHitPrediction': True,
                    'nHitBLayer': 0,
                    'usepTDepTrackSel': False}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
    return Analysis__TrackSelector(**options)

#------------------------------------------------------------------


metaTrigHybridInDetTrackSelector = {'ToolCollection': 'IP3DTrigHybridTag'}


def toolTrigHybridInDetTrackSelector(name, useBTagFlagsDefaults=True, **options):
    """Sets up a InDetTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'CutLevel': "Loose",
                    'maxZ0SinTheta': 3}
#                     'maxD0'                  : 2 }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name

    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    return InDet__InDetTrackSelectionTool(**options)

#------------------------------------------------------------------


metaTrigHybridSpecialTrackAssociator = {'ToolCollection': 'IP3DTrigHybridTag'}


def toolTrigHybridSpecialTrackAssociator(name, useBTagFlagsDefaults=True, **options):
    """Sets up a InDetTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:
    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'dzSinTheta_cut': 3,
                    'doPV': True}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name

    from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TightTrackVertexAssociationTool
    return CP__TightTrackVertexAssociationTool(**options)

#------------------------------------------------------------------


metaIP3DTrigHybridNewLikelihoodTool = {'CalibrationFolders': ['IP3DTrigHybrid', ],
                                       'DependsOn': ['BTagCalibrationBrokerTool', ],
                                       'PassByPointer': {'calibrationTool': 'BTagCalibrationBrokerTool'},
                                       'ToolCollection': 'IP3DTrigHybridTag'}


def toolIP3DTrigHybridNewLikelihoodTool(name, useBTagFlagsDefaults=True, **options):
    """Sets up a IP3DTrigHybridNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "IP3DTrigHybrid"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'taggerName': 'IP3DTrigHybrid'}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
