# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Configuration functions for IP3DTrigTag
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

metaIP3DTag_Trig = { 'IsATagger'         : True,
                   'xAODBaseName'      : 'IP3D',
                   'DependsOn'         : ['AtlasExtrapolator',
                                          'BTagTrackToVertexTool',
                                          'InDetVKalVxInJetTool_Trig',
                                          'BTagFullLinearizedTrackFactory',
                                          'BTagTrackToVertexIPEstimator',
                                          'IP3DNewLikelihoodTool_Trig',
                                          'IP3DTrackSelector_Trig',
                                          'InDetTrackSelector',
                                          'SpecialTrackAssociator',
                                          'SVForIPTool_IP3D_Trig',
                                          'IP3DBasicTrackGradeFactory_Trig',
                                          'IP3DDetailedTrackGradeFactory_Trig'],
                   'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP3D_Trig',
                                          'trackSelectorTool'          : 'IP3DTrackSelector_Trig',
                                          'trackGradeFactory'          : 'IP3DDetailedTrackGradeFactory_Trig',
                                          'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                          'InDetTrackSelectionTool'    : 'InDetTrackSelector',
                                          'TrackVertexAssociationTool' : 'SpecialTrackAssociator',
                                          'LikelihoodTool'             : 'IP3DNewLikelihoodTool_Trig'},
                   'PassTracksAs'      : 'trackAssociationName',
                   'JetCollectionList' : 'jetCollectionList',
                   'ToolCollection'    : 'IP3DTag_Trig' }

def toolIP3DTag_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTrigTag tool and returns it.

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
                     'impactParameterView'              : '3D',
                     'trackGradePartitions'             : grades,
                     'RejectBadTracks'                  : True,
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'unbiasIPEstimation'               : False,
                     'UseCHypo'                         : True,
                     'SecVxFinderName'                  : 'SV1',
                     'jetPtMinRef'                      : BTaggingFlags.JetPtMinRef,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__IPTag
    return Analysis__IPTag(**options)

#---------------------------------------------------------------------

metaIP3DDetailedTrackGradeFactory_Trig = { 'ToolCollection' : 'IP3DTag_Trig' }

def toolIP3DDetailedTrackGradeFactory_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTrigDetailedTrackGradeFactory tool and returns it.

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
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'useSharedHitInfo'       : True,
                     'useDetailSharedHitInfo' : True,
                     'hitBLayerGrade'         : True,
                     'useRun2TrackGrading'    : (btagrun1 == False),
                     'useInnerLayers0HitInfo' : (btagrun1 == False),
                     'useDetailSplitHitInfo'  : (btagrun1 == False),
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__DetailedTrackGradeFactory
    return Analysis__DetailedTrackGradeFactory(**options)

#---------------------------------------------------------------------

metaIP3DBasicTrackGradeFactory_Trig = { 'ToolCollection' : 'IP3DTag_Trig' }

def toolIP3DBasicTrackGradeFactory_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTrigBasicTrackGradeFactory tool and returns it.

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

#---------------------------------------------------------------------

metaSVForIPTool_IP3D_Trig = { 'ToolCollection' : 'IP3DTag_Trig' }

def toolSVForIPTool_IP3D_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_IP3DTrig tool and returns it.

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

metaIP3DTrackSelector_Trig = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                             'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                             'ToolCollection' : 'IP3DTag_Trig' }

def toolIP3DTrackSelector_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTrigTrackSelector tool and returns it.

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
                     'nHitBLayer'             : 0 ,
                     'usepTDepTrackSel'       : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
    return Analysis__TrackSelector(**options)

#---------------------------------------------------------------------

metaIP3DNewLikelihoodTool_Trig = { 'CalibrationTaggers' : ['IP3D',],
                                 'ToolCollection'     : 'IP3DTag_Trig' }

def toolIP3DNewLikelihoodTool_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTrigNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "IP3D"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'HistosKey'                           : 'TrigJetTagCalibHistosKey',
                     'taggerName'                          : 'IP3D' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)

