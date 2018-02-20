# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for IP3DNegTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags
from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
from IOVDbSvc.CondDB import conddb
btagrun1=False
if conddb.dbdata == 'COMP200':
    btagrun1=True
elif conddb.isMC:
    btagrun1 = (geoFlags.Run() == "RUN1" or (geoFlags.Run() == "UNDEFINED" and geoFlags.isIBL() == False))

metaIP3DNegTag = { 'IsATagger'         : True,
                   'xAODBaseName'      : 'IP3DNeg',
                   'DependsOn'         : ['AtlasExtrapolator',
                                          'BTagTrackToVertexTool',
                                          'InDetVKalVxInJetTool',
                                          'BTagFullLinearizedTrackFactory',
                                          'BTagTrackToVertexIPEstimator',
                                          'IP3DNegNewLikelihoodTool',
                                          'IP3DNegTrackSelector',
                                          'InDetTrackSelector',
                                          'SpecialTrackAssociator',
                                          'SVForIPTool_IP3DNeg',
                                          'IP3DNegBasicTrackGradeFactory',
                                          'IP3DNegDetailedTrackGradeFactory'],
                   'PassByPointer'     : {'SVForIPTool'                : 'SVForIPTool_IP3DNeg',
                                          'trackSelectorTool'          : 'IP3DNegTrackSelector',
                                          'trackGradeFactory'          : 'IP3DNegDetailedTrackGradeFactory',
#                                          'trackToVertexTool'          : 'BTagTrackToVertexTool',
                                          'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                          'InDetTrackSelectionTool'    : 'InDetTrackSelector',
                                          'TrackVertexAssociationTool' : 'SpecialTrackAssociator',
                                          'LikelihoodTool'             : 'IP3DNegNewLikelihoodTool'},
#                   'PassByName'        : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
#                                          'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                   'PassTracksAs'      : 'trackAssociationName',
                   'JetCollectionList' : 'jetCollectionList',
                   'ToolCollection'    : 'IP3DNegTag' }

def toolIP3DNegTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DNegTag tool and returns it.

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
    flipIPSign                          default: True
    usePosIP                            default: True
    useNegIP                            default: False
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
                     'flipIPSign'                       : True,
                     'flipZIPSign'                      : True,
                     'usePosIP'                         : True,
                     'useNegIP'                         : True,
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

metaIP3DNegDetailedTrackGradeFactory = { 'ToolCollection' : 'IP3DNegTag' }

def toolIP3DNegDetailedTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DNegDetailedTrackGradeFactory tool and returns it.

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

metaIP3DNegBasicTrackGradeFactory = { 'ToolCollection' : 'IP3DNegTag' }

def toolIP3DNegBasicTrackGradeFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DNegBasicTrackGradeFactory tool and returns it.

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

metaSVForIPTool_IP3DNeg = { 'ToolCollection' : 'IP3DNegTag' }

def toolSVForIPTool_IP3DNeg(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SVForIPTool_IP3DNeg tool and returns it.

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

metaIP3DNegTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                             'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                             'ToolCollection' : 'IP3DNegTag' }

def toolIP3DNegTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DNegTrackSelector tool and returns it.

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

metaIP3DNegNewLikelihoodTool = { 'CalibrationFolders' : ['IP3D',],
                                 'DependsOn'          : ['BTagCalibrationBrokerTool',],
                                 'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                                 'ToolCollection'     : 'IP3DNegTag' }

def toolIP3DNegNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DNegNewLikelihoodTool tool and returns it.

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

