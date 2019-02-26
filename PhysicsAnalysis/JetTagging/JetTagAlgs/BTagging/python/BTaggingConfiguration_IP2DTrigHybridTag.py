# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# FLS: copied the original IP2DTag.py and rename every occurrence of IP2D -> IP2DTrighHybrid

# Configuration functions for IP2DTrigHybridTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
from IOVDbSvc.CondDB import conddb
btagrun1=False
if conddb.dbdata == 'COMP200':
    btagrun1=True
elif conddb.isMC:
    btagrun1 = (commonGeoFlags.Run() == "RUN1" or (commonGeoFlags.Run() == "UNDEFINED" and geoFlags.isIBL() == False))

metaIP2DTrigHybridTag = {'IsATagger': True,
                         'xAODBaseName': 'IP2DTrigHybrid',
                         'DependsOn': ['AtlasExtrapolator',
                                       'BTagTrackToVertexTool',
                                       'InDetVKalVxInJetTool',
                                       'BTagFullLinearizedTrackFactory',
                                       'BTagTrackToVertexIPEstimator',
                                       'IP2DTrigHybridDetailedTrackGradeFactory',
                                       'IP2DTrigHybridBasicTrackGradeFactory',
                                       'SVForIPTool_IP2DTrigHybrid',
                                       'IP2DTrigHybridTrackSelector',
                                       'IP2DTrigHybridNewLikelihoodTool'],
                         'PassByPointer': {'SVForIPTool': 'SVForIPTool_IP2DTrigHybrid',
                                           'trackSelectorTool': 'IP2DTrigHybridTrackSelector',
                                           'trackGradeFactory': 'IP2DTrigHybridDetailedTrackGradeFactory',
                                           #                                       'trackToVertexTool'          : 'BTagTrackToVertexTool',
                                           'TrackToVertexIPEstimator': 'BTagTrackToVertexIPEstimator',
                                           'LikelihoodTool': 'IP2DTrigHybridNewLikelihoodTool'},
                         #                'PassByName'        : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                         #                                       'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                         #                'PassTracksAs'      : 'trackAssociation',
                         'PassTracksAs': 'trackAssociationName',
                         'JetCollectionList': 'jetCollectionList',
                         'ToolCollection': 'IP2DTrigHybridTag'}


def toolIP2DTrigHybridTag(name, useBTagFlagsDefaults=True, **options):
    """Sets up a IP2DTrigHybridTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    impactParameterView                 default: "2D"
    trackGradePartitions                default: [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
    RejectBadTracks                     default: False
    originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
    jetCollectionList                   default: BTaggingFlags.Jets
    unbiasIPEstimation                  default: False (switch to true (better!) when creating new PDFs)
    SecVxFinderName                     default: "SV1"
    UseCHypo                            default: True

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
                    'impactParameterView': '2D',
                    'trackGradePartitions': grades,
                    'RejectBadTracks': True,
                    'originalTPCollectionName': BTaggingFlags.TrackParticleCollectionName,
                    'jetCollectionList': BTaggingFlags.Jets,
                    'unbiasIPEstimation': False,
                    'UseCHypo': True,
                    'SecVxFinderName': 'SV1',
                    'storeTrackParticles': True,
                    'storeTrackParameters': True,
                    'storeIpValues': False,
                    }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    return Analysis__IPTag(**options)

#------------------------------------------------------------------


metaIP2DTrigHybridDetailedTrackGradeFactory = {
    'ToolCollection': 'IP2DTrigHybridTag'}


def toolIP2DTrigHybridDetailedTrackGradeFactory(name, useBTagFlagsDefaults=True, **options):
    """Sets up a IP2DTrigHybridDetailedTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useSharedHitInfo                    default: True
    useDetailSharedHitInfo              default: True
    useRun2TrackGrading                 default: False
    hitBLayerGrade                      default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    #                     'TrackSummaryTool'       : None,
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


metaIP2DTrigHybridBasicTrackGradeFactory = {
    'ToolCollection': 'IP2DTrigHybridTag'}


def toolIP2DTrigHybridBasicTrackGradeFactory(name, useBTagFlagsDefaults=True, **options):
    """Sets up a IP2DTrigHybridBasicTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useSharedHitInfo                    default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    #                     'TrackSummaryTool' : None,
                    'useSharedHitInfo': True}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
    return Analysis__BasicTrackGradeFactory(**options)

#------------------------------------------------------------------


metaSVForIPTool_IP2DTrigHybrid = {'ToolCollection': 'IP2DTrigHybridTag'}


def toolSVForIPTool_IP2DTrigHybrid(name, useBTagFlagsDefaults=True, **options):
    """Sets up a SVForIPTool_IP2DTrigHybrid tool and returns it.

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


metaIP2DTrigHybridTrackSelector = {'DependsOn': ['BTagTrackToVertexTool', ],
                                   'PassByPointer': {'trackToVertexTool': 'BTagTrackToVertexTool'},
                                   'ToolCollection': 'IP2DTrigHybridTag'}


def toolIP2DTrigHybridTrackSelector(name, useBTagFlagsDefaults=True, **options):
    """Sets up a IP2DTrigHybridTrackSelector tool and returns it.

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


metaIP2DTrigHybridNewLikelihoodTool = {'CalibrationFolders': ['IP2DTrigHybrid', ],
                                       'DependsOn': ['BTagCalibrationBrokerTool', ],
                                       'PassByPointer': {'calibrationTool': 'BTagCalibrationBrokerTool'},
                                       'ToolCollection': 'IP2DTrigHybridTag'}


def toolIP2DTrigHybridNewLikelihoodTool(name, useBTagFlagsDefaults=True, **options):
    """Sets up a IP2DTrigHybridNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "IP2D"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'taggerName': 'IP2DTrigHybrid'}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
