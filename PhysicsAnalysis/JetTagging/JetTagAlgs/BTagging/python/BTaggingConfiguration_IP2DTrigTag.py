# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Configuration functions for IP2DTag_Trig
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

metaIP2DTag_Trig = { 'IsATagger'         : True,
                   'xAODBaseName'      : 'IP2D',
                   'DependsOn'         : ['AtlasExtrapolator',
                                          'BTagTrackToVertexTool',
                                          'InDetVKalVxInJetTool_Trig',
                                          'BTagFullLinearizedTrackFactory',
                                          'BTagTrackToVertexIPEstimator',
                                          'IP2DNewLikelihoodTool_Trig',
                                          'IP2DTrackSelector_Trig',
                                          'SVForIPTool_IP2D_Trig',
                                          'IP2DBasicTrackGradeFactory_Trig',
                                          'IP2DDetailedTrackGradeFactory_Trig'],
                   'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP2D_Trig',
                                          'trackSelectorTool'          : 'IP2DTrackSelector_Trig',
                                          'trackGradeFactory'          : 'IP2DDetailedTrackGradeFactory_Trig',
                                          'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                          'LikelihoodTool'             : 'IP2DNewLikelihoodTool_Trig'},
                   'PassTracksAs'      : 'trackAssociationName',
                   'JetCollectionList' : 'jetCollectionList',
                   'ToolCollection'    : 'IP2DTag_Trig' }

def toolIP2DTag_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DTag_Trig tool and returns it.

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
    UseCHypo                            default: True
    SecVxFinderName                     default: "SV1"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        grades= [ "0HitIn0HitNInExp2","0HitIn0HitNInExpIn","0HitIn0HitNInExpNIn","0HitIn0HitNIn",
                  "0HitInExp", "0HitIn",
                  "0HitNInExp", "0HitNIn",
                  "InANDNInShared", "PixShared", "SctShared",
                  "InANDNInSplit", "PixSplit",
                  "Good"]
        if btagrun1: grades=[ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'jetPtMinRef'                      : BTaggingFlags.JetPtMinRef,
                     'impactParameterView'              : '2D',
                     'trackGradePartitions'             : grades,
                     'RejectBadTracks'                  : True,
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'unbiasIPEstimation'               : False,
                     'UseCHypo'                         : True,
                     'SecVxFinderName'                  : 'SV1',
                     'storeTrackParticles': True,
                     'storeTrackParameters': True,
                     'storeIpValues': False,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    return Analysis__IPTag(**options)

#---------------------------------------------------------------------

metaIP2DDetailedTrackGradeFactory_Trig = { 'ToolCollection' : 'IP2DTag_Trig' }

def toolIP2DDetailedTrackGradeFactory_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DTrigDetailedTrackGradeFactory tool and returns it.

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
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'useSharedHitInfo'       : True,
                     'useDetailSharedHitInfo' : True,
                     'useRun2TrackGrading'    : (btagrun1 == False),
                     'useInnerLayers0HitInfo' : (btagrun1 == False),
                     'useDetailSplitHitInfo'  : (btagrun1 == False),
                     'hitBLayerGrade'         : True,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
    return Analysis__DetailedTrackGradeFactory(**options)

#---------------------------------------------------------------------

metaIP2DBasicTrackGradeFactory_Trig = { 'ToolCollection' : 'IP2DTag_Trig' }

def toolIP2DBasicTrackGradeFactory_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DTrigBasicTrackGradeFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    useSharedHitInfo                    default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'      : BTaggingFlags.OutputLevel,
#                     'TrackSummaryTool' : None,
                     'useSharedHitInfo' : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__BasicTrackGradeFactory
    return Analysis__BasicTrackGradeFactory(**options)

#---------------------------------------------------------------------

metaSVForIPTool_IP2D_Trig = { 'ToolCollection' : 'IP2DTag_Trig' }

def toolSVForIPTool_IP2D_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_IP2DTrig tool and returns it.

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

metaIP2DTrackSelector_Trig = { 'DependsOn'       : ['BTagTrackToVertexTool',],
                             'PassedByPointer' : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                             'ToolCollection'  : 'IP2DTag_Trig' }

def toolIP2DTrackSelector_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DTrigTrackSelector tool and returns it.

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
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'useBLayerHitPrediction' : True,
                     'nHitBLayer'             : 0,
                     'usepTDepTrackSel'       : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
    return Analysis__TrackSelector(**options)

#---------------------------------------------------------------------

metaIP2DNewLikelihoodTool_Trig = { 'CalibrationTaggers' : ['IP2D',],
                                 'ToolCollection'     : 'IP2DTag_Trig' }

def toolIP2DNewLikelihoodTool_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP2DTrigNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "IP2D"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'HistosKey'                           : 'TrigJetTagCalibHistosKey',
                     'taggerName'                          : 'IP2D' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
