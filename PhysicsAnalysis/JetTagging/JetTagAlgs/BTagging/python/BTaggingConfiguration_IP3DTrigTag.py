# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Configuration functions for IP3DTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
from IOVDbSvc.CondDB import conddb
btagrun1=False
if conddb.dbdata == 'COMP200':
    btagrun1=True
elif conddb.isMC:
    btagrun1 = (commonGeoFlags.Run() == "RUN1" or (commonGeoFlags.Run() == "UNDEFINED" and geoFlags.isIBL() is False))

metaIP3DTag_Trig = { 'IsATagger'         : True,
                'xAODBaseName'      : 'IP3D',
                'DependsOn'         : [#'AtlasExtrapolator',
                                       #'BTagTrackToVertexTool',
                                       #'InDetVKalVxInJetTool',
                                       #'BTagFullLinearizedTrackFactory',
                                       #'BTagTrackToVertexIPEstimator',
                                       #'IP3DNewLikelihoodTool',
                                       #'IP3DTrackSelector',
                                       #'InDetTrackSelector',
                                       #'SpecialTrackAssociator',
                                       #'SVForIPTool_IP3D',
                                       #'IP3DBasicTrackGradeFactory',
                                       #'IP3DDetailedTrackGradeFactory'],
                                      ],
                'PassByPointer'     : {#'SVForIPTool'                : 'SVForIPTool_IP3D',
                                       #'trackSelectorTool'          : 'IP3DTrackSelector',
                                       #'InDetTrackSelectionTool'    : 'InDetTrackSelector',
                                       #'TrackVertexAssociationTool' : 'SpecialTrackAssociator',
                                       #'trackGradeFactory'          : 'IP3DDetailedTrackGradeFactory',
                                       #'TrackToVertexIPEstimator'   : 'BTagTrackToVertexIPEstimator',
                                       #'LikelihoodTool'             : 'IP3DNewLikelihoodTool'
                                      },
                'PassTracksAs'      : 'trackAssociationName',
                'JetCollectionList' : 'jetCollectionList',
                'ToolCollection'    : 'IP3DTag_Trig' }

def toolIP3DTag_Trig(name, useBTagFlagsDefaults = True, **options):
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

        from BTagging.BTaggingConfiguration_CommonTools import toolBTagTrackToVertexIPEstimator as toolBTagTrackToVertexIPEstimator
        trackToVertexIPEstimator = toolBTagTrackToVertexIPEstimator('TrkToVxIPEstimator')
        svForIPTool = toolSVForIPTool_IP3D('SVForIPTool')
        trackGradeFactory= toolIP3DDetailedTrackGradeFactory('IP3DDetailedTrackGradeFactory')
        trackSelectorTool = toolIP3DTrackSelector('IP3DTrackSelector')
        inDetTrackSelectionTool = toolInDetTrackSelector('InDetTrackSelector')
        likelihood = toolIP3DNewLikelihoodTool('IP3DNewLikelihoodTool')
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'jetPtMinRef'                      : BTaggingFlags.JetPtMinRef,
                     'impactParameterView'              : '3D',
                     'trackGradePartitions'             : grades ,
                     'RejectBadTracks'                  : True,
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
#                     'SecVxFinderNameForV0Removal'      : "InDetVKalVxInJetTool",
#                     'SecVxFinderNameForIPSign'         : "InDetVKalVxInJetTool",
                     'unbiasIPEstimation'               : False,
                     #'trackAssociation'                 : "Tracks" }
                     'UseCHypo'                         : True,
                     'SecVxFinderName'                  : 'SV1',
                     'storeTrackParameters': True,
                     'storeTrackParticles': True,
                     'storeIpValues': True,
                     'LikelihoodTool'                   : likelihood,
                     'trackSelectorTool'                : trackSelectorTool,
                     'InDetTrackSelectionTool'          : inDetTrackSelectionTool,
                     'SVForIPTool'                      : svForIPTool,
                     'trackGradeFactory'                : trackGradeFactory,
                     'TrackToVertexIPEstimator'         : trackToVertexIPEstimator,
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
    useRun2TrackGrading                 default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'useSharedHitInfo'       : True,
                     'useDetailSharedHitInfo' : True,
                     'useRun2TrackGrading'    : (btagrun1 is False),
                     'useInnerLayers0HitInfo' : (btagrun1 is False),
                     'useDetailSplitHitInfo'  : (btagrun1 is False),
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

metaIP3DTrackSelector = { #'DependsOn'      : ['BTagTrackToVertexTool',],
                          #'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                          'ToolCollection' : 'IP3DTag' }

def toolIP3DTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a IP3DTrackSelector tool and returns it.

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
        from BTagging.BTaggingConfiguration_CommonTools import toolBTagTrackToVertexTool as toolBTagTrackToVertexTool
        trackToVertexTool = toolBTagTrackToVertexTool('BTagTrackToVertexTool')
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'useBLayerHitPrediction' : True ,
                     'nHitBLayer'             : 0 ,
                     'usepTDepTrackSel'       : False,
                     'trackToVertexTool'      : trackToVertexTool, }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
    return Analysis__TrackSelector(**options)

#------------------------------------------------------------------

metaInDetTrackSelector = { 'ToolCollection' : 'IP3DTag' }
 
def toolInDetTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'CutLevel'               : "Loose",
                     'maxZ0SinTheta'          : 3 }
#                     'maxD0'                  : 2 }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    return InDet__InDetTrackSelectionTool(**options)

#------------------------------------------------------------------

metaSpecialTrackAssociator = { 'ToolCollection' : 'IP3DTag' }
 
def toolSpecialTrackAssociator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:
    OutputLevel                         default: BTaggingFlags.OutputLevel
    
    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'WorkingPoint'           : 'Loose' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    
    from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TrackVertexAssociationTool
    return CP__TrackVertexAssociationTool(**options)

#------------------------------------------------------------------

metaIP3DNewLikelihoodTool = { 'CalibrationTaggers' : ['IP3D',],
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
                     'HistosKey'                           : 'TrigJetTagCalibHistosKey',
                     'taggerName'                          : 'IP3D' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
